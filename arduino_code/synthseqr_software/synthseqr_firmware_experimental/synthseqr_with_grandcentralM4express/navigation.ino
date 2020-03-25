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
      }
      // mode switching
      if (dpad_right_flag == true) {
        dpad_right_flag = false;
        if (timing_mode < 5) {
          timing_mode++;
        }
      }

      if (enterbutton_flag == true) {
        enterbutton_flag = false;
        clear_the_lcd = true;
        enterbutton_LED.toggle();
        update_line1 = true;
        lcdflag = 255;
        next_lcdflag = 255;
      }

      if ((dpad_up_flag == true) || (dpad_down_flag == true)) {
        switch (timing_mode) {
          case 1: {
            set_timing_resolution();
            break;
          }
          case 2: {
            set_timing_resolution();
            break;
          }
          case 3: {
            set_timing_resolution();
            break;
          }
          case 4: {
            set_timing_resolution();
            break;
          }
          case 5: {
            swing_events();
            break;
          }
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

void set_timing_resolution() {
  switch (timing_mode) {
    case 1:  //
    {
      timing_resolution = 10.0;
      lcd.println("?x11?y0");
      break;
    }
    case 2:  //
    {
      timing_resolution = 1.0;
      lcd.println("?x12?y0");

      break;
    }
    case 3:  //
    {
      timing_resolution = 0.1;
      lcd.println("?x14?y0");

      break;
    }
    case 4:  //
    {
      timing_resolution = 0.01;
      lcd.println("?x15?y0");

      break;
    }
  }

  if (dpad_up_flag == true) {
    dpad_up_flag = false;

    TEMPO = TEMPO + timing_resolution;
    seq.setTempo(TEMPO);
    update_line1 = true;
    Serial.println(TEMPO);
  }

  if (dpad_down_flag == true) {
    dpad_down_flag = false;

    TEMPO = TEMPO - timing_resolution;
    seq.setTempo(TEMPO);
    update_line1 = true;
    Serial.println(TEMPO);
  }
}

void swing_events() {
  if (dpad_down_flag == true) {
    dpad_down_flag = false;

    lcd.print("?x01?y1");

    if (SWING > 0) {
      SWING--;
      seq.setShuffle(SWING);
      update_line2 = true;
      Serial.println(seq.getShuffle());
    }
  }
  if (dpad_up_flag == true) {
    dpad_up_flag = false;

    lcd.print("?x01?y1");

    if (SWING < 6) {
      SWING++;
      seq.setShuffle(SWING);
      update_line2 = true;
      Serial.println(seq.getShuffle());
    }
  }
}