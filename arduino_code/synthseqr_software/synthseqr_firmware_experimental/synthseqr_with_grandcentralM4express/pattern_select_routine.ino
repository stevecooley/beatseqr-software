// update_pat_nav_leds()
//
// Redraws step LEDs to reflect the current pattern-nav state.
// Called every frame while adv_pat_nav_active is true.
//   - Single pattern:  that pattern's LED blinks (adv_blink_state),
//                      all others off.
//   - Chain active:    all patterns in the chain are lit solid except the
//                      currently playing one, which blinks.
// Wrap-around chains (chain_start > chain_end) are handled correctly.
//
void update_pat_nav_leds() {
  for (int i = 0; i < 16; i++) {
    bool in_chain = false;
    if (extended_step_length_mode == 0) {
      in_chain = (i == (int)current_pattern);
    } else if (chain_start <= chain_end) {
      in_chain = (i >= (int)chain_start && i <= (int)chain_end);
    } else {
      // Wrap-around: e.g. start=7, end=2 → indices 7..15 and 0..2
      in_chain = (i >= (int)chain_start || i <= (int)chain_end);
    }

    if (!in_chain) {
      step_leds[i].off();
    } else if (i == (int)current_pattern) {
      // Blink the currently playing pattern.
      if (adv_blink_state) step_leds[i].on();
      else                 step_leds[i].off();
    } else {
      step_leds[i].on();
    }
  }
}

