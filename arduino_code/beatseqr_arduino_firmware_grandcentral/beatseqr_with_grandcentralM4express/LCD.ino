void run_LCD_setup_routine() {
  // On SAMD51, pinMode() disables PMUX on the pin (breaks Serial1 TX).
  // lcd.begin() immediately after re-enables it. Keep both calls together.
  // some LCD serial boards run at 9850, and some run at 9600 baud.
  pinMode(lcdTxPin, OUTPUT);
  lcd.begin(9600);

  lcd.print("?c0");
  lcd.print("?c1");        // cursor blink on
  lcd.print("?B70");       // backlight max brightness
  delay(500);

  lcd.print("?s6");        // tab stops every 6 chars
  delay(1000);

  // Custom character definitions (same as Synthseqr — LCD hardware is shared).
  // ?0 = play ▶   ?1 = voice   ?2 = velocity   ?3 = CC
  // ?4 = NN note  ?5 = slider  ?6 = voice-mode  ?7 = stop ■
  lcd.print("?D010181C1E1E1C1810");  delay(100);  // ?0 play
  lcd.print("?D1110A04000E11110E");  delay(100);  // ?1 voice
  lcd.print("?D21214181004040407");  delay(100);  // ?2 VL
  lcd.print("?D31C10101C07040407");  delay(100);  // ?3 CC
  lcd.print("?D4121A1612090D0B09");  delay(100);  // ?4 NN
  lcd.print("?D504040E0404111B15");  delay(100);  // ?5 slider mode
  lcd.print("?D61111110A04111B15");  delay(100);  // ?6 voice mode
  lcd.print("?D7001F1F1F1F1F1F00");  delay(100);  // ?7 stop block

  lcd.print("?f");                   // clear display
  lcd.print("?x00?y0");
  lcd.print("beatseqr v");
  lcd.print(hardware_version_number);
  lcd.print("?x00?y1");
  lcd.print("firmware v");
  lcd.print(firmware_version_number);
  delay(1500);

  lcd.print("?f");
  delay(200);  // give LCD time to process clear before main loop writes
}

