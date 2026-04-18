// cc_name()
//
// Returns a 7-char (max) display name for a MIDI CC number.
// Used in config menu label (14 chars) and editing line 2 (16 chars).
//
static const char* cc_name(uint8_t n) {
  switch (n) {
    case 1:  return "Mod Whl";
    case 2:  return "Breath ";
    case 4:  return "Foot Ct";
    case 5:  return "Port Tm";
    case 6:  return "DataMSB";
    case 7:  return "Volume ";
    case 8:  return "Balance";
    case 10: return "Pan    ";
    case 11: return "Express";
    case 12: return "FX Ctl1";
    case 13: return "FX Ctl2";
    case 16: return "GenPrp1";
    case 17: return "GenPrp2";
    case 18: return "GenPrp3";
    case 19: return "GenPrp4";
    case 64: return "Sustain";
    case 65: return "Port On";
    case 66: return "Sosten ";
    case 67: return "Soft Pd";
    case 68: return "Legato ";
    case 69: return "Hold 2 ";
    case 70: return "Snd Var";
    case 71: return "Resn   ";
    case 72: return "Rel Tim";
    case 73: return "Atk Tim";
    case 74: return "Cutoff ";
    case 75: return "Dcy Tim";
    case 76: return "Vib Rte";
    case 77: return "Vib Dpt";
    case 78: return "Vib Dly";
    case 88: return "HiResVl";
    case 91: return "Reverb ";
    case 92: return "Tremolo";
    case 93: return "Chorus ";
    case 94: return "Detune ";
    case 95: return "Phaser ";
    default: {
      static char _buf[8];
      if (n >= 33 && n <= 63) {
        snprintf(_buf, sizeof(_buf), "LSB %02d ", n - 32);
      } else if (n >= 102 && n <= 119) {
        snprintf(_buf, sizeof(_buf), "Usr%03d ", n);
      } else {
        snprintf(_buf, sizeof(_buf), "       ");
      }
      return _buf;
    }
  }
}

// next_valid_cc() — advance CC number by dir (+1 or -1), wrapping within 1–119
// and skipping forbidden values: 32 (Bank LSB) and 96–101 (RPN/NRPN).
static uint8_t next_valid_cc(uint8_t current, int dir) {
  int n = (int)current + dir;
  for (;;) {
    if (n < 1)   n = 119;
    if (n > 119) n = 1;
    if (n == 32 || (n >= 96 && n <= 101)) { n += dir; continue; }
    return (uint8_t)n;
  }
}

// config_menu.ino
//
// Modal config menu entered by double-tapping the knob_mode button (400 ms).
//
// Navigation:
//   Tempo knob CW   — scroll to next item / increment value
//   Tempo knob CCW  — scroll to previous item / decrement value
//   Swing knob CCW  — cancel/exit (replaces D-pad left)
//   Swing knob CW   — exit on "Exit" item (replaces D-pad right)
//   param_rec       — select / confirm (replaces Enter button)
//
// Note: Swing is a hardware knob in Beatseqr — it is NOT a config menu item.

#define CONFIG_ITEM_EXIT          0
#define CONFIG_ITEM_SAVE          1
#define CONFIG_ITEM_CLEAR_PAT     2
#define CONFIG_ITEM_CLEAR_ALL     3
#define CONFIG_ITEM_RESET_SLIDERS 4
#define CONFIG_ITEM_MODE          5
#define CONFIG_ITEM_CLOCK         6
#define CONFIG_ITEM_CHANNEL       7
#define CONFIG_ITEM_DIAGNOSTICS   8
#define CONFIG_ITEM_OCTAVE_SHIFT  9
#define CONFIG_ITEM_NOTE_SHIFT    10
#define CONFIG_ITEM_NOTE_RANGE    11
#define CONFIG_ITEM_NOTE_SCALES   12
#define CONFIG_ITEM_PAT_LENGTH    13
#define CONFIG_ITEM_PAT_DIR       14
#define CONFIG_ITEM_CC_NUMBER     15
#define CONFIG_ITEM_TEMPO         16
#define CONFIG_ITEM_SWING         17
#define CONFIG_MENU_ITEM_COUNT    18