void run_pattern_select_routine() {
  listen_for_copy_command();

  // -----------------------------------------------------------------------
  // Advanced mode: pattern button 0 held + step tap = select pattern 0–15.
  // Pattern button 1 click = toggle pattern-nav mode (step buttons become
  // pattern selectors / chain definers instead of step editors).
  // -----------------------------------------------------------------------
#if FEATURE_ADVANCED_MODE
  if (advanced_mode) {
    // --- Slider mode selection: buttons 1=NN, 2=GT, 3=VL ---
    // set_slider_mode() arms pickup guards so physical positions can't
    // silently overwrite pattern data when switching modes.
    if (pattern_select_button_flags[1]) {
      pattern_select_button_flags[1] = false;
      set_slider_mode(1);  // NN
    }
    if (pattern_select_button_flags[2]) {
      pattern_select_button_flags[2] = false;
      set_slider_mode(3);  // GT
    }
    if (pattern_select_button_flags[3]) {
      pattern_select_button_flags[3] = false;
      set_slider_mode(2);  // VL
    }

    // LED 0: nav mode active indicator.
    // LEDs 1/2/3: show which slider mode is currently active.
    if (adv_pat_nav_active) pattern_select_leds[0].on();
    else                    pattern_select_leds[0].off();
    if (slider_mode == 1) pattern_select_leds[1].on(); else pattern_select_leds[1].off();
    if (slider_mode == 3) pattern_select_leds[2].on(); else pattern_select_leds[2].off();
    if (slider_mode == 2) pattern_select_leds[3].on(); else pattern_select_leds[3].off();

    // --- Pattern-nav mode: step buttons select/chain patterns ---
    if (adv_pat_nav_active) {
      // Update blink timer (200 ms period).
      unsigned long nav_now = millis();
      if (nav_now - adv_blink_last_ms >= 200) {
        adv_blink_last_ms = nav_now;
        adv_blink_state = !adv_blink_state;
      }

      // Release tracking: clear held button once it's physically released.
      if (adv_chain_hold_step >= 0 &&
          !step_buttons[adv_chain_hold_step].wasPressed()) {
        adv_chain_hold_step = -1;
      }

      // Scan for step button presses (uniquePress() not called elsewhere
      // this frame because detect_step_button_presses() is gated off).
      for (int i = 0; i < 16; i++) {
        if (step_buttons[i].uniquePress()) {
          if (adv_chain_hold_step < 0) {
            // First press — single pattern select, cancel any active chain.
            adv_chain_hold_step = i;
            extended_step_length_mode = 0;
            go_to_pattern(i, 0);
            Serial.print("nav: single pattern ");
            Serial.println(i + 1);
          } else if (i != adv_chain_hold_step) {
            // Second press while first is still held — define a chain.
            // chain_start is the held button, chain_end is this button.
            // Wrap-around is supported (start > end).
            chain_start = (uint8_t)adv_chain_hold_step;
            chain_end   = (uint8_t)i;
            extended_step_length_mode = 1;
            go_to_pattern(chain_start, 0);
            Serial.print("nav: chain ");
            Serial.print(chain_start + 1);
            Serial.print(" -> ");
            Serial.println(chain_end + 1);
          }
          break;
        }
      }

      // Redraw step LEDs: chain patterns solid, current pattern blinks.
      update_pat_nav_leds();
      return;
    }
  }
#endif  // FEATURE_ADVANCED_MODE

  // -----------------------------------------------------------------------
  // Simple mode: pattern buttons 0–3 select patterns 0–3 directly.
  // -----------------------------------------------------------------------
  if (!advanced_mode) {
    for (int pattern = 0; pattern < 4; pattern++) {
      if (pattern_select_button_flags[pattern] == true) {
        pattern_select_button_flags[pattern] = false;
        go_to_pattern(pattern, 0);
      }
    }
  }

  // -----------------------------------------------------------------------
  // Simple mode chain toggle: buttons 0+3 simultaneously.
  // Sets chain_start=0, chain_end=3 (the original 4-pattern loop).
  // -----------------------------------------------------------------------
  if (!advanced_mode) {
    static bool chain_toggle_handled = false;
    // Non-blocking LED cascade state.
    // chain_anim_dir=1: forward (0→1→2→3→settle) for chain-on.
    // chain_anim_dir=-1: reverse (3→2→1→settle) for chain-off.
    static int  chain_anim_step    = -1;  // -1 = inactive
    static int  chain_anim_dir     =  1;
    static unsigned long chain_anim_last_ms = 0;

    // Advance cascade animation each frame.
    if (chain_anim_step >= 0) {
      unsigned long now = millis();
      if (now - chain_anim_last_ms >= 60) {
        chain_anim_last_ms = now;
        if (chain_anim_dir == 1) {
          // Forward: light LEDs 1→2→3, then settle (turn 1-3 off).
          if (chain_anim_step <= 3) {
            pattern_select_leds[chain_anim_step].on();
            chain_anim_step++;
          } else {
            pattern_select_leds[1].off();
            pattern_select_leds[2].off();
            pattern_select_leds[3].off();
            chain_anim_step = -1;
          }
        } else {
          // Reverse: turn off LEDs 3→2→1, then settle (LED 0 stays on).
          if (chain_anim_step >= 1) {
            pattern_select_leds[chain_anim_step].off();
            chain_anim_step--;
          } else {
            chain_anim_step = -1;
          }
        }
      }
    }

    if (pattern_select_buttons[0].wasPressed() &&
        pattern_select_buttons[3].wasPressed()) {
      if (!chain_toggle_handled) {
        chain_toggle_handled = true;
        if (extended_step_length_mode == 1) {
          lcdflag = 200;  next_lcdflag = 200;  // single
          extended_step_length_mode = 0;
          // go_to_pattern first: lights LED 0, clears LEDs 1-3.
          go_to_pattern(0, 1);
          // Reverse cascade: re-light LEDs 1-3, then turn them off 3→2→1.
          pattern_select_leds[1].on();
          pattern_select_leds[2].on();
          pattern_select_leds[3].on();
          chain_anim_dir     = -1;
          chain_anim_step    = 3;
          chain_anim_last_ms = millis();
        } else {
          lcdflag = 201;  next_lcdflag = 201;  // chain 4
          extended_step_length_mode = 1;
          chain_start = 0;
          chain_end   = 3;
          // Forward cascade: go_to_pattern sets LED 0 on; cascade adds 1-3.
          chain_anim_dir     = 1;
          chain_anim_step    = 1;
          chain_anim_last_ms = millis();
          go_to_pattern(0, 1);
        }
      }
    } else {
      chain_toggle_handled = false;
    }
  }
}

void run_auto_pattern_select_routine() {
  // Advance within chain_start..chain_end, supporting wrap-around ranges.
  // A wrapping chain has chain_start > chain_end (e.g. start=7, end=2 →
  // plays 7,8,9,10,11,12,13,14,15,0,1,2).
  if (chain_start <= chain_end) {
    // Normal sequential range — no wrap.
    if (current_pattern >= chain_end || current_pattern < chain_start) {
      current_pattern = chain_start;
    } else {
      current_pattern++;
    }
  } else {
    // Wrap-around range.
    bool in_range = (current_pattern >= chain_start) ||
                    (current_pattern <= chain_end);
    if (!in_range || current_pattern == chain_end) {
      current_pattern = chain_start;
    } else {
      current_pattern = (current_pattern + 1) % 16;
    }
  }
  go_to_pattern(current_pattern, 1);
}

