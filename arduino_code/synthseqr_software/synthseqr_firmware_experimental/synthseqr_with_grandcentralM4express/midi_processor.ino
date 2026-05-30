bool print_message(void *msg)
{
  const char *m = (const char *)msg;
  Serial.print("print_message: ");
  Serial.println(m);
  //  noteOff(0, 48, 64);   // Channel 0, middle C, normal velocity

  return true; // repeat? true
}

void read_midi()
{

  midiEventPacket_t rx;
  do
  {
    rx = MidiUSB.read();
    if (rx.header != 0)
    {

      // MIDI thru: forward all incoming packets downstream.
      // In external clock mode this passes the incoming clock (0xF8),
      // Start (0xFA), and Stop (0xFC) to any device listening on this
      // USB port. In internal clock mode, 0xF8 is not received (we're
      // the master) so there is no feedback loop.
      MidiUSB.sendMIDI(rx);
      MidiUSB.flush();

      /*
        Serial.print("Received: ");
        Serial.print(rx.header, HEX);
        Serial.print("-");
        Serial.print(rx.byte1, HEX);
        Serial.print("-");
        Serial.print(rx.byte2, HEX);
        Serial.print("-");
        Serial.println(rx.byte3, HEX);
      */
      if (rx.byte1 == CLOCKBYTE)
      {
        if (external_clock_mode)
        {
          // Slave mode: incoming 0xF8 drives the sequencer.
          // When SWING > 0, odd-step transitions are deferred by
          // avg_interval * SWING µs to replicate internal-clock swing feel.
          unsigned long now_us = micros();

          // Update running average pulse interval (IIR: 7/8 old + 1/8 new).
          if (ext_clk_last_pulse_us > 0) {
            unsigned long interval = now_us - ext_clk_last_pulse_us;
            ext_clk_avg_interval_us = (ext_clk_avg_interval_us * 7 + interval) >> 3;
          }
          ext_clk_last_pulse_us = now_us;

          // Count pulses per step (0–5). The 6th pulse advances the step.
          ext_clk_pulse_count++;
          bool is_step_pulse = (ext_clk_pulse_count >= 6);
          if (is_step_pulse) ext_clk_pulse_count = 0;

          if (is_step_pulse) {
            // If play was pressed while clock was running, arm the sequencer
            // now on this beat boundary so we start in phase.
            if (ext_clock_start_pending) {
              seq.start();
              ext_clock_start_pending = false;
            }
          }

          if (ft_swing && is_step_pulse && SWING > 0 && (seq.getPosition() % 2 == 0)) {
            // Transitioning to an odd step with swing — defer this pulse.
            ext_swing_pulse_pending = true;
            ext_swing_pulse_fire_us = now_us + ext_clk_avg_interval_us * (unsigned long)SWING;
          } else {
            seq.hardwareClockPulse();
          }
          // Refresh LCD tempo field with newly-averaged BPM on each step pulse.
          if (is_step_pulse) update_line1 = true;
        }
        else
        {
          // Master mode: forward our own outgoing clock pulse.
          clockPulse();

          clock_pulse_count++;
          if (clock_pulse_count > 5)
          {
            clock_pulse_count = 0;
            current_step++;
            if (current_step > 15)
            {
              current_step = 0;
            }
          }
        }
      }
      else if (rx.byte1 == MIDISTART)
      {
        Serial.println("Midi Start!");
        // Reset external clock swing state so the interval tracker starts fresh.
        ext_clk_pulse_count     = 0;
        ext_clk_last_pulse_us   = 0;
        ext_swing_pulse_pending = false;
        // Only let incoming transport control the sequencer in external clock mode.
        if (external_clock_mode) midistarted = true;
      }
      else if (rx.byte1 == MIDISTOP)
      {
        Serial.println("Midi Stop!");
        // Discard any pending deferred pulse so it doesn't fire after stop.
        ext_clk_pulse_count      = 0;
        ext_swing_pulse_pending  = false;
        ext_clock_start_pending  = false;
        // Only let incoming transport control the sequencer in external clock mode.
        if (external_clock_mode) midistopped = true;
      }
      else if ((rx.header & 0x0F) == 0x0B)
      {
        // MIDI Learn: capture controller number when the user is editing a CC#.
        // Channel is ignored. Filtered CCs (32, 96–101) are skipped; edit stays armed.
        uint8_t cc_num = rx.byte2 & 0x7F;
        bool is_valid = (cc_num >= 1 && cc_num <= 119 && cc_num != 32 && (cc_num < 96 || cc_num > 101));
        if (is_valid) {
          if (config_menu_active && config_editing_value && config_menu_item == CONFIG_ITEM_CC_NUMBER) {
            cc_number[pattern_value] = cc_num;
            update_line1 = true;
          }
          else if (slider_mode == 6 && live_cc_editing_lane >= 0) {
            live_cc_number[live_cc_editing_lane] = cc_num;
            live_cc_last_sent[live_cc_editing_lane] = 255;
            update_line1 = true;
            update_line2 = true;
          }
        }
      }
      else if (ft_midi_program_mode && midi_capture_step >= 0 &&
               ((rx.header & 0x0F) == 0x09 || (rx.header & 0x0F) == 0x08))
      {
        // MIDI keyboard programming: accumulate notes onto the held step.
        // Note-on with velocity 0 is treated as note-off per MIDI convention.
        // Channel is ignored. The buffer commits to step_custom_chord in
        // detect_step_button_presses() when the step button is released.
        uint8_t cin   = rx.header & 0x0F;
        uint8_t pitch = rx.byte2  & 0x7F;
        uint8_t vel   = rx.byte3  & 0x7F;
        bool is_on  = (cin == 0x09 && vel > 0);
        bool is_off = (cin == 0x08) || (cin == 0x09 && vel == 0);

        if (is_on) {
          // If every keyboard note had been released before this press AND
          // the buffer already has content, treat the new note as the start
          // of a replacement chord — wipe the buffer first. Lets the user
          // fumble, lift everything, and replay during the same step hold.
          bool held_was_empty = true;
          for (uint8_t b = 0; b < 16; b++) {
            if (midi_capture_held[b]) { held_was_empty = false; break; }
          }
          if (held_was_empty && midi_capture_count > 0) {
            for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) midi_capture_buf[n] = -1;
            midi_capture_count = 0;
          }
          midi_capture_held[pitch >> 3] |= (uint8_t)(1u << (pitch & 7));

          // Insertion-sort (ascending) with dedupe; cap at MAX_CHORD_NOTES.
          bool exists = false;
          for (uint8_t n = 0; n < midi_capture_count; n++) {
            if (midi_capture_buf[n] == (int8_t)pitch) { exists = true; break; }
          }
          if (!exists && midi_capture_count < MAX_CHORD_NOTES) {
            uint8_t ins = midi_capture_count;
            for (uint8_t n = 0; n < midi_capture_count; n++) {
              if (midi_capture_buf[n] > (int8_t)pitch) { ins = n; break; }
            }
            for (int8_t n = (int8_t)midi_capture_count; n > (int8_t)ins; n--) {
              midi_capture_buf[n] = midi_capture_buf[n - 1];
            }
            midi_capture_buf[ins] = (int8_t)pitch;
            midi_capture_count++;
            if (midi_capture_count == 1) midi_capture_first_velocity = vel;
            midi_capture_dirty = true;
          }
        } else if (is_off) {
          midi_capture_held[pitch >> 3] &= (uint8_t)~(1u << (pitch & 7));
        }
      }
    }

  } while (rx.header != 0);
}

