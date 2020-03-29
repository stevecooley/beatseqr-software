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
        if (timing_mode < 5) {
          timing_mode++;
        }
        switch_timing_mode_events();
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
            switch_timing_mode_events();
            set_timing_resolution();
            break;
          }
          case 2: {
            switch_timing_mode_events();
            set_timing_resolution();
            break;
          }
          case 3: {
            switch_timing_mode_events();
            set_timing_resolution();
            break;
          }
          case 4: {
            switch_timing_mode_events();
            set_timing_resolution();
            break;
          }
          case 5: {
            swing_events();
            switch_timing_mode_events();
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
    case 1:  //
    {
      timing_resolution = 10.0;
      cursor_x = 11;
      cursor_y = 0;
      break;
    }
    case 2:  //
    {
      timing_resolution = 1.0;
      cursor_x = 12;
      cursor_y = 0;
      break;
    }
    case 3:  //
    {
      timing_resolution = 0.1;
      cursor_x = 14;
      cursor_y = 0;
      break;
    }
    case 4:  //
    {
      timing_resolution = 0.01;
      cursor_x = 15;
      cursor_y = 0;
      break;
    }
    case 5:  // swing?
      cursor_x = 1;
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

    cursor_x = 1;
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

    lcd.print("?x01?y1");
    Serial.println("?x01?y1");

    if (SWING < 6) {
      SWING++;
      seq.setShuffle(SWING);
      update_line2 = true;
      Serial.println(seq.getShuffle());
    }
  }
  // set cursor position
  cursor_flag = true;
}