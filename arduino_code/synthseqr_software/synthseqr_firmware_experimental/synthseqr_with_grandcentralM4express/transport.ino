// Hardware interrupt for the play button (pin 21, PULLUP).
// The ISR fires on the falling edge (press) and sets a volatile flag with a
// 50 ms debounce guard. The main loop reads the flag and sets playbutton_flag.
// millis() is safe to read here on SAMD51 — SysTick runs at higher priority
// than user interrupt lines so it keeps incrementing inside the EIC ISR.
volatile bool play_button_isr_fired = false;
volatile unsigned long play_button_last_isr_ms = 0;

void playButtonISR() {
  unsigned long now = millis();
  if (now - play_button_last_isr_ms >= 100) {
    play_button_last_isr_ms = now;
    play_button_isr_fired = true;
  }
}

void listen_for_delay_tasks() {}

void listen_for_transport_events() {
  if (playbutton_flag == true || midistarted == true || midistopped == true) {
    // now we'll check the playstatus to see if it's false.
    // this means that we're not playing anything.
    if (playbutton_flag == true && playstatus == false) {
      // stand down the transport button pressed flag
      playbutton_flag = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing stuff
      playstatus = true;
      update_line1 = true;

      // In external clock mode the host is the master; don't send transport.
      if (!external_clock_mode) clockStart();
      if (external_clock_mode) {
        // Clock is still running — wait for the next beat boundary so we
        // start in phase. Reset pulse count so we get a clean 6-pulse window,
        // then arm the pending flag; read_midi() will call seq.start() on the
        // next step pulse.
        ext_clk_pulse_count = 0;
        ext_swing_pulse_pending = false;
        ext_clock_start_pending = true;
      } else {
        // start the sequencer
        seq.start();
        // Synchronise TC4 counter (internal mode only; TC4 is stopped in ext mode).
        resetSequencerTimerSync();
      }

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    } else if (playbutton_flag == true && playstatus == true) {
      // stand down the transport button pressed flag
      playbutton_flag = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;
      update_line1 = true;

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();

    } else if (midistarted == true) {
      // stand down the event flag, we got this
      midistarted = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing
      playstatus = true;
      update_line1 = true;

      if (!external_clock_mode) clockStart();
      // start the sequencer
      seq.start();
      if (!external_clock_mode) resetSequencerTimerSync();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    } else if (midistopped == true) {
      // stand down the event flag, we got this
      midistopped = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;
      update_line1 = true;

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    }
  }
}

void run_chase_lights(unsigned int this_step) {
  // In pattern nav mode, step LEDs display pattern selection/chain state.
  // The nav routine manages them; skip chase light processing entirely.
  if (adv_pat_nav_active) return;

  // While the config menu PAT_LENGTH editor is active, the step LEDs show
  // the pattern-length indicator (0..N-1 lit). Re-apply it before the chase
  // toggle so incoming steps don't overwrite it with read_step_memory().
  bool editing_pat_len = config_menu_active && config_editing_value
                         && config_menu_item == CONFIG_ITEM_PAT_LENGTH;

  if (chase_lights_status == 1) {
    if (last_step !=
        this_step)  // clock pulses counted so we can advance to the next step.
    {
      if (editing_pat_len) {
        // Re-apply the pattern-length indicator, then blink the current step.
        for (int i = 0; i < 16; i++) {
          if (i < pattern_length) step_leds[i].on();
          else                    step_leds[i].off();
        }
      } else {
        // clear the LEDs back to their data (mode-aware)
        if (slider_mode == 4) read_cc_step_memory();
        else                  read_step_memory(0, pattern_value);
      }
      step_leds[this_step].toggle();  // chase lights!
      last_step = this_step;
    }
  } else {
    if (!editing_pat_len) {
      if (slider_mode == 4) read_cc_step_memory();
      else                  read_step_memory(0, pattern_value);
    }
  }
}

// allNotesOff()
//
// Sends note-off for every step that is currently sounding, using the exact
// pitch that was sent in the note-on. Safe to call on stop, pattern change,
// or any other situation where notes might be left open.
//
void allNotesOff() {
  for (int i = 0; i < 16; i++) {
    if (sounding_notes[i] >= 0) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[i], 0);
      sounding_notes[i] = -1;
    }
    sounding_note_end_step[i] = -1;
  }
  MidiUSB.flush();
}

// Fill shuffle_order[0..pattern_length-1] with a random permutation using
// Fisher-Yates, then reset shuffle_pos to 0. Called when entering Shuf mode,
// when pattern_length changes while in Shuf mode, and at the end of each
// completed permutation cycle so the next pass is freshly randomised.
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

