// next_slider_mode()
//
// Cycles slider mode forward from current: NN → VL → GT → CC → PR → LV → D → NN,
// skipping any modes whose feature flag is off. Tries up to slider_mode_total
// times to find an enabled mode; falls back to NN if everything else is off.
//
static uint8_t next_slider_mode(uint8_t current) {
  uint8_t next = current;
  for (uint8_t i = 0; i < slider_mode_total; i++) {
    next = (next % slider_mode_total) + 1;
    bool enabled = true;
    switch (next) {
      case 2: if (!ft_velocity_mode) enabled = false; break;
      case 3: if (!ft_gate_mode)     enabled = false; break;
      case 4: if (!ft_cc_mode)       enabled = false; break;
      case 5: if (!ft_probability)   enabled = false; break;
      case 6: if (!ft_live_cc_mode)  enabled = false; break;
      case 7: if (!ft_drift_mode)    enabled = false; break;
      default: break;
    }
    if (enabled) return next;
  }
  return 1;  // NN is always available
}

void listen_for_navigation_events() {
  // D-pad left exits advanced pattern-nav mode and clears chain mode.
  if (adv_pat_nav_active && dpad_left_flag) {
    dpad_left_flag = false;
    adv_pat_nav_active = false;
    adv_chain_hold_step = -1;
    extended_step_length_mode = 0;
    read_step_memory(0, pattern_value);
    update_line1 = true;
    update_line2 = true;
    next_lcdflag = 255;
    Serial.println("nav mode: exited via dpad left");
    return;
  }

  // D-pad left also clears chain mode in simple mode.
  if (!advanced_mode && extended_step_length_mode && dpad_left_flag) {
    extended_step_length_mode = 0;
    // don't consume flag — let timing mode cycle happen too
  }

  // Cancel advanced copy mode (either phase) with d-pad left.
  if ((adv_copy_waiting_source || adv_copy_armed) && dpad_left_flag) {
    dpad_left_flag = false;
    adv_copy_waiting_source = false;
    adv_copy_armed = false;
    adv_pat_nav_active = false;
    read_step_memory(0, pattern_value);
    update_line1 = true;
    update_line2 = true;
    next_lcdflag = 255;
    Serial.println("copy cancelled");
    return;
  }

  // Serial.println("listening for navigation events");
  switch (navmode) {
    case 100:  // main screen navigation
    {
      // LV mode + editing a lane: d-pad up/down adjusts that lane's CC#,
      // d-pad left exits editing. Intercepts before normal pattern-nav handling.
      if (slider_mode == 6 && live_cc_editing_lane >= 0) {
        if (dpad_left_flag) {
          dpad_left_flag = false;
          step_leds[live_cc_editing_lane].off();
          live_cc_editing_lane = -1;
          update_line2 = true;
          break;
        }
        if (dpad_up_flag) {
          dpad_up_flag = false;
          live_cc_number[live_cc_editing_lane] =
            next_valid_cc(live_cc_number[live_cc_editing_lane], +1);
          // Force next slider read to retransmit at the new CC# even if value unchanged.
          live_cc_last_sent[live_cc_editing_lane] = 255;
          update_line2 = true;
        }
        if (dpad_down_flag) {
          dpad_down_flag = false;
          live_cc_number[live_cc_editing_lane] =
            next_valid_cc(live_cc_number[live_cc_editing_lane], -1);
          live_cc_last_sent[live_cc_editing_lane] = 255;
          update_line2 = true;
        }
        if (enterbutton_flag) {
          enterbutton_flag = false;
          step_leds[live_cc_editing_lane].off();
          live_cc_editing_lane = -1;
          update_line2 = true;
        }
        break;
      }

      // Consume d-pad left here — no timing modes to cycle on main screen.
      if (dpad_left_flag) dpad_left_flag = false;

      if (enterbutton_flag == true) {
        enterbutton_flag = false;
        // Cycle slider mode in both simple and advanced mode (advanced no
        // longer leaves Enter unused — it shares the same NN/VL/GT/CC/PR/LV
        // cycle, with disabled modes skipped). Pattern buttons 1/2/3 still
        // remain as quick-jump shortcuts in advanced mode.
        set_slider_mode(next_slider_mode(slider_mode));
      }

      if ((dpad_up_flag == true) || (dpad_down_flag == true)) {
        if (!adv_copy_waiting_source && !adv_copy_armed) {
          pattern_select_events();
        } else {
          dpad_up_flag = false;
          dpad_down_flag = false;
        }
      }

      break;
    }
    case 110:  // adjust the slider values low and high
    {
      if (dpad_left.uniquePress()) {
        slider_map_low_value =
            map(voice_sliders[0].getSector(), 0, 255, 0, 128);

        // begin display the high and low values for the sliders
        // Serial.println(slider_map_low_value);

        // parking LCD updates in lcdflag 91!

        // end display the high and low values for the sliders

        midinn_sliderrangelow = slider_map_low_value;

        for (uint8_t i = 0; i < 15; i++) {
          if (voice_slider_midinotenum[i] <= midinn_sliderrangelow) {
            voice_slider_midinotenum[i] = midinn_sliderrangelow;
          }
        }
      }
      break;
    }
  }
}

