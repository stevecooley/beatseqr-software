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

      // If the config menu is open (user stopped to satisfy "Stop first!"),
      // redraw it now so the stale "Stop first!" message clears and the user
      // can see they're ready to press Enter to save.
      if (config_menu_active) draw_config_menu();

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

  // In LV (live CC) mode, step LEDs are off except for the lane currently
  // being edited — managed by detect_step_button_presses() and set_slider_mode().
  // Skip the chase entirely so step_data state isn't repainted over our cleared LEDs.
  if (slider_mode == 6) return;

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
        if (ft_cc_mode && slider_mode == 4)         read_cc_step_memory();
        else if (ft_drift_mode && slider_mode == 7) read_drift_step_memory();
        else                                        read_step_memory(0, pattern_value);
      }
      step_leds[this_step].toggle();  // chase lights!
      last_step = this_step;
    }
  } else {
    if (!editing_pat_len) {
      if (ft_cc_mode && slider_mode == 4)         read_cc_step_memory();
      else if (ft_drift_mode && slider_mode == 7) read_drift_step_memory();
      else                                        read_step_memory(0, pattern_value);
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
    for (int n = 0; n < MAX_CHORD_NOTES; n++) {
      if (sounding_chord[i][n] >= 0) {
        noteOff(MIDICHANNEL - 1, (uint8_t)sounding_chord[i][n], 0);
        sounding_chord[i][n] = -1;
      }
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
  uint8_t play_step = (uint8_t)current_step;  // default: forward
  if (ft_pattern_direction) {
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
        uint8_t half = (uint8_t)((pattern_length + 1) / 2);
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
  }

  run_chase_lights(play_step);

  // Flash play button LED on quarter notes: on at steps 0,4,8,12 — off at 2,6,10,14.
  if (current_step % 4 == 0)      playbutton_LED.on();
  else if (current_step % 4 == 2) playbutton_LED.off();

  // Note-off scan: turn off any notes whose gate expires at this step.
  // Gate end steps are stored as hardware clock steps (current_step-based),
  // so this scan always uses current_step regardless of direction.
  // All notes of a chord share one end_step entry — walk the chord slots.
  for (int i = 0; i < 16; i++) {
    if (sounding_note_end_step[i] == (int8_t)current_step) {
      for (int n = 0; n < MAX_CHORD_NOTES; n++) {
        if (sounding_chord[i][n] >= 0) {
          noteOff(MIDICHANNEL - 1, (uint8_t)sounding_chord[i][n], 0);
          sounding_chord[i][n] = -1;
        }
      }
      sounding_note_end_step[i] = -1;
    }
  }

  if (step_data[pattern_value][0][play_step] == 1) {
    bool fires = true;
    if (ft_probability) {
      fires = (step_probability[pattern_value][play_step] >= 100)
              || ((uint8_t)random(100) < step_probability[pattern_value][play_step]);
    }
    if (fires) {
      // If this slot is still sounding (e.g. random mode re-triggered it),
      // send note-off for every chord slot before the new note-on(s).
      for (int n = 0; n < MAX_CHORD_NOTES; n++) {
        if (sounding_chord[play_step][n] >= 0) {
          noteOff(MIDICHANNEL - 1, (uint8_t)sounding_chord[play_step][n], 0);
          sounding_chord[play_step][n] = -1;
        }
      }
      sounding_note_end_step[play_step] = -1;

      // Compute root pitch: shift + scale-quantize. Drift is applied per chord
      // note below so each note in a chord can wobble independently.
      int16_t shifted = (int16_t)voice_slider_midinotenum[play_step];
      if (ft_octave_note_shift) {
        shifted += (int16_t)(octave_shift * 12) + (int16_t)note_shift;
      }
      if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0) {
        if (shifted < 0)   shifted = 0;
        if (shifted > 127) shifted = 127;
        shifted = (int16_t)quantize_to_scale((uint8_t)shifted);
      }
      if (shifted < 0)   shifted = 0;
      if (shifted > 127) shifted = 127;
      uint8_t root = (uint8_t)shifted;

      // Chord source: a captured MIDI keyboard chord (step_custom_chord) wins
      // over the standard root + step_chord_type path. Captured pitches are
      // literal MIDI numbers from the keyboard; still subject to octave/note
      // shift and scale snap so global offsets behave consistently across
      // every step regardless of source.
      uint8_t chord_out[MAX_CHORD_NOTES];
      uint8_t chord_n = 0;
      if (step_custom_chord[pattern_value][play_step][0] >= 0) {
        for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) {
          int8_t pp = step_custom_chord[pattern_value][play_step][n];
          if (pp < 0) break;
          int16_t sp = (int16_t)pp;
          if (ft_octave_note_shift) {
            sp += (int16_t)(octave_shift * 12) + (int16_t)note_shift;
          }
          if (sp < 0)   sp = 0;
          if (sp > 127) sp = 127;
          if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0) {
            sp = (int16_t)quantize_to_scale((uint8_t)sp);
          }
          chord_out[chord_n++] = (uint8_t)sp;
        }
        // Slot-0 of step_custom_chord is the lowest captured pitch; promote
        // it to root so LCD line 2 + the chord-builder fallback below see a
        // consistent value.
        root = chord_out[0];
      } else {
        // Build chord pitches from the root. step_chord_type defaults to 0
        // (single note); the build function returns just the root, so
        // single-note playback is identical to the pre-chord-mode behavior.
        chord_n = build_chord_pitches(root, step_chord_type[pattern_value][play_step], chord_out);
      }

      // Total drift = global pitch_drift (if enabled) + per-step drift (if
      // step_drift_enabled is set). Applied per chord note so clusters shimmer.
      int total_drift = 0;
      if (ft_pitch_drift) total_drift += (int)pitch_drift;
      if (ft_drift_mode && step_drift_enabled[pattern_value][play_step]) {
        total_drift += (int)step_drift_amount[pattern_value][play_step];
      }

      uint8_t vel = voice_slider_midivelocity[play_step];
      uint8_t last_pitch = root;  // shown on LCD line 2
      for (uint8_t n = 0; n < chord_n; n++) {
        int16_t p = (int16_t)chord_out[n];
        if (total_drift > 0) {
          p += (int16_t)random(-(long)total_drift, (long)total_drift + 1);
          // Clamp to MIDI range only, not slider sweep range — drift wander
          // should follow the octave/note-shifted playback position, not snap
          // back to the slider's stored range.
          if (p < 0)   p = 0;
          if (p > 127) p = 127;
          if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0)
            p = (int16_t)quantize_to_scale((uint8_t)p);
        }
        if (p < 0)   p = 0;
        if (p > 127) p = 127;
        uint8_t pitch = (uint8_t)p;
        noteOn(MIDICHANNEL - 1, pitch, vel);
        sounding_chord[play_step][n] = (int8_t)pitch;
        last_pitch = pitch;
      }

      // Schedule note-off: gate steps later in hardware clock time.
      // All notes in a chord share the same gate (one end_step entry).
      sounding_note_end_step[play_step] =
          (int8_t)((current_step + step_gate[pattern_value][play_step]) % pattern_length);

      if (!config_menu_active) {
        last_triggered_step = (int8_t)play_step;
        last_triggered_pitch = last_pitch;
        update_line1 = true;
        update_line2 = true;
      }
    }
  }

  // CC send: fire CC on this step if enabled (independent from notes).
  if (ft_cc_mode && cc_step_enabled[pattern_value][play_step]) {
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
  // SWING=0 or swing disabled → straight timing.
  if (!external_clock_mode) {
    unsigned long base_us = 60000000UL / (unsigned long)TEMPO / 24UL;
    if (ft_swing && SWING > 0) {
      if (current_step % 2 == 0) {
        setSequencerTimerPeriod(base_us * (6 + SWING) / 6);
      } else {
        setSequencerTimerPeriod(base_us * (6 - SWING) / 6);
      }
    } else {
      setSequencerTimerPeriod(base_us);
    }
  }
}