void stepsend(int current_step, int last_step) {
  // Compute play_step: the data index to read notes/velocities/gates from.
  // current_step is always 0..(pattern_length-1) from FifteenStep.
  // play_step maps that to the actual step position based on direction.
  uint8_t play_step;
  switch (pattern_direction) {
    case 1: // Reverse
      play_step = (uint8_t)((pattern_length - 1) - current_step);
      break;
    case 2: // Ping-pong — use the maintained virtual position
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
    case 3: // Random — independent random step each tick
      play_step = (uint8_t)random(0, pattern_length);
      break;
    case 4: // Shuffle — random permutation; each step plays once before reshuffling
      play_step = shuffle_order[shuffle_pos];
      shuffle_pos++;
      if (shuffle_pos >= pattern_length) init_shuffle();
      break;
    case 5: { // Even/Odd — all even-indexed steps then all odd-indexed steps
      uint8_t half = (uint8_t)((pattern_length + 1) / 2);  // ceil(N/2) even steps first
      if ((uint8_t)current_step < half)
        play_step = (uint8_t)(current_step * 2);
      else
        play_step = (uint8_t)((current_step - half) * 2 + 1);
      break;
    }
    case 6: // Inward — outside-in: 0, N-1, 1, N-2, 2, N-3, ...
      if (current_step % 2 == 0)
        play_step = (uint8_t)(current_step / 2);
      else
        play_step = (uint8_t)(pattern_length - 1 - current_step / 2);
      break;
    case 7: { // Quad — plays Q1, Q3, Q2, Q4 (quarter-groups reordered)
      static const uint8_t qord[4] = {0, 2, 1, 3};
      uint8_t gsz = (pattern_length >= 4) ? pattern_length / 4 : 1;
      uint8_t gi  = (uint8_t)(current_step / gsz);
      if (gi > 3) gi = 3;
      play_step = (uint8_t)(qord[gi] * gsz + current_step % gsz);
      if (play_step >= pattern_length) play_step = (uint8_t)current_step;
      break;
    }
    default: // Forward
      play_step = (uint8_t)current_step;
      break;
  }

  run_chase_lights(play_step);

  // Note-off scan: turn off any notes whose gate expires at this step.
  // Gate end steps are stored as hardware clock steps (current_step-based),
  // so this scan always uses current_step regardless of direction.
  for (int i = 0; i < 16; i++) {
    if (sounding_notes[i] >= 0 && sounding_note_end_step[i] == (int8_t)current_step) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[i], 0);
      sounding_notes[i] = -1;
      sounding_note_end_step[i] = -1;
    }
  }

  if (step_data[pattern_value][0][play_step] == 1) {
    // If this slot is still sounding (e.g. random mode re-triggered it),
    // send note-off before the new note-on.
    if (sounding_notes[play_step] >= 0) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[play_step], 0);
      sounding_notes[play_step] = -1;
      sounding_note_end_step[play_step] = -1;
    }
    int16_t shifted = (int16_t)voice_slider_midinotenum[play_step] + (int16_t)(octave_shift * 12) + (int16_t)note_shift;
    if (shifted < 0) shifted = 0;
    if (shifted > 127) shifted = 127;
    uint8_t pitch = (uint8_t)shifted;
    uint8_t vel = voice_slider_midivelocity[play_step];
    noteOn(MIDICHANNEL - 1, pitch, vel);
    sounding_notes[play_step] = (int8_t)pitch;
    // Schedule note-off: gate steps later in hardware clock time
    sounding_note_end_step[play_step] =
        (int8_t)((current_step + step_gate[pattern_value][play_step]) % pattern_length);
    // Update LCD lines with this step's trigger info —
    // but only when the config menu isn't using line 2.
    if (!config_menu_active) {
      last_triggered_step = (int8_t)play_step;
      update_line1 = true;  // step counter on line 1
      update_line2 = true;
    }
  }

  // CC send: fire CC on this step if enabled (independent from notes).
  if (cc_step_enabled[pattern_value][play_step]) {
    controlChange(MIDICHANNEL - 1, cc_number[pattern_value], cc_step_values[pattern_value][play_step]);
    if (!config_menu_active) {
      last_triggered_step = (int8_t)play_step;
      update_line1 = true;
      update_line2 = true;
    }
  }

  // Auto-advance to the next pattern at the last step of the pattern when
  // chain mode is active. Uses pattern_length instead of the hard-coded 15.
  if (extended_step_length_mode == 1 && current_step == (int)(pattern_length - 1)) {
    run_auto_pattern_select_routine();
  }

  // Apply swing by alternating TC4 clock periods.
  // FifteenStep's setShuffle() is a no-op in hardware timer mode, so we
  // implement swing here: even steps get a longer gap to the next (odd) step,
  // odd steps get a shorter gap back. Total time per pair stays the same.
  // SWING=0 → straight (both multipliers = 6/6 = 1). SWING=6 → extreme.
  if (!external_clock_mode) {
    unsigned long base_us = 60000000UL / (unsigned long)TEMPO / 24UL;
    if (current_step % 2 == 0) {
      setSequencerTimerPeriod(base_us * (6 + SWING) / 6);
    } else {
      setSequencerTimerPeriod(base_us * (6 - SWING) / 6);
    }
  }
}