// swing_events() — no longer called from D-pad path (swing moved to config menu).
// Kept as a no-op stub to avoid breaking any lingering call sites.
void swing_events() {}

// setExternalClockMode(bool)
//
// Switch between internal TC4 clock and external USB-MIDI clock.
// - false (internal): restarts TC4, sequencer is self-clocked.
// - true  (external): stops TC4, incoming 0xF8 drives hardwareClockPulse().
//
void setExternalClockMode(bool enable) {
  if (enable == external_clock_mode) return;  // no change
  external_clock_mode = enable;
  if (external_clock_mode) {
    stopSequencerTimer();
  } else {
    // Resync timer period to current TEMPO before re-enabling.
    setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
    startSequencerTimer();
  }
  Serial.print("clock mode: ");
  Serial.println(external_clock_mode ? "EXT" : "INT");
}

// pattern_select_events()
//
// Called from listen_for_navigation_events() when timing_mode == 8.
// Up advances to the next pattern (wraps 4→1), down goes to the previous
// (wraps 1→4). Delegates to go_to_pattern() so LEDs, step display, and
// slider pickup arming all behave the same as pressing a pattern button.
//
void pattern_select_events() {
  uint8_t max_patterns = advanced_mode ? 16 : 4;
  // In LV mode, step LEDs are owned by the lane editor — don't repaint from
  // step_data when switching patterns (pattern affects the note sequence
  // running in the background, but LV lane mapping is global).
  // In CC and D mode, go_to_pattern() already paints the right per-pattern
  // overlay (cc_step_enabled / step_drift_enabled), so skip the redundant
  // step_data repaint here too.
  bool repaint_step_leds = (slider_mode != 6 && slider_mode != 4 && slider_mode != 7);
  if (dpad_up_flag == true) {
    dpad_up_flag = false;
    uint8_t next = (current_pattern + 1) % max_patterns;
    go_to_pattern(next, 0);
    if (repaint_step_leds) read_step_memory(0, next);
    cursor_x = LCD_L1_X_PATTERN;
    cursor_y = 0;
    cursor_flag = true;
  }
  if (dpad_down_flag == true) {
    dpad_down_flag = false;
    uint8_t next = (current_pattern + max_patterns - 1) % max_patterns;  // -1 with wrap
    go_to_pattern(next, 0);
    if (repaint_step_leds) read_step_memory(0, next);
    cursor_x = LCD_L1_X_PATTERN;
    cursor_y = 0;
    cursor_flag = true;
  }
}

// midi_channel_events()
//
// Called from listen_for_navigation_events() when timing_mode == 7.
// Up increments MIDI channel (max 16), down decrements (min 1).
// Line 1 redraws to show the updated "C%02d" value.
//
void midi_channel_events() {
  if (dpad_up_flag == true) {
    dpad_up_flag = false;
    if (MIDICHANNEL < 16) {
      MIDICHANNEL++;
    }
    update_line2 = true;
    cursor_flag = true;
    Serial.print("MIDI channel: ");
    Serial.println(MIDICHANNEL);
  }
  if (dpad_down_flag == true) {
    dpad_down_flag = false;
    if (MIDICHANNEL > 1) {
      MIDICHANNEL--;
    }
    update_line2 = true;
    cursor_flag = true;
    Serial.print("MIDI channel: ");
    Serial.println(MIDICHANNEL);
  }
}

// clock_source_events()
//
// Called from listen_for_navigation_events() when timing_mode == 6.
// Up = external clock, down = internal clock.
// Shown inline on line 2 as "clk:int" or "clk:ext" after the swing value.
//
void clock_source_events() {
  if (dpad_up_flag == true) {
    dpad_up_flag = false;
    setExternalClockMode(true);
    update_line2 = true;
    cursor_flag = true;
  }
  if (dpad_down_flag == true) {
    dpad_down_flag = false;
    setExternalClockMode(false);
    update_line2 = true;
    cursor_flag = true;
  }
}