void go_to_pattern(int pattern, int silent) {
  // LED 0 stays on while pattern-nav mode is active; don't clear it here.
  if (!adv_pat_nav_active) pattern_select_leds[0].off();
  pattern_select_leds[1].off();
  pattern_select_leds[2].off();
  pattern_select_leds[3].off();
  // In simple mode, light the LED for the active pattern.
  // In advanced mode, LEDs are function-key indicators — don't light on switch.
  if (!advanced_mode) {
    pattern_select_leds[pattern % 4].on();
  }

  // When switching to a different pattern, restore its saved pitches and
  // velocities, and arm pickup so NN-mode sliders don't immediately overwrite.
  if (pattern != pattern_value) {
    for (int step = 0; step <= 15; step++) {
      voice_slider_midinotenum[step] = pattern_step_pitches[pattern][step];
      voice_slider_midivelocity[step] = pattern_step_velocities[pattern][step];
      slider_needs_pickup[step] = true;
    }
  }

  pattern_value = pattern;
  update_line1 = true;
  // pattern_select_button_pressing_counter = 0;

  // "P"age / pattern "S"elect
  /*
  the_serial_message = "ZPS,";
  the_serial_message += pattern_value;
  the_serial_message += ";";
  serial_printer(the_serial_message);
*/
  current_pattern = pattern_value;

  // In pattern nav mode the step LEDs display pattern/chain selection,
  // not step on/off data. The nav routine redraws them each frame.
  if (!adv_pat_nav_active) {
#if FEATURE_CC_MODE
    if (slider_mode == 4) {
      read_cc_step_memory();
    } else
#endif
    {
      for (int voice = 0; voice < 1; voice++)  // synthseqr configuration
      {
        for (int step = 0; step <= 15; step++) {
          step_value = step_data[pattern][voice][step];

          if (step_value == 1) {
            step_leds[step].on();
          } else {
            step_leds[step].off();
          }
        }
      }
    }
  }
}

void listen_for_copy_command() {
  // return; // let's just not, right now.

  int ended_on;

  if (told_which_pattern_to_copy_to ==
      true)  // we were told to copy the current pattern to another pattern
  {
    for (int i = 0; i < 4; i++) {
      if (pattern_select_button_flags[i] == true) {
        pattern_select_button_flags[i] = false;

        copy_pattern_to = i;  // for lcdflag 101

        pattern_select_leds[0].off();
        pattern_select_leds[1].off();
        pattern_select_leds[2].off();
        pattern_select_leds[3].off();

        pattern_select_leds[i].on();

        for (int voice = 0; voice < 1; voice++)  // synthseqr configuration
        {
          for (int step = 0; step <= 15; step++) {
            step_data[copy_pattern_to][voice][step] =
                step_data[current_pattern][voice][step];
            pattern_step_pitches[copy_pattern_to][step] =
                pattern_step_pitches[current_pattern][step];
            pattern_step_velocities[copy_pattern_to][step] =
                pattern_step_velocities[current_pattern][step];
            step_gate[copy_pattern_to][step] =
                step_gate[current_pattern][step];
#if FEATURE_CC_MODE
            cc_step_enabled[copy_pattern_to][step] =
                cc_step_enabled[current_pattern][step];
            cc_step_values[copy_pattern_to][step] =
                cc_step_values[current_pattern][step];
#endif
            step_probability[copy_pattern_to][step] =
                step_probability[current_pattern][step];
          }
        }
#if FEATURE_CC_MODE
        cc_number[copy_pattern_to] = cc_number[current_pattern];
#endif
        told_which_pattern_to_copy_to = false;
        ended_on = copy_pattern_to;

        go_to_pattern(copy_pattern_to, 0);
        lcdflag = 101;  next_lcdflag = 101;  // pattern copy to N
      }
    }
  }
}

void listen_for_extended_step_length_command() {
  int ended_on;

  if (not_told_which_pattern_mode_to_use ==
      false)  // we were told to use a different pattern mode
  {
    for (int pattern = 0; pattern <= 3; pattern++) {
      if (pattern_select_buttons[pattern].uniquePress()) {
        pattern_select_leds[0].off();
        pattern_select_leds[1].off();
        pattern_select_leds[2].off();
        pattern_select_leds[3].off();
        // pattern_select_leds[pattern].blink(250,3);
        pattern_select_leds[pattern].on();
        // pattern_select_button_pressing_counter = 0;

        not_told_which_pattern_mode_to_use = true;
        extended_step_length_mode = 1;
        patterns_to_play_in_a_row = pattern;
      }
    }
    // "P"age / pattern "S"elect
  }
}
