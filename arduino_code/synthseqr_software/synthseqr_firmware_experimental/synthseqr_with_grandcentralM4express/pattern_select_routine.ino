void run_pattern_select_routine() {
  listen_for_copy_command();

  // -----------------------------------------------------------------------
  // Advanced mode: pattern button 0 held + step tap = select pattern 0–15.
  // While button 0 is held, step buttons are consumed here and do NOT toggle
  // steps (run_step_button_routine() gates detect_step_button_presses() on
  // adv_pat_select_active).
  // -----------------------------------------------------------------------
  if (advanced_mode) {
    adv_pat_select_active = pattern_select_buttons[0].wasPressed();
    if (adv_pat_select_active) {
      // Light LED 0 to indicate pattern-select mode is armed.
      pattern_select_leds[0].on();
      pattern_select_leds[1].off();
      pattern_select_leds[2].off();
      pattern_select_leds[3].off();
      for (int i = 0; i < 16; i++) {
        if (step_buttons[i].uniquePress()) {
          go_to_pattern(i, 0);
          read_step_memory(0, i);
          break;
        }
      }
      // Consume all pattern button flags so simple-mode select doesn't fire.
      for (int i = 0; i < 4; i++) pattern_select_button_flags[i] = false;
      return;
    } else {
      adv_pat_select_active = false;
      // In advanced mode, no LED lit while idle — buttons are function keys.
      pattern_select_leds[0].off();
      pattern_select_leds[1].off();
      pattern_select_leds[2].off();
      pattern_select_leds[3].off();
    }
  }

  // -----------------------------------------------------------------------
  // Simple mode: pattern buttons 0–3 select patterns 0–3 directly.
  // -----------------------------------------------------------------------
  if (!advanced_mode) {
    for (int pattern = 0; pattern < 4; pattern++) {
      if (pattern_select_button_flags[pattern] == true) {
        pattern_select_button_flags[pattern] = false;
        go_to_pattern(pattern, 0);
        read_step_memory(0, pattern);
      }
    }
  }

  // -----------------------------------------------------------------------
  // Simple mode chain toggle: buttons 0+3 simultaneously.
  // Sets chain_start=0, chain_end=3 (the original 4-pattern loop).
  // -----------------------------------------------------------------------
  if (!advanced_mode) {
    static bool chain_toggle_handled = false;
    if (pattern_select_buttons[0].wasPressed() &&
        pattern_select_buttons[3].wasPressed()) {
      if (!chain_toggle_handled) {
        chain_toggle_handled = true;
        if (extended_step_length_mode == 1) {
          lcdflag = 200;  next_lcdflag = 200;  // single
          extended_step_length_mode = 0;
        } else {
          lcdflag = 201;  next_lcdflag = 201;  // chain 4
          extended_step_length_mode = 1;
          chain_start = 0;
          chain_end   = 3;
        }
        go_to_pattern(0, 1);
      }
    } else {
      chain_toggle_handled = false;
    }
  }
}

void run_auto_pattern_select_routine() {
  // Advance within chain_start..chain_end range, wrapping at chain_end.
  if (current_pattern >= chain_end || current_pattern < chain_start) {
    current_pattern = chain_start;
  } else {
    current_pattern++;
  }
  go_to_pattern(current_pattern, 1);
}

void go_to_pattern(int pattern, int silent) {
  pattern_select_leds[0].off();
  pattern_select_leds[1].off();
  pattern_select_leds[2].off();
  pattern_select_leds[3].off();
  // In simple mode, light the LED for the active pattern.
  // In advanced mode, LEDs are function-key indicators — don't light on switch.
  if (!advanced_mode) {
    pattern_select_leds[pattern % 4].on();
  }

  // When switching to a different pattern, restore its saved pitches and arm
  // pickup so sliders don't immediately overwrite them.
  if (pattern != pattern_value) {
    for (int step = 0; step <= 15; step++) {
      voice_slider_midinotenum[step] = pattern_step_pitches[pattern][step];
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
          }
        }
        told_which_pattern_to_copy_to = false;
        ended_on = copy_pattern_to;

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