static const char* config_labels[CONFIG_MENU_ITEM_COUNT] = {
  "Exit          ",   // 14 chars each
  "Save          ",
  "Clear pattern ",
  "Clear all pats",
  "Reset sliders ",
  "Mode:         ",
  "Clock:        ",
  "Channel:      ",
  "Diagnostics   ",
  "Octave shift  ",
  "Note shift    ",
  "Note range    ",
  "Note scales   ",
  "Pat length    ",
  "Pat dir:      ",
  "CC num:       ",
  "Tempo:        ",
  "Swing:        "
};

// Build the 14-char label for a given item index.
void print_config_label(uint8_t item) {
  char _buf[15];
  if (item == CONFIG_ITEM_MODE) {
    lcd.print(advanced_mode ? "Mode: Advanced" : "Mode: Simple  ");
  } else if (item == CONFIG_ITEM_CLOCK) {
    lcd.print(external_clock_mode ? "Clock: ext    " : "Clock: int    ");
  } else if (item == CONFIG_ITEM_CHANNEL) {
    snprintf(_buf, sizeof(_buf), "Channel:   %02d ", MIDICHANNEL);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_OCTAVE_SHIFT) {
    lcd.print(octave_shift != 0 ? "Octave shift *" : "Octave shift  ");
  } else if (item == CONFIG_ITEM_NOTE_SHIFT) {
    lcd.print(note_shift != 0   ? "Note shift   *" : "Note shift    ");
  } else if (item == CONFIG_ITEM_NOTE_RANGE) {
    bool non_default = (slider_map_low_value != 36 || slider_map_high_value != 51);
    lcd.print(non_default ? "Note range   *" : "Note range    ");
  } else if (item == CONFIG_ITEM_NOTE_SCALES) {
    bool non_default = (scale_type != 0 || scale_root != 0);
    lcd.print(non_default ? "Note scales  *" : "Note scales   ");
  } else if (item == CONFIG_ITEM_PAT_LENGTH) {
    lcd.print(pattern_length != 16 ? "Pat length   *" : "Pat length    ");
  } else if (item == CONFIG_ITEM_PAT_DIR) {
    const char* dname;
    switch (pattern_direction) {
      case 1: dname = "Rev "; break;
      case 2: dname = "Pong"; break;
      case 3: dname = "Rand"; break;
      case 4: dname = "Shuf"; break;
      case 5: dname = "E/O "; break;
      case 6: dname = "In  "; break;
      case 7: dname = "Quad"; break;
      default: dname = "Fwd "; break;
    }
    snprintf(_buf, sizeof(_buf), "Pat dir:%-6s", dname);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_CC_NUMBER) {
    snprintf(_buf, sizeof(_buf), "CC:%03d %-7s", cc_number[pattern_value], cc_name(cc_number[pattern_value]));
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_TEMPO) {
    snprintf(_buf, sizeof(_buf), "Tempo:     %3d", (int)TEMPO);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_SWING) {
    snprintf(_buf, sizeof(_buf), "Swing:      %2d", (int)SWING);
    lcd.print(_buf);
  } else {
    lcd.print(config_labels[item]);
  }
}

// ---------------------------------------------------------------------------

