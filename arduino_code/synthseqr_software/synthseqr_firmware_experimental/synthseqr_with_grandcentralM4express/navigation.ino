void listen_for_navigation_events() {
  // Serial.println("listening for navigation events");
  switch (navmode) {
    case 100:  // default to tempo and swing adjustments
    {
      // mode switching
      if (dpad_left_flag == true) {
        dpad_left_flag = false;
        if (timing_mode > 1) {
          timing_mode--;
        }
        switch_timing_mode_events();
      }
      // mode switching
      if (dpad_right_flag == true) {
        dpad_right_flag = false;
        if (timing_mode < 8) {
          timing_mode++;
        }
        switch_timing_mode_events();
      }

      if (enterbutton_flag == true) {
        enterbutton_flag = false;
        clear_the_lcd = true;
        enterbutton_LED.toggle();
        update_line1 = true;
        update_line2 = true;
        lcdflag = 255;
        next_lcdflag = 255;
      }

      // Hold Enter for 2 s (without Play) → save all patterns to EEPROM.
      // Play+Enter held is the diagnostics combo, so we guard against that.
      {
        static bool save_handled = false;
        if (enterbutton.heldFor(2000) && !playbutton.wasPressed()) {
          if (!save_handled) {
            save_handled = true;
            save_to_eeprom();
            lcdflag = 202;
            next_lcdflag = 202;
          }
        } else if (!enterbutton.wasPressed()) {
          save_handled = false;
        }
      }

      if ((dpad_up_flag == true) || (dpad_down_flag == true)) {
        switch (timing_mode) {
          case 1: {  // pattern select
            pattern_select_events();
            break;
          }
          case 2:  // ±10 BPM
          case 3:  // ±1 BPM
          case 4:  // ±0.1 BPM
          case 5: {  // ±0.01 BPM
            switch_timing_mode_events();
            set_timing_resolution();
            break;
          }
          case 6: {  // swing
            swing_events();
            switch_timing_mode_events();
            break;
          }
          case 7: {  // clock source
            clock_source_events();
            break;
          }
          case 8: {  // MIDI channel
            midi_channel_events();
            break;
          }
        }
        Serial.print(" getTempo : ");
        Serial.print(seq.getTempo());
        Serial.print(" getShuffle : ");
        Serial.print(seq.getShuffle());
        Serial.print(" getbeatlength : ");
        Serial.print(seq.getbeatlength());
        Serial.print(" getPosition : ");
        Serial.print(seq.getPosition());
        Serial.println();
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
      // high range is set with right d-pad and enter
      if (dpad_right.uniquePress()) {
        slider_map_high_value =
            map(voice_sliders[0].getValue(), 0, 255, 0, 128);

        // begin display the high and low values for the sliders

        // parked in lcdflag = 92!

        // end display the high and low values for the sliders

        midinn_sliderrangehigh = slider_map_high_value;

        for (int i = 0; i < 15; i++) {
          if (voice_slider_midinotenum[i] != midinn_sliderrangehigh) {
            voice_slider_midinotenum[i] = midinn_sliderrangehigh;
          }
        }
      }
      break;
    }
  }
}

void switch_timing_mode_events() {
  switch (timing_mode) {
    case 1:  // pattern select — cursor on pattern digit
      cursor_x = LCD_L1_X_PATTERN;
      cursor_y = 0;
      break;
    case 2:  // ±10 BPM
      timing_resolution = 10.0;
      cursor_x = LCD_L1_X_TEMPO_10;
      cursor_y = 0;
      break;
    case 3:  // ±1 BPM
      timing_resolution = 1.0;
      cursor_x = LCD_L1_X_TEMPO_1;
      cursor_y = 0;
      break;
    case 4:  // ±0.1 BPM
      timing_resolution = 0.1;
      cursor_x = LCD_L1_X_TEMPO_01;
      cursor_y = 0;
      break;
    case 5:  // ±0.01 BPM
      timing_resolution = 0.01;
      cursor_x = LCD_L1_X_TEMPO_001;
      cursor_y = 0;
      break;
    case 6:  // swing — cursor on swing digit
      cursor_x = LCD_L2_X_SWING;
      cursor_y = 1;
      break;
    case 7:  // clock source — cursor on int/ext value; redraw line 2
      cursor_x = LCD_L2_X_CLOCK;
      cursor_y = 1;
      update_line2 = true;
      break;
    case 8:  // MIDI channel — cursor on channel digits
      cursor_x = LCD_L2_X_MIDICHAN;
      cursor_y = 1;
      break;
  }
  // update the cursor position
  cursor_flag = true;
}

void set_timing_resolution() {
  if (dpad_up_flag == true) {
    dpad_up_flag = false;

    TEMPO = TEMPO + timing_resolution;
    seq.setTempo(TEMPO);
    // Keep the hardware timer aligned with the new tempo.
    setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
    update_line1 = true;
    Serial.println(TEMPO);
  }

  if (dpad_down_flag == true) {
    dpad_down_flag = false;

    TEMPO = TEMPO - timing_resolution;
    seq.setTempo(TEMPO);
    // Keep the hardware timer aligned with the new tempo.
    setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
    update_line1 = true;
    Serial.println(TEMPO);
  }
}

void swing_events() {
  if (dpad_down_flag == true) {
    dpad_down_flag = false;

    cursor_x = LCD_L2_X_SWING;
    cursor_y = 1;

    if (SWING > 0) {
      SWING--;
      seq.setShuffle(SWING);
      update_line2 = true;
      Serial.println(seq.getShuffle());
    }
  }
  if (dpad_up_flag == true) {
    dpad_up_flag = false;

    if (SWING < 5) {
      SWING++;
      seq.setShuffle(SWING);
      update_line2 = true;
      Serial.println(seq.getShuffle());
    }
  }
  // set cursor position
  cursor_flag = true;
}

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
  if (dpad_up_flag == true) {
    dpad_up_flag = false;
    uint8_t next = (current_pattern + 1) % 4;
    go_to_pattern(next, 0);
    read_step_memory(0, next);
    cursor_x = LCD_L1_X_PATTERN;
    cursor_y = 0;
    cursor_flag = true;
  }
  if (dpad_down_flag == true) {
    dpad_down_flag = false;
    uint8_t next = (current_pattern + 3) % 4;  // -1 with wrap
    go_to_pattern(next, 0);
    read_step_memory(0, next);
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