void run_LCD_update() {
  if (diag_mode) return;

  // Rate-limit all LCD writes to ~15 fps (every 66 ms).
  // The LCD serial port is 9850 baud (~985 bytes/sec). Without this gate the
  // main loop floods the receive buffer and produces garbage.
  static unsigned long last_lcd_ms = 0;
  unsigned long now_ms = millis();
  if (now_ms - last_lcd_ms < 66) return;
  last_lcd_ms = now_ms;

  if (clear_the_lcd) {
    clear_the_lcd = false;
    lcd.print("?f");
  }

  if (next_lcdflag != lcdflag) lcdflag = next_lcdflag;

  switch (lcdflag) {

    case 93:  // reset sliders confirmation
    {
      lcd.print("?f");
      lcd.print("?x00?y0");
      lcd.print("sliders reset   ");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }

    case 100:  // simple-mode copy: "Copy N->" waiting for destination
    {
      lcd.print("?f");
      lcd.print("?x00?y0");
      lcd.print("Copy ");
      lcd.print(current_pattern + 1);
      lcd.print("->             ");
      next_lcdflag = 100;
      break;
    }

    case 101:  // copy complete — "Copied X to Y" for 500 ms
    {
      static unsigned long msg_until = 0;
      if (msg_until == 0) {
        msg_until = millis() + 500;
        lcd.print("?f");
        lcd.print("?x00?y0");
        char _buf[17];
        snprintf(_buf, sizeof(_buf), "Copied %d to %d  ",
                 current_pattern + 1, copy_pattern_to + 1);
        lcd.print(_buf);
      }
      if (millis() >= msg_until) {
        msg_until = 0;
        update_line1 = true;
        update_line2 = true;
        next_lcdflag = 255;
      } else {
        next_lcdflag = 101;
      }
      break;
    }

    case 102:  // adv copy phase 2 — "Copy N->where?"
    {
      lcd.print("?f");
      lcd.print("?x00?y0");
      char _buf[17];
      snprintf(_buf, sizeof(_buf), "Copy %d->where? ", current_pattern + 1);
      lcd.print(_buf);
      next_lcdflag = 102;
      break;
    }

    case 103:  // adv copy phase 1 — "copy which pat?"
    {
      lcd.print("?f");
      lcd.print("?x00?y0");
      lcd.print("copy which pat?");
      next_lcdflag = 103;
      break;
    }

    case 202:  // save confirmation — hold 2 s then return to main display
    {
      static unsigned long msg_until = 0;
      if (msg_until == 0) {
        msg_until = millis() + 2000;
        lcd.print("?x00?y0");
        lcd.print("saved!          ");
      }
      if (millis() >= msg_until) {
        msg_until = 0;
        update_line1 = true;
        update_line2 = true;
        next_lcdflag = 255;
      } else {
        next_lcdflag = 202;
      }
      break;
    }

    case 200:  // chain single
    {
      lcd.print("?x00?y0");
      lcd.print("single          ");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }

    case 201:  // chain 4
    {
      lcd.print("?x00?y0");
      lcd.print("chain 4         ");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }

    case 255:
    default: {
      if (config_menu_active) {
        update_line1 = false;
        update_line2 = false;
        break;
      }

      bool did_redraw = false;

      if (update_line1) {
        update_line1 = false;
        did_redraw = true;

        // Line 1: P01 >03 V3 N 120   (16 chars)
        //   cols: 0   4   8  10 12
        // Tempo is integer BPM (3 chars). In external clock mode, show detected BPM.
        float bpm = (external_clock_mode && ext_clk_avg_interval_us > 0)
          ? 60000000.0f / ((float)ext_clk_avg_interval_us * 24.0f)
          : TEMPO;

        char step_str[4];
        if (last_triggered_step >= 0) {
          sprintf(step_str, ">%02d", (int)last_triggered_step + 1);
        } else {
          strcpy(step_str, ">--");
        }

        // Mode char: N=NN, V=VL, G=GT, C=CC
        char mode_char;
        switch (slider_mode) {
          case 1: mode_char = 'N'; break;
          case 2: mode_char = 'V'; break;
          case 3: mode_char = 'G'; break;
          case 4: mode_char = 'C'; break;
          default: mode_char = 'N'; break;
        }

        // "P01 >03 V3 N 120" = 16 chars exactly.
        sprintf(lcd_line1, "P%02u %s V%u %c %3d",
                current_pattern + 1,
                step_str,
                current_voice + 1,
                mode_char,
                (int)bpm);

        lcd.print("?x00?y0");
        lcd.print(lcd_line1);
      }

      if (update_line2) {
        update_line2 = false;
        did_redraw = true;

        // Line 2: always shows the currently selected voice's data.
        // Format: "?4PPP ?2VVV G#?3CCC" — 16 chars.
        //   ?4=NN icon + 3-digit pitch + space (5 chars)
        //   ?2=VL icon + 3-digit velocity + space (5 chars)
        //   G + 1-digit gate (2 chars)
        //   ?3=CC icon + 3-digit CC value or "---" (4 chars)
        lcd.print("?x00?y1");

        uint8_t v = current_voice;
        char _s[8];

        lcd.print("?4");
        sprintf(_s, "%03d ", voice_pitch[pattern_value][v]);
        lcd.print(_s);

        lcd.print("?2");
        sprintf(_s, "%03d ", voice_velocity[pattern_value][v]);
        lcd.print(_s);

        lcd.print("G");
        sprintf(_s, "%d", voice_gate[pattern_value][v]);
        lcd.print(_s);

        lcd.print("?3");
        if (voice_cc_enabled[v]) {
          sprintf(_s, "%03d", voice_cc_value[pattern_value][v]);
        } else {
          strcpy(_s, "---");
        }
        lcd.print(_s);
      }

      // No D-pad cursor in Beatseqr. Move cursor off-screen so blink
      // doesn't obscure the text at position 0,0.
      if (did_redraw) {
        set_lcd_cursor(1, 16);
      }

      next_lcdflag = 255;
      break;
    }
  }
}

void set_lcd_cursor(int line, int position) {
  sprintf(the_string, "?x%02d?y%d", position, line);
  lcd.print(the_string);
}

void clear_lcd() {
  lcd.print("?f");
}
