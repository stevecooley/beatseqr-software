// update_pat_nav_leds — redraws step LEDs for pattern-nav mode.
// Same logic as Synthseqr: chain patterns solid, currently-playing blinks.
void update_pat_nav_leds() {
  for (int i = 0; i < 16; i++) {
    bool in_chain = false;
    if (extended_step_length_mode == 0) {
      in_chain = (i == (int)current_pattern);
    } else if (chain_start <= chain_end) {
      in_chain = (i >= (int)chain_start && i <= (int)chain_end);
    } else {
      in_chain = (i >= (int)chain_start || i <= (int)chain_end);
    }

    if (!in_chain) {
      step_leds[i].off();
    } else if (i == (int)current_pattern) {
      if (adv_blink_state) step_leds[i].on();
      else                 step_leds[i].off();
    } else {
      step_leds[i].on();
    }
  }
}

void run_pattern_select_routine() {
  listen_for_copy_command();

  if (advanced_mode) {
    // Pattern buttons 1/2/3 set slider mode (same as Synthseqr).
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

    // Pattern LEDs: 0 = nav mode; 1/2/3 = active slider mode.
    if (adv_pat_nav_active) pattern_select_leds[0].on();
    else                    pattern_select_leds[0].off();
    pattern_select_leds[1].off();
    pattern_select_leds[2].off();
    pattern_select_leds[3].off();
    if (slider_mode == 1) pattern_select_leds[1].on();
    if (slider_mode == 3) pattern_select_leds[2].on();
    if (slider_mode == 2) pattern_select_leds[3].on();

    // Pattern-nav mode: step buttons select/chain patterns.
    if (adv_pat_nav_active) {
      unsigned long nav_now = millis();
      if (nav_now - adv_blink_last_ms >= 200) {
        adv_blink_last_ms = nav_now;
        adv_blink_state = !adv_blink_state;
      }

      if (adv_chain_hold_step >= 0 &&
          !step_buttons[adv_chain_hold_step].wasPressed()) {
        adv_chain_hold_step = -1;
      }

      for (int i = 0; i < 16; i++) {
        if (step_buttons[i].uniquePress()) {
          if (adv_chain_hold_step < 0) {
            adv_chain_hold_step = i;
            extended_step_length_mode = 0;
            go_to_pattern(i, 0);
          } else if (i != adv_chain_hold_step) {
            chain_start = (uint8_t)adv_chain_hold_step;
            chain_end   = (uint8_t)i;
            extended_step_length_mode = 1;
            go_to_pattern(chain_start, 0);
          }
          break;
        }
      }

      update_pat_nav_leds();
      return;
    }
  }

  // Simple mode: buttons 0–3 select patterns 0–3 directly.
  if (!advanced_mode) {
    for (int pattern = 0; pattern < 4; pattern++) {
      if (pattern_select_button_flags[pattern]) {
        pattern_select_button_flags[pattern] = false;
        go_to_pattern(pattern, 0);
        read_step_memory(current_voice, pattern);
      }
    }
  }

  // Simple mode chain toggle: buttons 0 + 3 simultaneously.
  if (!advanced_mode) {
    static bool chain_toggle_handled = false;
    static int  chain_anim_step    = -1;
    static int  chain_anim_dir     =  1;
    static unsigned long chain_anim_last_ms = 0;

    if (chain_anim_step >= 0) {
      unsigned long now = millis();
      if (now - chain_anim_last_ms >= 60) {
        chain_anim_last_ms = now;
        if (chain_anim_dir == 1) {
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
          lcdflag = 200; next_lcdflag = 200;
          extended_step_length_mode = 0;
          go_to_pattern(0, 1);
          pattern_select_leds[1].on();
          pattern_select_leds[2].on();
          pattern_select_leds[3].on();
          chain_anim_dir     = -1;
          chain_anim_step    =  3;
          chain_anim_last_ms = millis();
        } else {
          lcdflag = 201; next_lcdflag = 201;
          extended_step_length_mode = 1;
          chain_start = 0;
          chain_end   = 3;
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
  if (chain_start <= chain_end) {
    if (current_pattern >= chain_end || current_pattern < chain_start) {
      current_pattern = chain_start;
    } else {
      current_pattern++;
    }
  } else {
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

// go_to_pattern — switch the active pattern.
//
// Restores all 8 voices' pitch/velocity/gate for the new pattern and arms
// pickup guards so sliders can't silently overwrite data mid-switch.
void go_to_pattern(int pattern, int silent) {
  if (!adv_pat_nav_active) pattern_select_leds[0].off();
  pattern_select_leds[1].off();
  pattern_select_leds[2].off();
  pattern_select_leds[3].off();
  if (!advanced_mode) {
    pattern_select_leds[pattern % 4].on();
  }

  if (pattern != pattern_value) {
    // Restore per-voice data for the incoming pattern and lock sliders until
    // physical positions are reached.
    for (int v = 0; v < VOICE_COUNT; v++) {
      slider_needs_pickup[v] = true;
    }
  }

  pattern_value   = pattern;
  current_pattern = pattern;
  update_line1    = true;

  if (!adv_pat_nav_active) {
    read_step_memory(current_voice, pattern);
  }
}

// listen_for_copy_command — simple mode pattern copy (hold 2 s + tap dest).
void listen_for_copy_command() {
  if (!told_which_pattern_to_copy_to) return;

  for (int i = 0; i < 4; i++) {
    if (pattern_select_button_flags[i]) {
      pattern_select_button_flags[i] = false;
      copy_pattern_to = i;

      for (int k = 0; k < 4; k++) pattern_select_leds[k].off();
      pattern_select_leds[i].on();

      // Copy all 8 voices.
      for (int v = 0; v < VOICE_COUNT; v++) {
        for (int s = 0; s < 16; s++) {
          step_data[copy_pattern_to][v][s] = step_data[current_pattern][v][s];
        }
        voice_pitch[copy_pattern_to][v]    = voice_pitch[current_pattern][v];
        voice_velocity[copy_pattern_to][v] = voice_velocity[current_pattern][v];
        voice_gate[copy_pattern_to][v]     = voice_gate[current_pattern][v];
        voice_cc_value[copy_pattern_to][v] = voice_cc_value[current_pattern][v];
      }
      cc_number[copy_pattern_to] = cc_number[current_pattern];

      told_which_pattern_to_copy_to = false;
      lcdflag = 101; next_lcdflag = 101;
    }
  }
}