void draw_config_menu() {
  lcd.print("?x00?y0");
  lcd.print("> ");
  print_config_label(config_menu_item);

  lcd.print("?x00?y1");
  if (config_confirm_pending) {
    lcd.print("Entr=ok  Lft=no ");
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_CHANNEL) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Channel: %d", MIDICHANNEL);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_OCTAVE_SHIFT) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Oct: %+d", octave_shift);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_NOTE_SHIFT) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Note: %+d", note_shift);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_NOTE_RANGE) {
    char line2[17];
    int len;
    if (config_note_range_phase == 0) {
      len = snprintf(line2, sizeof(line2), "Edit Lo: %d", slider_map_low_value);
    } else {
      len = snprintf(line2, sizeof(line2), "Edit Hi: %d", slider_map_high_value);
    }
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_PAT_LENGTH) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Length: %d", pattern_length);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
    for (int i = 0; i < 16; i++) {
      if (i < pattern_length) step_leds[i].on();
      else                     step_leds[i].off();
    }
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_PAT_DIR) {
    const char* dname;
    switch (pattern_direction) {
      case 1: dname = "Rev";  break;
      case 2: dname = "Pong"; break;
      case 3: dname = "Rand"; break;
      case 4: dname = "Shuf"; break;
      case 5: dname = "E/O";  break;
      case 6: dname = "In";   break;
      case 7: dname = "Quad"; break;
      default: dname = "Fwd"; break;
    }
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Dir: %s", dname);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_CC_NUMBER) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "CC: %03d %s", cc_number[pattern_value], cc_name(cc_number[pattern_value]));
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_NOTE_SCALES) {
    char line2[17];
    int len;
    if (config_scale_phase == 0) {
      len = snprintf(line2, sizeof(line2), "  Sc: %-10s", SCALE_NAMES[scale_type]);
    } else {
      len = snprintf(line2, sizeof(line2), "  Root: %-8s", ROOT_NAMES[scale_root]);
    }
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_TEMPO) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  BPM: %d", (int)TEMPO);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_SWING) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Swing: %d", (int)SWING);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else {
    uint8_t next = (config_menu_item + 1) % CONFIG_MENU_ITEM_COUNT;
    lcd.print("  ");
    print_config_label(next);
  }
}

void enter_config_menu() {
  config_menu_active      = true;
  config_menu_item        = 0;
  config_confirm_pending  = false;
  config_editing_value    = false;
  // Anchor jog baselines so initial knob position isn't misread as movement.
  enter_knob_jog_mode();
  lcd.print("?f");
  draw_config_menu();
}

void exit_config_menu() {
  config_menu_active      = false;
  config_confirm_pending  = false;
  config_editing_value    = false;
  config_note_range_phase = 0;
  config_scale_phase      = 0;
  // Restore step LEDs to current voice's pattern data.
  read_step_memory(current_voice, pattern_value);
  update_line1 = true;
  update_line2 = true;
  lcdflag      = 255;
  next_lcdflag = 255;
  // No cursor flag in Beatseqr (no D-pad).
}

