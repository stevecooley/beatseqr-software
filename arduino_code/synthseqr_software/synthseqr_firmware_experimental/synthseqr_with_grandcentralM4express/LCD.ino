void run_LCD_setup_routine() {
  // On SAMD51, pinMode() disables PMUX on the pin, which would break Serial1
  // TX. The lcd.begin() call immediately after re-enables PMUX, restoring
  // UART function. Both calls must stay together in this order.
  //
  // Baud rate quirk: some LCD backpack batches run at 9850 baud, not the
  // standard 9600. Change the value in both lcd.begin() calls (here and in
  // setup()) if you swap in a different module.
  // lcd.begin(9600);  // ← use this line instead for standard-baud modules
  pinMode(lcdTxPin, OUTPUT);
  lcd.begin(9600);

  lcd.print("?c0");
  Serial.println("?c0");
  lcd.print("?c1");       // blinky
  Serial.println("?c1");  // blinky

  lcd.print("?B70");  // set backlight to (70, but) ff hex, maximum brightness
  Serial.println(
      "?B70");  // set backlight to (70, but) ff hex, maximum brightness
  delay(500);   // pause to allow lcd EEPROM to program

  lcd.print("?s6");       // set tabs to six spaces
  Serial.println("?s6");  // set tabs to six spaces
  delay(1000);            // pause to allow lcd EEPROM to program

  lcd.print("?D010181C1E1E1C1810");       // play
  Serial.println("?D010181C1E1E1C1810");  // play
  delay(100);

  lcd.print("?D1110A04000E11110E");       // voice
  Serial.println("?D1110A04000E11110E");  // voice
  delay(100);

  lcd.print("?D21214181004040407");       // VL
  Serial.println("?D21214181004040407");  // VL
  delay(100);

  lcd.print("?D31C10101C07040407");       // CC
  Serial.println("?D31C10101C07040407");  // CC
  delay(100);

  lcd.print("?D4121A1612090D0B09");       // NN
  Serial.println("?D4121A1612090D0B09");  // NN
  delay(100);

  lcd.print("?D504040E0404111B15");       // slider mode
  Serial.println("?D504040E0404111B15");  // slider mode
  delay(100);

  lcd.print("?D61111110A04111B15");  // voice select button aka "VM", voice mode
  Serial.println(
      "?D61111110A04111B15");  // voice select button aka "VM", voice mode
  delay(100);

  lcd.print("?D7001F1F1F1F1F1F00");       // STOP ...
  Serial.println("?D7001F1F1F1F1F1F00");  // STOP ...
  //  this used to be "solo", but instead of explicitly calling it "solo", I'm
  //  going to go with "toggle" instead, and just use a "T" to  represent that
  //  Doing that will let me change the "O" I was using to represent "stop" and
  //  replace it with a solid square instead.

  delay(100);

  // see moderndevice.com for a handy custom char generator (software app)

  lcd.print("?f");            // clear the LCD
  Serial.println("?f");       // clear the LCD
  lcd.print("?x00?y0");       // move cursor to beginning of line 1
  Serial.println("?x00?y0");  // move cursor to beginning of line 1
  lcd.print("synthseqr v");
  Serial.println("synthseqr v");
  lcd.print(hardware_version_number);
  Serial.println(hardware_version_number);

  lcd.print("?x00?y1");       // move cursor to beginning of line 0
  Serial.println("?x00?y1");  // move cursor to beginning of line 0
  // "fw:" prefix (not "firmware v") so the line stays inside 16 chars even
  // with the auto-generated FIRMWARE_VERSION ("3.147-dev" worst case = 13
  // chars after the prefix = 16 total).
  lcd.print("fw:");
  Serial.println("fw:");
  lcd.print(firmware_version_number);
  Serial.println(firmware_version_number);
  delay(1500);
  // pinMode(66, INPUT); // ???
  // pinMode(67, INPUT); // ???

  lcd.print("?f");       // clear the LCD
  Serial.println("?f");  // clear the LCD
}

