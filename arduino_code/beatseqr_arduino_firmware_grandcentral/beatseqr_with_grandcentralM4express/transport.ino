// Hardware interrupt for the play button (A11 / pin 65, PULLUP).
// ISR fires on falling edge (press), sets a volatile flag with a 100 ms
// debounce guard. Main loop reads the flag and sets playbutton_flag.
// millis() is safe inside SAMD51 EIC ISRs — SysTick runs at higher priority.
volatile bool play_button_isr_fired = false;
volatile unsigned long play_button_last_isr_ms = 0;

void playButtonISR() {
  unsigned long now = millis();
  if (now - play_button_last_isr_ms >= 100) {
    play_button_last_isr_ms = now;
    play_button_isr_fired = true;
  }
}

void listen_for_transport_events() {
  if (playbutton_flag || midistarted || midistopped) {

    if (playbutton_flag && !playstatus) {
      playbutton_flag = false;
      playbutton_LED.on();
      playstatus = true;
      update_line1 = true;

      if (!external_clock_mode) clockStart();
      if (external_clock_mode) {
        ext_clk_pulse_count = 0;
        ext_swing_pulse_pending = false;
        ext_clock_start_pending = true;
      } else {
        seq.start();
        resetSequencerTimerSync();
      }
      chase_lights_status = true;

    } else if (playbutton_flag && playstatus) {
      playbutton_flag = false;
      playbutton_LED.off();
      playstatus = false;
      update_line1 = true;

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();

    } else if (midistarted) {
      midistarted = false;
      playbutton_LED.on();
      playstatus = true;
      update_line1 = true;

      if (!external_clock_mode) clockStart();
      seq.start();
      if (!external_clock_mode) resetSequencerTimerSync();
      chase_lights_status = true;

    } else if (midistopped) {
      midistopped = false;
      playbutton_LED.off();
      playstatus = false;
      update_line1 = true;

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();
      chase_lights_status = true;
    }
  }
}

// Chase light: blinks the current step's LED while playing.
// In Beatseqr the step LEDs always reflect the current_voice's step_data,
// so read_step_memory() takes current_voice (no CC-mode separate display).
void run_chase_lights(unsigned int this_step) {
  if (adv_pat_nav_active) return;

  bool editing_pat_len = config_menu_active && config_editing_value
                         && config_menu_item == CONFIG_ITEM_PAT_LENGTH;

  if (chase_lights_status == 1) {
    if (last_step != this_step) {
      if (editing_pat_len) {
        for (int i = 0; i < 16; i++) {
          if (i < pattern_length) step_leds[i].on();
          else                    step_leds[i].off();
        }
      } else {
        read_step_memory(current_voice, pattern_value);
      }
      step_leds[this_step].toggle();
      last_step = this_step;
    }
  } else {
    if (!editing_pat_len) {
      read_step_memory(current_voice, pattern_value);
    }
  }
}

// allNotesOff — send note-off for every currently-sounding voice.
// Indexed by voice (VOICE_COUNT = 8), not by step.
void allNotesOff() {
  for (int v = 0; v < VOICE_COUNT; v++) {
    if (sounding_notes[v] >= 0) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[v], 0);
      sounding_notes[v] = -1;
    }
    sounding_note_end_step[v] = -1;
    if (v != current_voice) voice_select_leds[v].off();
  }
  MidiUSB.flush();
  voice_select_leds[current_voice].on();
}

// init_shuffle — Fisher-Yates permutation for Shuffle pattern direction.
void init_shuffle() {
  for (uint8_t i = 0; i < pattern_length; i++) shuffle_order[i] = i;
  for (uint8_t i = pattern_length - 1; i > 0; i--) {
    uint8_t j = (uint8_t)random(0, i + 1);
    uint8_t tmp = shuffle_order[i];
    shuffle_order[i] = shuffle_order[j];
    shuffle_order[j] = tmp;
  }
  shuffle_pos = 0;
}