void run_config_menu() {
  if (!config_menu_active) return;

  // Read knob jog events once per call — edge-triggered, fire at most once per
  // threshold crossing. +1 = CW / "more",  -1 = CCW / "less".
  int jog_v = knob_jog_vertical();    // tempo knob → up/down / increment/decrement
  int jog_h = knob_jog_horizontal();  // swing knob → left(cancel) / right(exit)

  // Swing knob left: exit editing → exit confirmation → exit menu.
  if (jog_h < 0) {
    if (config_editing_value) {
      if (config_menu_item == CONFIG_ITEM_PAT_LENGTH)
        read_step_memory(current_voice, pattern_value);
      config_editing_value    = false;
      config_note_range_phase = 0;
      config_scale_phase      = 0;
      draw_config_menu();
    } else if (config_confirm_pending) {
      config_confirm_pending = false;
      draw_config_menu();
    } else {
      exit_config_menu();
    }
    return;
  }

  // Value editing sub-state: tempo knob adjusts the value; param_rec exits.
  if (config_editing_value) {
    // jog_v > 0 = CW = increment; jog_v < 0 = CCW = decrement.
    if (jog_v != 0) {
      if (config_menu_item == CONFIG_ITEM_CHANNEL) {
        if (jog_v > 0 && MIDICHANNEL < 16) { MIDICHANNEL++; draw_config_menu(); }
        if (jog_v < 0 && MIDICHANNEL > 1)  { MIDICHANNEL--; draw_config_menu(); }
      } else if (config_menu_item == CONFIG_ITEM_OCTAVE_SHIFT) {
        if (jog_v > 0 && octave_shift < 5)  { octave_shift++; draw_config_menu(); }
        if (jog_v < 0 && octave_shift > -5) { octave_shift--; draw_config_menu(); }
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SHIFT) {
        if (jog_v > 0 && note_shift < 12)   { note_shift++; draw_config_menu(); }
        if (jog_v < 0 && note_shift > -12)  { note_shift--; draw_config_menu(); }
      } else if (config_menu_item == CONFIG_ITEM_NOTE_RANGE) {
        if (config_note_range_phase == 0) {
          if (jog_v > 0 && slider_map_low_value < slider_map_high_value - 1) {
            slider_map_low_value++;
            init_blank_patterns_to_range(); build_scale_notes(); draw_config_menu();
          }
          if (jog_v < 0 && slider_map_low_value > 0) {
            slider_map_low_value--;
            init_blank_patterns_to_range(); build_scale_notes(); draw_config_menu();
          }
        } else {
          if (jog_v > 0 && slider_map_high_value < 127) {
            slider_map_high_value++;
            init_blank_patterns_to_range(); build_scale_notes(); draw_config_menu();
          }
          if (jog_v < 0 && slider_map_high_value > slider_map_low_value + 1) {
            slider_map_high_value--;
            init_blank_patterns_to_range(); build_scale_notes(); draw_config_menu();
          }
        }
      } else if (config_menu_item == CONFIG_ITEM_PAT_LENGTH) {
        if (jog_v > 0 && pattern_length < 16) {
          pattern_length++;
          seq.setSteps(pattern_length);
          if (pattern_direction == 4) init_shuffle();
          draw_config_menu();
        }
        if (jog_v < 0 && pattern_length > 1) {
          pattern_length--;
          seq.setSteps(pattern_length);
          if (ping_pong_step >= pattern_length) ping_pong_step = (uint8_t)(pattern_length - 1);
          if (pattern_direction == 4) init_shuffle();
          draw_config_menu();
        }
      } else if (config_menu_item == CONFIG_ITEM_PAT_DIR) {
        // CW = cycle forward through direction list.
        if (jog_v > 0) pattern_direction = (pattern_direction + 1) % PATTERN_DIRECTION_COUNT;
        if (jog_v < 0) pattern_direction = (pattern_direction + PATTERN_DIRECTION_COUNT - 1) % PATTERN_DIRECTION_COUNT;
        if (pattern_direction == 2) { ping_pong_going_forward = true; ping_pong_step = 0; }
        if (pattern_direction == 4) init_shuffle();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SCALES) {
        if (config_scale_phase == 0) {
          if (jog_v > 0) scale_type = (scale_type + 1) % SCALE_COUNT;
          if (jog_v < 0) scale_type = (scale_type + SCALE_COUNT - 1) % SCALE_COUNT;
        } else {
          if (jog_v > 0) scale_root = (scale_root + 1) % 12;
          if (jog_v < 0) scale_root = (scale_root + 12 - 1) % 12;
        }
        apply_scale_to_all_patterns();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_CC_NUMBER) {
        if (jog_v > 0) cc_number[pattern_value] = next_valid_cc(cc_number[pattern_value], +1);
        if (jog_v < 0) cc_number[pattern_value] = next_valid_cc(cc_number[pattern_value], -1);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_TEMPO) {
        if (jog_v > 0 && TEMPO < (float)upper_BPM_number) {
          TEMPO += 1.0f;
          seq.setTempo(TEMPO);
          setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
          update_line1 = true;
          draw_config_menu();
        }
        if (jog_v < 0 && TEMPO > (float)lower_BPM_number) {
          TEMPO -= 1.0f;
          seq.setTempo(TEMPO);
          setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
          update_line1 = true;
          draw_config_menu();
        }
      } else if (config_menu_item == CONFIG_ITEM_SWING) {
        if (jog_v > 0 && SWING < 5) { SWING++; draw_config_menu(); }
        if (jog_v < 0 && SWING > 0) { SWING--; draw_config_menu(); }
      }
    }

    // Step button shortcut: tap step N while editing PAT_LENGTH → length = N+1.
    if (config_menu_item == CONFIG_ITEM_PAT_LENGTH) {
      for (int i = 0; i < 16; i++) {
        if (step_buttons[i].uniquePress()) {
          pattern_length = (uint8_t)(i + 1);
          seq.setSteps(pattern_length);
          if (ping_pong_step >= pattern_length) ping_pong_step = (uint8_t)(pattern_length - 1);
          if (pattern_direction == 4) init_shuffle();
          draw_config_menu();
          break;
        }
      }
    }

    if (param_rec_flag) {
      param_rec_flag = false;
      if (config_menu_item == CONFIG_ITEM_NOTE_RANGE && config_note_range_phase == 0) {
        config_note_range_phase = 1;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SCALES && config_scale_phase == 0) {
        config_scale_phase = 1;
        draw_config_menu();
      } else {
        if (config_menu_item == CONFIG_ITEM_PAT_LENGTH)
          read_step_memory(current_voice, pattern_value);
        config_editing_value    = false;
        config_note_range_phase = 0;
        config_scale_phase      = 0;
        draw_config_menu();
      }
    }
    return;
  }

  // Confirmation pending: only param_rec (confirm) and swing-left (cancel) act.
  if (config_confirm_pending) {
    if (param_rec_flag) {
      param_rec_flag = false;
      switch (config_menu_item) {
        case CONFIG_ITEM_CLEAR_PAT:
          clear_pattern_memory_for_voice(current_voice);
          break;
        case CONFIG_ITEM_CLEAR_ALL:
          clear_pattern_memory();
          break;
        case CONFIG_ITEM_RESET_SLIDERS:
          resetSliders();
          break;
      }
      config_confirm_pending = false;
      draw_config_menu();
    }
    return;
  }

  // Menu scroll: tempo knob CW = next item, CCW = previous item.
  if (jog_v > 0) {
    config_menu_item = (config_menu_item + 1) % CONFIG_MENU_ITEM_COUNT;
    draw_config_menu();
  } else if (jog_v < 0) {
    config_menu_item = (config_menu_item + CONFIG_MENU_ITEM_COUNT - 1) % CONFIG_MENU_ITEM_COUNT;
    draw_config_menu();
  }

  // Swing knob CW: exit only when cursor is on Exit item.
  if (jog_h > 0 && config_menu_item == CONFIG_ITEM_EXIT) {
    exit_config_menu();
    return;
  }

  // param_rec selects the current item.
  if (param_rec_flag) {
    param_rec_flag = false;
    switch (config_menu_item) {
      case CONFIG_ITEM_EXIT:
        exit_config_menu();
        break;
      case CONFIG_ITEM_SAVE:
        if (playstatus) {
          lcd.print("?x00?y1");
          lcd.print("Stop first!     ");
        } else {
          save_everywhere();
          lcdflag = 202; next_lcdflag = 202;
          exit_config_menu();
        }
        break;
      case CONFIG_ITEM_CLEAR_PAT:
      case CONFIG_ITEM_CLEAR_ALL:
      case CONFIG_ITEM_RESET_SLIDERS:
        config_confirm_pending = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_MODE:
        advanced_mode = !advanced_mode;
        Serial.print("mode: ");
        Serial.println(advanced_mode ? "Advanced" : "Simple");
        draw_config_menu();
        break;
      case CONFIG_ITEM_CLOCK:
        setExternalClockMode(!external_clock_mode);
        draw_config_menu();
        break;
      case CONFIG_ITEM_DIAGNOSTICS:
        exit_config_menu();
        enter_diagnostics();
        break;
      case CONFIG_ITEM_CHANNEL:
      case CONFIG_ITEM_OCTAVE_SHIFT:
      case CONFIG_ITEM_NOTE_SHIFT:
      case CONFIG_ITEM_NOTE_RANGE:
      case CONFIG_ITEM_NOTE_SCALES:
      case CONFIG_ITEM_PAT_LENGTH:
      case CONFIG_ITEM_PAT_DIR:
      case CONFIG_ITEM_CC_NUMBER:
      case CONFIG_ITEM_TEMPO:
      case CONFIG_ITEM_SWING:
        config_editing_value    = true;
        config_note_range_phase = 0;
        config_scale_phase      = 0;
        draw_config_menu();
        break;
    }
  }
}