void run_LCD_update() {
  // Diagnostics mode writes directly to the LCD — suppress normal updates.
#if FEATURE_DIAGNOSTICS
  if (diag_mode) return;
#endif

  // Rate-limit all LCD writes to ~15 fps (every 66 ms).
  // The LCD serial port is only 9850 baud (~985 bytes/sec). Without this gate
  // the main loop floods the LCD's receive buffer and produces garbage output.
  // Flag-driven updates (update_line1, update_line2) are still processed every
  // frame — they just can't arrive faster than the LCD can handle.
  static unsigned long last_lcd_ms = 0;
  unsigned long now_ms = millis();
  if (now_ms - last_lcd_ms < 66) return;
  last_lcd_ms = now_ms;

  if (clear_the_lcd == true)
  {
    clear_the_lcd = false;
    lcd.print("?f");
    Serial.println("?f");
  }

  if (next_lcdflag != lcdflag) {
    lcdflag = next_lcdflag;
  }

  switch (lcdflag) {
    case 90:  // parked from run_voice_slider_routine for NN
    {
      // lcd.print("?f");      // clear the LCD
      Serial.println("?f");       // clear the LCD
      lcd.print("?x02?y0");       // move cursor to beginning of line 0
      Serial.println("?x02?y0");  // move cursor to beginning of line 0
      lcd.print("v");
      Serial.println("v");
      lcd.print(1);
      Serial.println(1);

      if (voice_slider_values[0] < 100) {
        lcd.print(" ");
        Serial.println(" ");
      }

      lcd.print(" ?4");
      Serial.println(" ?4");
      lcd.print(":");
      Serial.println(":");
      lcd.print(voice_slider_values[0]);
      Serial.println(voice_slider_values[0]);
      if (voice_slider_values[0] < 100) {
        lcd.print(" ");
        Serial.println(" ");
      }
      lcd.print("?x00?y1");       // move cursor to beginning of line 1
      Serial.println("?x00?y1");  // move cursor to beginning of line 1
      next_lcdflag = 90;
      break;
    }
    case 91:  // parked from run_voice_slider_routine for changing high range
              // values
    {
      if (slider_map_low_value < 100) {
        lcd.print(" ");
        Serial.println(" ");
      }
      lcd.print("?x09?y1");       // move cursor to line 2, char 10
      Serial.println("?x09?y1");  // move cursor to line 2, char 10
      lcd.print("hi:");
      Serial.println("hi:");
      lcd.print("?x12?y1");       // move cursor to line 2, char 13
      Serial.println("?x12?y1");  // move cursor to line 2, char 13
      lcd.print(slider_map_high_value);
      Serial.println(slider_map_high_value);
      if (slider_map_high_value < 100) {
        //            lcd.print(" ");
        Serial.println(" ");
      }
      next_lcdflag = 91;
      break;
    }
    case 92:  // changing the voice slider high range
    {
      // lcd.print("?x0?y1");   // move cursor to line 2
      Serial.println("?x0?y1");   // move cursor to line 2
      lcd.print("?x00?y1");       // move cursor to line 2, char 1
      Serial.println("?x00?y1");  // move cursor to line 2, char 1
      lcd.print("?5");
      Serial.println("?5");
      lcd.print(" lo:");
      Serial.println(" lo:");
      lcd.print("?x05?y1");       // move cursor to line 2, char 6
      Serial.println("?x05?y1");  // move cursor to line 2, char 6
      lcd.print(slider_map_low_value);
      Serial.println(slider_map_low_value);
      if (slider_map_low_value < 100) {
        lcd.print(" ");
        Serial.println(" ");
      }
      lcd.print("?x09?y1");       // move cursor to line 2, char 10
      Serial.println("?x09?y1");  // move cursor to line 2, char 10
      lcd.print("hi:");
      Serial.println("hi:");
      lcd.print("?x12?y1");       // move cursor to line 2, char 13
      Serial.println("?x12?y1");  // move cursor to line 2, char 13
      lcd.print(slider_map_high_value);
      Serial.println(slider_map_high_value);
      if (slider_map_high_value < 100) {
        lcd.print(" ");
        Serial.println(" ");
      }
      break;
      next_lcdflag = 92;
    }
    case 93:  // reset voice_slider_values
    {
      lcd.print("?f");
      Serial.println("?f");
      lcd.print("?x00?y0");
      Serial.println("?x00?y0");
      lcd.print("notenum reset");
      Serial.println("notenum reset");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }
    case 100:  // pattern copy — simple mode: "Copy N->" waiting for destination
    {
      lcd.print("?f");            // clear the lcd
      Serial.println("?f");       // clear the lcd
      lcd.print("?x00?y0");       // move cursor to beginning of line 1
      Serial.println("?x00?y0");  // move cursor to beginning of line 1
      lcd.print("Copy ");
      Serial.println("Copy ");
      lcd.print(current_pattern + 1);
      Serial.println(current_pattern + 1);
      lcd.print("->");
      Serial.println("->");
      next_lcdflag = 100;
      break;
    }
    case 101:  // copy complete — "Copied X to Y" for 500 ms
    {
      static unsigned long msg_until = 0;
      if (msg_until == 0) {
        msg_until = millis() + 500;
        lcd.print("?f");
        Serial.println("?f");
        lcd.print("?x00?y0");
        Serial.println("?x00?y0");
        char _buf[17];
        snprintf(_buf, sizeof(_buf), "Copied %d to %d", copy_pattern_from + 1, copy_pattern_to + 1);
        lcd.print(_buf);
        Serial.println(_buf);
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
    case 102:  // advanced copy phase 2 — source selected, waiting for destination
    {
      // Caller sets update_line1 = true to request a redraw. This avoids the
      // sticky-static idiom that left stale state across repeated copy attempts.
      if (update_line1) {
        update_line1 = false;
        lcd.print("?f");
        Serial.println("?f");
        lcd.print("?x00?y0");
        Serial.println("?x00?y0");
        char _buf[17];
        snprintf(_buf, sizeof(_buf), "Copy %d->where  ", copy_pattern_from + 1);
        lcd.print(_buf);
        Serial.println(_buf);
      }
      next_lcdflag = 102;
      break;
    }
    case 103:  // advanced copy phase 1 — waiting for user to tap source pattern
    {
      if (update_line1) {
        update_line1 = false;
        lcd.print("?f");
        Serial.println("?f");
        lcd.print("?x00?y0");
        Serial.println("?x00?y0");
        lcd.print("copy which pat  ");
        Serial.println("copy which pat  ");
      }
      next_lcdflag = 103;
      break;
    }
    case 202:  // EEPROM save confirmation — hold for 2 s then return to main display
    {
      static unsigned long msg_until = 0;
      if (msg_until == 0) {
        msg_until = millis() + 2000;
        lcd.print("?x00?y0");
        Serial.println("?x00?y0");
        lcd.print("saved!          ");  // 16 chars — overwrites full line 1
        Serial.println("saved!");
      }
      if (millis() >= msg_until) {
        msg_until = 0;
        update_line1 = true;
        update_line2 = true;
        next_lcdflag = 255;
      } else {
        next_lcdflag = 202;  // stay on this message until timer expires
      }
      break;
    }
    case 203:  // EEPROM saved but SD failed
    {
      static unsigned long msg_until = 0;
      if (msg_until == 0) {
        msg_until = millis() + 2000;
        lcd.print("?x00?y0");
        lcd.print("EE ok, no SD!   ");
        Serial.println("EE ok, SD failed");
      }
      if (millis() >= msg_until) {
        msg_until = 0;
        update_line1 = true;
        update_line2 = true;
        next_lcdflag = 255;
      } else {
        next_lcdflag = 203;
      }
      break;
    }
    case 200:  // pattern chain single — show once then return to main display
    {
      lcd.print("?x00?y0");
      Serial.println("?x00?y0");
      lcd.print("single  ");
      Serial.println("single");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }
    case 201:  // pattern chain 4 — show once then return to main display
    {
      lcd.print("?x00?y0");
      Serial.println("?x00?y0");
      lcd.print("chain 4 ");
      Serial.println("chain 4");
      update_line1 = true;
      update_line2 = true;
      next_lcdflag = 255;
      break;
    }
    case 255:
    default: {
      bool did_redraw = false;

      // Config menu draws directly via draw_config_menu() — suppress live
      // line updates so step triggers and transport events don't overwrite it.
      if (config_menu_active) {
        update_line1 = false;
        update_line2 = false;
        break;
      }

      if (update_line1 == true) {
        // stand down the update line 1 flag
        update_line1 = false;
        did_redraw = true;

        // Line 1: "P01 >03 120.0 ♪N" — 16 chars total.
        // Format: P{pat:02u} >{step:02d_or_--} {tempo:05.1f} [?5][mode]
        // In external clock mode, show detected BPM from avg 0xF8 interval.
        float bpm = (external_clock_mode && ext_clk_avg_interval_us > 0)
          ? 60000000.0f / ((float)ext_clk_avg_interval_us * 24.0f)
          : seq.getTempo();
        char step_str[4];
        if (last_triggered_step >= 0) {
          sprintf(step_str, ">%02d", (int)last_triggered_step + 1);
        } else {
          strcpy(step_str, ">--");
        }
        // "P01 >03 120.0 " = 14 chars + 2-char mode indicator = 16
        sprintf(lcd_line1, "P%02u %s %05.1f ", current_pattern + 1, step_str, bpm);
        Serial.println(lcd_line1);
        lcd.print("?x00?y0");       // move cursor to beginning of line 0
        Serial.println("?x00?y0");
        lcd.print(lcd_line1);
        Serial.println(lcd_line1);

        // Slider mode indicator at cols 14-15: ?5 (slider-mode icon) + mode char.
        // ?4=NN  ?2=VL  G=gate  ?3=CC  P=PR  L=LV
        lcd.print("?5");
        Serial.print("?5");
        switch (slider_mode) {
          case 1: lcd.print("?4"); Serial.println("?4"); break;
          case 2: lcd.print("?2"); Serial.println("?2"); break;
          case 3: lcd.print("G");  Serial.println("G");  break;
          case 4: lcd.print("?3"); Serial.println("?3"); break;
          case 5: lcd.print("P");  Serial.println("P");  break;
          case 6: lcd.print("L");  Serial.println("L");  break;
          default: lcd.print("?4"); Serial.println("?4"); break;
        }
      }

      if (update_line2 == true) {
        update_line2 = false;
        did_redraw = true;
        // LV mode owns line 2: editing shows focused lane + CC# + name,
        // idle shows last-touched lane + CC# + last-sent value, or a
        // placeholder until any slider has moved.
        if (slider_mode == 6) {
          char line2[17];
          int len;
          if (live_cc_editing_lane >= 0) {
            uint8_t cc = live_cc_number[live_cc_editing_lane];
            len = snprintf(line2, sizeof(line2), "L%02d:%03d %-7s",
                           (int)live_cc_editing_lane + 1, cc, cc_name(cc));
          } else if (live_cc_last_lane >= 0) {
            uint8_t cc  = live_cc_number[live_cc_last_lane];
            uint8_t val = live_cc_last_sent[live_cc_last_lane];
            if (val > 127) val = 0;
            len = snprintf(line2, sizeof(line2), "L%02d CC%03d V%03d",
                           (int)live_cc_last_lane + 1, cc, val);
          } else {
            len = snprintf(line2, sizeof(line2), "Live CC ch%02d", live_cc_channel);
          }
          while (len < 16) line2[len++] = ' ';
          line2[16] = '\0';
          lcd.print("?x00?y1");
          Serial.print("?x00?y1");
          lcd.print(line2);
          Serial.println(line2);
          if (cursor_flag || did_redraw) {
            cursor_flag = false;
            set_lcd_cursor(cursor_y, cursor_x);
          }
          next_lcdflag = 255;
          break;
        }
        // Line 2: "♪PPP ♩VVV G#♫CCC" — 16 chars.
        // ?4=NN icon, ?2=VL icon, ?3=CC icon; plain chars via sprintf.
        // Fields: note(5) + vel(5) + gate(2) + cc(4) = 16 chars exactly.
        lcd.print("?x00?y1");
        Serial.print("?x00?y1");
        if (last_triggered_step >= 0) {
          char _s[8];
          // NN icon + note value + space (1+3+1 = 5 chars)
          lcd.print("?4");  Serial.print("?4");
          sprintf(_s, "%03d ", last_triggered_pitch);
          lcd.print(_s);  Serial.print(_s);
          // VL icon + velocity + space (1+3+1 = 5 chars)
          lcd.print("?2");  Serial.print("?2");
          sprintf(_s, "%03d ", voice_slider_midivelocity[last_triggered_step]);
          lcd.print(_s);  Serial.print(_s);
          // Gate label + value (G + 1 digit = 2 chars)
          lcd.print("G");  Serial.print("G");
          sprintf(_s, "%d", step_gate[pattern_value][last_triggered_step]);
          lcd.print(_s);  Serial.print(_s);
          // CC icon + value or "---" (1+3 = 4 chars)
          lcd.print("?3");  Serial.print("?3");
#if FEATURE_CC_MODE
          if (cc_step_enabled[pattern_value][last_triggered_step]) {
            sprintf(_s, "%03d", cc_step_values[pattern_value][last_triggered_step]);
          } else {
            strcpy(_s, "---");
          }
#else
          strcpy(_s, "---");
#endif
          lcd.print(_s);  Serial.println(_s);
        } else {
          lcd.print("                ");   // 16 spaces until first step fires
          Serial.println("                ");
        }
      }

      // Reposition cursor whenever navigation moves it (cursor_flag) OR after
      // any line redraw — redraws leave the cursor at an unpredictable position.
      if (cursor_flag || did_redraw) {
        cursor_flag = false;
        set_lcd_cursor(cursor_y, cursor_x);
      }

      next_lcdflag = 255;
      break;
    }
  }
}

void set_lcd_cursor(int line, int position) {
  sprintf(the_string, "?x%02d?y%d", position, line);
  lcd.print(the_string);  // move cursor
  Serial.println(the_string);
}

void clear_lcd() {
  lcd.print("?f");       // clear the LCD
  Serial.println("?f");  // clear the LCD
}