/*
   MIDIUSB_test.ino

   Created: 4/6/2015 10:47:08 AM
   Author: gurbrinder grewal
   Modified by Arduino LLC (2015)
*/

#include <MIDIUSB.h>

// Modify the noteOn and noteOff functions
void noteOn(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t thenote = {0x08, (uint8_t)(0x90 | channel), pitch, velocity};
    MidiUSB.sendMIDI(thenote);
    MidiUSB.flush();
    trs_write(0x90 | channel);
    trs_write(pitch);
    trs_write(velocity);
}

void noteOff(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t thenote = {0x08, (uint8_t)(0x80 | channel), pitch, 0};
    MidiUSB.sendMIDI(thenote);
    MidiUSB.flush();
    trs_write(0x80 | channel);
    trs_write(pitch);
    trs_write(0);
}

void clockStop() {
    midiEventPacket_t stopMsg = {0x0, 0xFC, 0x0, 0x0};
    MidiUSB.sendMIDI(stopMsg);
    midiEventPacket_t sppMsg  = {0x03, 0xF2, 0x00, 0x00};
    MidiUSB.sendMIDI(sppMsg);
    MidiUSB.flush();
    trs_write(0xFC);
    trs_write(0xF2); trs_write(0x00); trs_write(0x00);
}

void clockStart() {
    midiEventPacket_t startMsg = {0x0, 0xFA, 0x0, 0x0};
    MidiUSB.sendMIDI(startMsg);
    MidiUSB.flush();
    trs_write(0xFA);
}

void controlChange(byte channel, byte controller, byte value) {
    midiEventPacket_t msg = {0x0B, (uint8_t)(0xB0 | channel), controller, value};
    MidiUSB.sendMIDI(msg);
    MidiUSB.flush();
    trs_write(0xB0 | channel);
    trs_write(controller);
    trs_write(value);
}

void clockPulse() {
    midiEventPacket_t pulseMsg = {0x0, 0xF8, 0x0, 0x0};
    MidiUSB.sendMIDI(pulseMsg);
    MidiUSB.flush();
    trs_write(0xF8);
}