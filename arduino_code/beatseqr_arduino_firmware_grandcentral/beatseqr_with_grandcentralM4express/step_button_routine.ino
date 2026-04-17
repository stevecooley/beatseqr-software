void run_step_button_routine() {
  // Advanced mode copy phase 1: tap a step to pick the source pattern.
  if (adv_copy_waiting_source) {
    for (int i = 0; i < 16; i++) {
      if (step_buttons[i].uniquePress()) {
        go_to_pattern(i, 0);
        if (!adv_pat_nav_active) read_step_memory(current_voice, i);
        adv_copy_waiting_source = false;
        adv_copy_armed = true;
        lcdflag = 102; next_lcdflag = 102;
        Serial.print("copy source: pattern "); Serial.println(i + 1);
        break;
      }
    }
    return;
  }

  // Advanced mode copy phase 2: tap a step to pick the destination pattern.
  if (adv_copy_armed) {
    for (int i = 0; i < 16; i++) {
      if (step_buttons[i].uniquePress()) {
        // Copy all 8 voices — step data, pitch, velocity, gate, CC value.
        for (int v = 0; v < VOICE_COUNT; v++) {
          for (int s = 0; s < 16; s++) {
            step_data[i][v][s] = step_data[current_pattern][v][s];
          }
          voice_pitch[i][v]    = voice_pitch[current_pattern][v];
          voice_velocity[i][v] = voice_velocity[current_pattern][v];
          voice_gate[i][v]     = voice_gate[current_pattern][v];
          voice_cc_value[i][v] = voice_cc_value[current_pattern][v];
        }
        cc_number[i] = cc_number[current_pattern];
        copy_pattern_to = i;
        adv_copy_armed = false;
        lcdflag = 101; next_lcdflag = 101;
        Serial.print("copied to pattern "); Serial.println(i + 1);
        break;
      }
    }
    return;
  }

  if (!adv_pat_nav_active) {
    detect_step_button_presses();
  }

  // Clear combos — only active outside pattern-nav mode.
  // Use wasPressed() here: uniquePress() was already called for all step
  // buttons inside detect_step_button_presses() above, so calling isPressed()
  // again would steal the CHANGED flag and miss the press.
  if (!adv_pat_nav_active) {
    if (step_buttons[0].wasPressed() && step_buttons[15].wasPressed()) {
      clear_pattern_memory_for_voice(current_voice);
    }
    if (step_buttons[0].wasPressed() && step_buttons[11].wasPressed()) {
      clear_pattern_memory();
    }
  }
}

void detect_step_button_presses() {
  for (int i = 0; i < 16; i++) {
    if (step_buttons[i].uniquePress()) {
      // In CC mode the step buttons still toggle steps on/off just like other
      // modes. voice_cc_enabled[v] is the per-voice CC gate — it is not per-step
      // and is not toggled here. (Toggle it via voice_mode_select if desired.)
      step_data[pattern_value][current_voice][i] =
          step_data[pattern_value][current_voice][i] ? 0 : 1;

      if (step_data[pattern_value][current_voice][i]) {
        step_leds[i].on();

        // First step pressed for this voice: seed pitch from slider live position
        // so it lands where the slider already sits rather than the stored default.
        bool was_blank = true;
        for (int s = 0; s < 16; s++) {
          if (s != i && step_data[pattern_value][current_voice][s] != 0) {
            was_blank = false;
            break;
          }
        }
        if (was_blank) {
          uint8_t live_pitch = (uint8_t)raw_voice_slider_values[current_voice];
          voice_pitch[pattern_value][current_voice] = live_pitch;
          slider_needs_pickup[current_voice] = false;
        }
      } else {
        step_leds[i].off();
      }
    }
  }
}

void read_step_memory(int voice, int pattern) {
  for (int i = 0; i < 16; i++) {
    if (step_data[pattern][voice][i] == 1) step_leds[i].on();
    else                                   step_leds[i].off();
  }
}

void clear_step_leds() {
  for (int i = 0; i < 16; i++) step_leds[i].off();
}

// clear_pattern_memory_for_voice — clear the active pattern for one voice.
// Bound to step 0 + step 15 held simultaneously.
void clear_pattern_memory_for_voice(int voice) {
  for (int i = 0; i < 16; i++) {
    step_data[pattern_value][voice][i] = 0;
    step_leds[i].off();
  }
  voice_pitch[pattern_value][voice]    = slider_map_low_value;
  voice_velocity[pattern_value][voice] = 100;
  voice_gate[pattern_value][voice]     = 1;
  voice_cc_value[pattern_value][voice] = 0;
}

// clear_pattern_memory — clear all 16 patterns, all 8 voices.
// Bound to step 0 + step 11 held simultaneously.
void clear_pattern_memory() {
  for (int p = 0; p < 16; p++) {
    for (int v = 0; v < VOICE_COUNT; v++) {
      for (int i = 0; i < 16; i++) {
        step_data[p][v][i] = 0;
      }
      voice_pitch[p][v]    = slider_map_low_value;
      voice_velocity[p][v] = 100;
      voice_gate[p][v]     = 1;
      voice_cc_value[p][v] = 0;
    }
    cc_number[p] = 1;
  }
  read_step_memory(current_voice, pattern_value);
}

// init_blank_patterns_to_range — for every pattern where all steps are off
// for every voice, reset voice pitches to slider_map_low_value.
// Called after note range changes and after reset sliders.
void init_blank_patterns_to_range() {
  for (int p = 0; p < 16; p++) {
    bool all_off = true;
    for (int v = 0; v < VOICE_COUNT && all_off; v++) {
      for (int i = 0; i < 16; i++) {
        if (step_data[p][v][i] != 0) { all_off = false; break; }
      }
    }
    if (all_off) {
      for (int v = 0; v < VOICE_COUNT; v++) {
        voice_pitch[p][v] = slider_map_low_value;
      }
    }
  }
}
