// midi_processor.ino
//
// Reads incoming USB-MIDI packets each loop iteration.
// Handles:
//   0xF8  — MIDI clock pulse (external clock mode: drives sequencer)
//   0xFA  — MIDI Start
//   0xFC  — MIDI Stop

void read_midi() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {

      if (rx.byte1 == CLOCKBYTE) {
        if (external_clock_mode) {
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

          if (is_step_pulse && SWING > 0 && (seq.getPosition() % 2 == 0)) {
            // Transitioning to an odd step with swing — defer this pulse.
            ext_swing_pulse_pending = true;
            ext_swing_pulse_fire_us = now_us + ext_clk_avg_interval_us * (unsigned long)SWING;
          } else {
            seq.hardwareClockPulse();
          }
          // Refresh LCD tempo field with newly-averaged BPM on each step pulse.
          if (is_step_pulse) update_line1 = true;

        } else {
          // Master mode: forward outgoing clock pulse.
          clockPulse();

          clock_pulse_count++;
          if (clock_pulse_count > 5) {
            clock_pulse_count = 0;
            current_step++;
            if (current_step > 15) current_step = 0;
          }
        }

      } else if (rx.byte1 == MIDISTART) {
        Serial.println("Midi Start!");
        ext_clk_pulse_count     = 0;
        ext_clk_last_pulse_us   = 0;
        ext_swing_pulse_pending = false;
        midistarted = true;

      } else if (rx.byte1 == MIDISTOP) {
        Serial.println("Midi Stop!");
        ext_clk_pulse_count      = 0;
        ext_swing_pulse_pending  = false;
        ext_clock_start_pending  = false;
        midistopped = true;
      }
    }
  } while (rx.header != 0);
}