// stepsend — FifteenStep step callback.
//
// Called once per sequencer step. Loops all 8 voices: fires note-off for any
// voice whose gate expires at this step, then fires note-on for any voice that
// has a step active at play_step. Velocity == 0 silences the voice (live mute).
void stepsend(int current_step, int last_step) {

  // Compute play_step from pattern_direction.
  uint8_t play_step;
  switch (pattern_direction) {
    case 1:  // Reverse
      play_step = (uint8_t)((pattern_length - 1) - current_step);
      break;
    case 2:  // Ping-pong
      play_step = ping_pong_step;
      if (pattern_length <= 1) {
        ping_pong_step = 0;
      } else if (ping_pong_going_forward) {
        if (ping_pong_step >= pattern_length - 1) {
          ping_pong_going_forward = false;
          ping_pong_step = pattern_length - 2;
        } else {
          ping_pong_step++;
        }
      } else {
        if (ping_pong_step == 0) {
          ping_pong_going_forward = true;
          ping_pong_step = 1;
        } else {
          ping_pong_step--;
        }
      }
      break;
    case 3:  // Random
      play_step = (uint8_t)random(0, pattern_length);
      break;
    case 4:  // Shuffle
      play_step = shuffle_order[shuffle_pos];
      shuffle_pos++;
      if (shuffle_pos >= pattern_length) init_shuffle();
      break;
    case 5: {  // Even/Odd
      uint8_t half = (uint8_t)((pattern_length + 1) / 2);
      if ((uint8_t)current_step < half)
        play_step = (uint8_t)(current_step * 2);
      else
        play_step = (uint8_t)((current_step - half) * 2 + 1);
      break;
    }
    case 6:  // Inward
      if (current_step % 2 == 0)
        play_step = (uint8_t)(current_step / 2);
      else
        play_step = (uint8_t)(pattern_length - 1 - current_step / 2);
      break;
    case 7: {  // Quad
      static const uint8_t qord[4] = {0, 2, 1, 3};
      uint8_t gsz = (pattern_length >= 4) ? pattern_length / 4 : 1;
      uint8_t gi  = (uint8_t)(current_step / gsz);
      if (gi > 3) gi = 3;
      play_step = (uint8_t)(qord[gi] * gsz + current_step % gsz);
      if (play_step >= pattern_length) play_step = (uint8_t)current_step;
      break;
    }
    default:  // Forward
      play_step = (uint8_t)current_step;
      break;
  }

  run_chase_lights(play_step);

  // Note-off scan: expire any voice whose gate ends at this hardware step.
  for (int v = 0; v < VOICE_COUNT; v++) {
    if (sounding_notes[v] >= 0 && sounding_note_end_step[v] == (int8_t)current_step) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[v], 0);
      sounding_notes[v] = -1;
      sounding_note_end_step[v] = -1;
      if (v != current_voice) voice_select_leds[v].off();
    }
  }

  // Note-on scan: fire all voices that are active at play_step.
  for (int v = 0; v < VOICE_COUNT; v++) {
    if (step_data[pattern_value][v][play_step] == 1) {
      bool fires = (voice_probability[pattern_value][v] >= 100)
                   || ((uint8_t)random(100) < voice_probability[pattern_value][v]);
      if (fires) {
        // Retrigger: if this voice is still ringing, cut it before the new hit.
        if (sounding_notes[v] >= 0) {
          noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[v], 0);
          sounding_notes[v] = -1;
          sounding_note_end_step[v] = -1;
        }

        int16_t shifted = (int16_t)voice_pitch[pattern_value][v]
                        + (int16_t)(octave_shift * 12)
                        + (int16_t)note_shift;
        if (shifted < 0)   shifted = 0;
        if (shifted > 127) shifted = 127;
        uint8_t pitch = (uint8_t)shifted;
        uint8_t vel   = voice_velocity[pattern_value][v];

        // velocity == 0 means the voice is muted by the fader — skip note-on.
        if (vel > 0) {
          noteOn(MIDICHANNEL - 1, pitch, vel);
          sounding_notes[v] = (int8_t)pitch;
          sounding_note_end_step[v] =
              (int8_t)((current_step + voice_gate[pattern_value][v]) % pattern_length);
          voice_select_leds[v].on();
        }
      }
    }

    // CC: fire if this voice has CC enabled and the step is active.
    if (voice_cc_enabled[v] && step_data[pattern_value][v][play_step]) {
      controlChange(MIDICHANNEL - 1, cc_number[pattern_value],
                    voice_cc_value[pattern_value][v]);
    }
  }

  MidiUSB.flush();

  if (!config_menu_active) {
    last_triggered_step = (int8_t)play_step;
    update_line1 = true;
    update_line2 = true;
  }

  // Pattern chain auto-advance at the last step.
  if (extended_step_length_mode == 1 && current_step == (int)(pattern_length - 1)) {
    run_auto_pattern_select_routine();
  }

  // Swing: alternate TC4 period so odd steps arrive late, even steps early.
  // SWING == 0 → straight timing (multipliers cancel to 1).
  if (!external_clock_mode) {
    unsigned long base_us = 60000000UL / (unsigned long)TEMPO / 24UL;
    if (current_step % 2 == 0) {
      setSequencerTimerPeriod(base_us * (6 + SWING) / 6);
    } else {
      setSequencerTimerPeriod(base_us * (6 - SWING) / 6);
    }
  }
}
