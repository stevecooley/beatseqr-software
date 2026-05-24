// cc_name()
//
// Returns a 7-char (max) display name for a MIDI CC number.
// Used in config menu label (14 chars) and editing line 2 (16 chars).
// Safe CC range: 1–119, skipping 32 (Bank LSB) and 96–101 (RPN/NRPN).
// Non-static so LCD.ino can use it for live CC mode lane labels.
//
const char* cc_name(uint8_t n) {
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

// next_valid_cc()
//
// Advance CC number by dir (+1 or -1), wrapping within 1–119 and skipping
// forbidden values: 32 (Bank LSB) and 96–101 (RPN/NRPN data entry).
// Non-static so navigation.ino can use it for live CC lane editing.
//
uint8_t next_valid_cc(uint8_t current, int dir) {
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
// Modal config menu entered by double-tapping the Enter button.
//
// Navigation:
//   D-pad up/down   — scroll through items
//   D-pad left      — exit from anywhere (also cancels confirmation)
//   D-pad right     — exit when cursor is on "Exit" item
//   Enter           — select item; on "Exit" exits; on destructive items shows
//                     confirmation on line 2; on Mode item toggles immediately
//
// Menu items (alphabetical order):
//   0  About           (shows beatseqr.com + firmware version; Enter or Left exits)
//   1  CC num
//   2  Channel
//   3  Clear/Reset     (opens Reset/Clear submenu)
//   4  Clock
//   5  Diagnostics
//   6  Exit
//   7  Features
//   8  Live CC ch      (hidden when ft_live_cc_mode is off)
//   9  Mode            (toggles Simple / Advanced; confirmation required)
//  10  Note range
//  11  Note scales
//  12  Note shift
//  13  Octave shift
//  14  Pat dir
//  15  Pat length
//  16  Pitch drift
//  17  Save
//  18  Step prob
//  19  Swing
//  20  Tempo

// Reset/Clear submenu items (alphabetical):
//   0  Clear all pats  (confirmation required)
//   1  Clear pattern   (confirmation required)
//   2  Reset sliders   (confirmation required)

#define CONFIG_ITEM_ABOUT         0
#define CONFIG_ITEM_CC_NUMBER     1
#define CONFIG_ITEM_CHANNEL       2
#define CONFIG_ITEM_CLEAR_RESET   3
#define CONFIG_ITEM_CLOCK         4
#define CONFIG_ITEM_DIAGNOSTICS   5
#define CONFIG_ITEM_EXIT          6
#define CONFIG_ITEM_FEATURES      7
#define CONFIG_ITEM_LIVE_CC_CHAN  8
#define CONFIG_ITEM_MODE          9
#define CONFIG_ITEM_NOTE_RANGE    10
#define CONFIG_ITEM_NOTE_SCALES   11
#define CONFIG_ITEM_NOTE_SHIFT    12
#define CONFIG_ITEM_OCTAVE_SHIFT  13
#define CONFIG_ITEM_PAT_DIR       14
#define CONFIG_ITEM_PAT_LENGTH    15
#define CONFIG_ITEM_PITCH_DRIFT   16
#define CONFIG_ITEM_SAVE          17
#define CONFIG_ITEM_STEP_PROB     18
#define CONFIG_ITEM_SWING         19
#define CONFIG_ITEM_TEMPO         20
#define CONFIG_MENU_ITEM_COUNT    21

#define RESET_ITEM_CLEAR_ALL  0
#define RESET_ITEM_CLEAR_PAT  1
#define RESET_ITEM_SLIDERS    2
#define RESET_ITEM_COUNT      3

#define NR_ITEM_CUSTOM  0
#define NR_ITEM_16      1
#define NR_ITEM_12      2
#define NR_ITEM_8       3
#define NR_ITEM_6       4
#define NR_ITEM_4       5
#define NR_ITEM_COUNT   6

#define DIAG_ITEM_LED_TEST    0
#define DIAG_ITEM_INPUT_TEST  1
#define DIAG_ITEM_HI_TRIM     2
#define DIAG_ITEM_COUNT       3

// line1_label: 14 chars printed after "> " on line 1.
// Items with inline values are rendered dynamically in print_config_label().
static const char* config_labels[CONFIG_MENU_ITEM_COUNT] = {
  "About         ",   //  0 ABOUT         — Enter shows beatseqr.com + firmware version
  "CC num:       ",   //  1 CC_NUMBER     — value overwritten at draw time
  "Channel:      ",   //  2 CHANNEL       — value overwritten at draw time
  "Clear/Reset   ",   //  3 CLEAR_RESET   — opens Reset/Clear submenu
  "Clock:        ",   //  4 CLOCK         — value overwritten at draw time
  "Diagnostics   ",   //  5 DIAGNOSTICS
  "Exit          ",   //  6 EXIT
  "Features      ",   //  7 FEATURES
  "Live CC ch:   ",   //  8 LIVE_CC_CHAN  — value overwritten at draw time
  "Mode:         ",   //  9 MODE          — value overwritten at draw time
  "Note range    ",   // 10 NOTE_RANGE
  "Note scales   ",   // 11 NOTE_SCALES   — * appended when non-default
  "Note shift    ",   // 12 NOTE_SHIFT    — * appended when non-zero
  "Octave shift  ",   // 13 OCTAVE_SHIFT  — * appended when non-zero
  "Pat dir:      ",   // 14 PAT_DIR       — value overwritten at draw time
  "Pat length    ",   // 15 PAT_LENGTH    — * appended when not 16
  "Pitch drift   ",   // 16 PITCH_DRIFT   — * appended when non-zero
  "Save          ",   // 17 SAVE
  "Step prob     ",   // 18 STEP_PROB     — * appended if any step < 100
  "Swing:        ",   // 19 SWING         — value overwritten at draw time
  "Tempo         "    // 20 TEMPO         — value overwritten at draw time; hidden when ext clock
};

// Tempo resolution index while editing: 0=±10, 1=±1, 2=±0.1.
static uint8_t config_tempo_res = 0;

// Build the 14-char label for a given item index.
// For MODE, replaces trailing spaces with the current value.
void print_config_label(uint8_t item) {
  char _buf[15];
  if (item == CONFIG_ITEM_TEMPO) {
    snprintf(_buf, sizeof(_buf), "Tempo: %5.1f  ", TEMPO);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_MODE) {
    if (config_confirm_pending) {
      // Show target mode so it's clear what Enter will do
      lcd.print(advanced_mode ? "Mode:->Simple " : "Mode:->Advancd");
    } else {
      lcd.print(advanced_mode ? "Mode: Advanced" : "Mode: Simple  ");
    }
  } else if (item == CONFIG_ITEM_CLOCK) {
    // "Clock: int    " or "Clock: ext    " — 14 chars
    lcd.print(external_clock_mode ? "Clock: ext    " : "Clock: int    ");
  } else if (item == CONFIG_ITEM_CHANNEL) {
    // "Channel:   02 " — 14 chars
    snprintf(_buf, sizeof(_buf), "Channel:   %02d ", MIDICHANNEL);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_LIVE_CC_CHAN) {
    // "Live CC ch:02 " — 14 chars
    snprintf(_buf, sizeof(_buf), "Live CC ch:%02d ", live_cc_channel);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_SWING) {
    // "Swing:        " with value inline — 14 chars
    snprintf(_buf, sizeof(_buf), "Swing:      %d ", SWING);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_OCTAVE_SHIFT) {
    lcd.print(octave_shift != 0 ? "Octave shift *" : "Octave shift  ");
  } else if (item == CONFIG_ITEM_NOTE_SHIFT) {
    lcd.print(note_shift != 0  ? "Note shift   *" : "Note shift    ");
  } else if (item == CONFIG_ITEM_NOTE_RANGE) {
    bool non_default = (slider_map_low_value != 36 || slider_map_high_value != 52);
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
    // 14 chars: "Pat dir:Fwd   " etc.
    snprintf(_buf, sizeof(_buf), "Pat dir:%-6s", dname);
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_CC_NUMBER) {
    // "CC:%03d %-7s" = 3+3+1+7 = 14 chars
    snprintf(_buf, sizeof(_buf), "CC:%03d %-7s", cc_number[pattern_value], cc_name(cc_number[pattern_value]));
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_STEP_PROB) {
    bool non_default = false;
    for (int s = 0; s < 16; s++) {
      if (step_probability[pattern_value][s] < 100) { non_default = true; break; }
    }
    lcd.print(non_default ? "Step prob    *" : "Step prob     ");
  } else if (item == CONFIG_ITEM_PITCH_DRIFT) {
    lcd.print(pitch_drift != 0 ? "Pitch drift  *" : "Pitch drift   ");
  } else {
    lcd.print(config_labels[item]);
  }
}

// ---------------------------------------------------------------------------

// config_menu_next()
//
// Advance the menu cursor by dir (+1 = down, -1 = up), wrapping around, and
// skip any items whose feature is compiled out.  Returns `from` unchanged if
// every item is disabled (prevents an infinite loop when all features are off,
// though in practice at least Exit/Save/Channel are always enabled).
//
static uint8_t config_menu_next(uint8_t from, int dir) {
  uint8_t cur = from;
  do {
    cur = (uint8_t)((cur + CONFIG_MENU_ITEM_COUNT + dir) % CONFIG_MENU_ITEM_COUNT);
    bool enabled = true;
    switch (cur) {
      case CONFIG_ITEM_TEMPO:        if (external_clock_mode)     enabled = false; break;
      case CONFIG_ITEM_MODE:         if (!ft_advanced_mode)       enabled = false; break;
      case CONFIG_ITEM_CLOCK:        if (!ft_external_clock)      enabled = false; break;
      case CONFIG_ITEM_SWING:        if (!ft_swing)               enabled = false; break;
      case CONFIG_ITEM_DIAGNOSTICS:  if (!ft_diagnostics)         enabled = false; break;
      case CONFIG_ITEM_OCTAVE_SHIFT: if (!ft_octave_note_shift)   enabled = false; break;
      case CONFIG_ITEM_NOTE_SHIFT:   if (!ft_octave_note_shift)   enabled = false; break;
      case CONFIG_ITEM_NOTE_SCALES:  if (!ft_scale_quantization)  enabled = false; break;
      case CONFIG_ITEM_PAT_LENGTH:   if (!ft_variable_pat_length) enabled = false; break;
      case CONFIG_ITEM_PAT_DIR:      if (!ft_pattern_direction)   enabled = false; break;
      case CONFIG_ITEM_CC_NUMBER:    if (!ft_cc_mode)             enabled = false; break;
      case CONFIG_ITEM_STEP_PROB:    if (!ft_probability)         enabled = false; break;
      case CONFIG_ITEM_PITCH_DRIFT:  if (!ft_pitch_drift)         enabled = false; break;
      case CONFIG_ITEM_LIVE_CC_CHAN: if (!ft_live_cc_mode)        enabled = false; break;
      default: break;
    }
    if (enabled) return cur;
  } while (cur != from);
  return from;
}

void draw_config_menu() {
  // Line 1: "> {current item label}" — 16 chars total
  lcd.print("?x00?y0");
  lcd.print("> ");
  print_config_label(config_menu_item);

  // Line 2: confirmation prompt, value editor, or next-item preview
  lcd.print("?x00?y1");
  if (config_confirm_pending) {
    lcd.print("Entr=ok  Lft=no ");
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_TEMPO) {
    static const char* res_labels[] = {"+/-10 BPM", "+/-1 BPM", "+/-0.1 BPM"};
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  %s", res_labels[config_tempo_res]);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_CHANNEL) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Channel: %d", MIDICHANNEL);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_LIVE_CC_CHAN) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  LiveCC ch: %d", live_cc_channel);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_SWING) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Swing: %d", SWING);
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
    // Light step LEDs to visualise the current pattern length.
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
    // "CC: 001 Mod Whl " — 4+3+1+7+1 = 16 chars
    int len = snprintf(line2, sizeof(line2), "CC: %03d %s", cc_number[pattern_value], cc_name(cc_number[pattern_value]));
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_PITCH_DRIFT) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Drift: %d", pitch_drift);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_NOTE_SCALES) {
    char line2[17];
    int len;
    if (config_scale_phase == 0) {
      // Sub-state 0: choose scale type. "  Sc: %-10s" = 16 chars.
      len = snprintf(line2, sizeof(line2), "  Sc: %-10s", SCALE_NAMES[scale_type]);
    } else {
      // Sub-state 1: choose root note. "  Root: %-8s" = 16 chars.
      len = snprintf(line2, sizeof(line2), "  Root: %-8s", ROOT_NAMES[scale_root]);
    }
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else {
    uint8_t next = config_menu_next(config_menu_item, 1);
    lcd.print("  ");
    print_config_label(next);
  }
}

void enter_config_menu() {
  config_menu_active = true;
  // Restore last position; clamp in case item count changed since last open.
  if (config_menu_item >= CONFIG_MENU_ITEM_COUNT) config_menu_item = 0;
  config_confirm_pending = false;
  config_editing_value = false;
  // Clear any Enter flag that arrived as part of the opening double-tap so it
  // doesn't immediately select the first menu item on the first run_config_menu().
  enterbutton_flag = false;
  draw_config_menu();
}

void exit_config_menu() {
  config_menu_active = false;
  config_confirm_pending = false;
  config_editing_value = false;
  config_note_range_phase = 0;
  config_scale_phase = 0;
  config_tempo_res = 0;
  config_features_active = false;
  config_reset_active    = false;
  config_reset_item      = 0;
  config_reset_confirm   = false;
  config_nr_submenu_active = false;
  config_nr_item           = 0;
  config_nr_custom_active  = false;
  config_diag_submenu_active = false;
  config_diag_item           = 0;
  config_diag_editing_trim   = false;
  config_about_active        = false;
  // Drop any pattern-button state that may have been accumulated while the
  // menu was open. Without this, a stray uniquePress() during menu navigation
  // can fire nav-mode toggle right after exit — visually indistinguishable
  // from advanced mode activating on its own.
  adv_pat0_press_ms = 0;
  for (int i = 0; i < 4; i++) pattern_select_button_flags[i] = false;
  // If Simple mode, always clear advanced sub-states — guards against any
  // scenario where adv_pat_nav_active was left true with advanced_mode false.
  if (!advanced_mode) {
    adv_pat_nav_active = false;
    adv_copy_waiting_source = false;
    adv_copy_armed = false;
    adv_chain_hold_step = -1;
  }
  // Restore step LEDs for the active slider mode (in case PAT_LENGTH was editing).
  // CC mode shows cc_step_enabled; LV mode shows only the editing lane (if any).
  if (slider_mode == 4) {
    read_cc_step_memory();
  } else if (slider_mode == 6) {
    clear_step_leds();
    if (live_cc_editing_lane >= 0) step_leds[live_cc_editing_lane].on();
  } else {
    read_step_memory(0, pattern_value);
  }
  // Force a full LCD redraw back to the main display.
  update_line1 = true;
  update_line2 = true;
  lcdflag = 255;
  next_lcdflag = 255;
  cursor_flag = true;
}

void run_config_menu() {
  if (!config_menu_active) return;

  // Submenus are modal within the config menu.
  if (config_features_active) {
    run_features_submenu();
    return;
  }
  if (config_reset_active) {
    run_reset_submenu();
    return;
  }
  if (config_nr_submenu_active) {
    run_note_range_submenu();
    return;
  }
  if (config_diag_submenu_active) {
    run_diag_submenu();
    return;
  }
  if (config_about_active) {
    run_about_screen();
    return;
  }

  // D-pad left: exits editing/confirmation mode first, then exits menu.
  if (dpad_left_flag) {
    dpad_left_flag = false;
    if (config_editing_value) {
      if (config_menu_item == CONFIG_ITEM_PAT_LENGTH) read_step_memory(0, pattern_value);
      config_editing_value = false;
      config_note_range_phase = 0;
      config_scale_phase = 0;
      config_tempo_res = 0;
      draw_config_menu();
    } else if (config_confirm_pending) {
      config_confirm_pending = false;
      draw_config_menu();
    } else {
      exit_config_menu();
    }
    return;
  }

  // While editing a value (e.g. octave shift), up/down adjust the value.
  // Enter or Left exit editing mode and return to normal menu scroll.
  if (config_editing_value) {
    if (dpad_up_flag) {
      dpad_up_flag = false;
      if (config_menu_item == CONFIG_ITEM_TEMPO) {
        float res = (config_tempo_res == 0) ? 10.0f : (config_tempo_res == 1) ? 1.0f : 0.1f;
        TEMPO += res;
        if (TEMPO > 250.0f) TEMPO = 250.0f;
        seq.setTempo(TEMPO);
        setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_CHANNEL && MIDICHANNEL < 16) {
        MIDICHANNEL++;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_LIVE_CC_CHAN && live_cc_channel < 16) {
        live_cc_channel++;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_SWING && SWING < 5) {
        SWING++;
        seq.setShuffle(SWING);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_OCTAVE_SHIFT && octave_shift < 5) {
        octave_shift++;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SHIFT && note_shift < 12) {
        note_shift++;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_RANGE) {
        if (config_note_range_phase == 0 && slider_map_low_value < slider_map_high_value - 1) {
          slider_map_low_value++;
          init_blank_patterns_to_range();
          build_scale_notes();
          draw_config_menu();
        } else if (config_note_range_phase == 1 && slider_map_high_value < 127) {
          slider_map_high_value++;
          init_blank_patterns_to_range();
          build_scale_notes();
          draw_config_menu();
        }
      } else if (config_menu_item == CONFIG_ITEM_PAT_LENGTH && pattern_length < 16) {
        pattern_length++;
        seq.setSteps(pattern_length);
        if (pattern_direction == 4) init_shuffle();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_PAT_DIR) {
        pattern_direction = (pattern_direction + PATTERN_DIRECTION_COUNT - 1) % PATTERN_DIRECTION_COUNT;
        if (pattern_direction == 2) { ping_pong_going_forward = true; ping_pong_step = 0; }
        if (pattern_direction == 4) init_shuffle();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SCALES) {
        if (config_scale_phase == 0) {
          scale_type = (scale_type + SCALE_COUNT - 1) % SCALE_COUNT;
        } else {
          scale_root = (scale_root + 12 - 1) % 12;
        }
        apply_scale_to_all_patterns();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_CC_NUMBER) {
        cc_number[pattern_value] = next_valid_cc(cc_number[pattern_value], +1);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_PITCH_DRIFT && pitch_drift < 7) {
        pitch_drift++;
        draw_config_menu();
      }
    }
    if (dpad_down_flag) {
      dpad_down_flag = false;
      if (config_menu_item == CONFIG_ITEM_TEMPO) {
        float res = (config_tempo_res == 0) ? 10.0f : (config_tempo_res == 1) ? 1.0f : 0.1f;
        TEMPO -= res;
        if (TEMPO < 10.0f) TEMPO = 10.0f;
        seq.setTempo(TEMPO);
        setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_CHANNEL && MIDICHANNEL > 1) {
        MIDICHANNEL--;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_LIVE_CC_CHAN && live_cc_channel > 1) {
        live_cc_channel--;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_SWING && SWING > 0) {
        SWING--;
        seq.setShuffle(SWING);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_OCTAVE_SHIFT && octave_shift > -5) {
        octave_shift--;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SHIFT && note_shift > -12) {
        note_shift--;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_RANGE) {
        if (config_note_range_phase == 0 && slider_map_low_value > 0) {
          slider_map_low_value--;
          init_blank_patterns_to_range();
          build_scale_notes();
          draw_config_menu();
        } else if (config_note_range_phase == 1 && slider_map_high_value > slider_map_low_value + 1) {
          slider_map_high_value--;
          init_blank_patterns_to_range();
          build_scale_notes();
          draw_config_menu();
        }
      } else if (config_menu_item == CONFIG_ITEM_PAT_LENGTH && pattern_length > 1) {
        pattern_length--;
        seq.setSteps(pattern_length);
        if (ping_pong_step >= pattern_length) ping_pong_step = (uint8_t)(pattern_length - 1);
        if (pattern_direction == 4) init_shuffle();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_PAT_DIR) {
        pattern_direction = (pattern_direction + 1) % PATTERN_DIRECTION_COUNT;
        if (pattern_direction == 2) { ping_pong_going_forward = true; ping_pong_step = 0; }
        if (pattern_direction == 4) init_shuffle();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SCALES) {
        if (config_scale_phase == 0) {
          scale_type = (scale_type + 1) % SCALE_COUNT;
        } else {
          scale_root = (scale_root + 1) % 12;
        }
        apply_scale_to_all_patterns();
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_CC_NUMBER) {
        cc_number[pattern_value] = next_valid_cc(cc_number[pattern_value], -1);
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_PITCH_DRIFT && pitch_drift > 0) {
        pitch_drift--;
        draw_config_menu();
      }
    }
    // Step button shortcut: tap step N while editing PAT_LENGTH → length = N+1.
    // uniquePress() here consumes the event before run_step_button_routine() sees it.
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
    if (enterbutton_flag) {
      enterbutton_flag = false;
      if (config_menu_item == CONFIG_ITEM_TEMPO) {
        // Cycle resolution: ±10 → ±1 → ±0.1 → ±10
        config_tempo_res = (config_tempo_res + 1) % 3;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_RANGE && config_note_range_phase == 0) {
        config_note_range_phase = 1;
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_NOTE_SCALES && config_scale_phase == 0) {
        // Advance from scale type to root note sub-state.
        config_scale_phase = 1;
        draw_config_menu();
      } else {
        if (config_menu_item == CONFIG_ITEM_PAT_LENGTH) read_step_memory(0, pattern_value);
        config_editing_value = false;
        config_note_range_phase = 0;
        config_scale_phase = 0;
        draw_config_menu();
      }
    }
    // Left is handled above and already exits editing via the global left check.
    return;
  }

  // While a confirmation is pending, only Enter (confirm) and Left (cancel,
  // handled above) are meaningful. Consume up/down/right so they don't leak.
  if (config_confirm_pending) {
    dpad_up_flag = false;
    dpad_down_flag = false;
    if (enterbutton_flag) {
      enterbutton_flag = false;
      switch (config_menu_item) {
        case CONFIG_ITEM_MODE:
          advanced_mode = !advanced_mode;
          if (!advanced_mode) {
            adv_pat_nav_active = false;
            adv_copy_waiting_source = false;
            adv_copy_armed = false;
            adv_chain_hold_step = -1;
            read_step_memory(0, pattern_value);
          } else {
            adv_pat0_press_ms = 0;
            // Buttons 2 (VL) and 3 (PR) must work in advanced mode.
            ft_velocity_mode = true;
            ft_probability   = true;
          }
          // Repaint pattern LEDs for the new mode. Without this, e.g. LED 1
          // stays lit after Advanced→Simple because the Advanced-mode slider-
          // indicator runs every frame while Advanced, but nothing repaints on
          // exit. Looks indistinguishable from "still in Advanced."
          go_to_pattern(current_pattern, 1);
          break;
      }
      config_confirm_pending = false;
      // After a destructive action, return to menu (don't auto-exit)
      draw_config_menu();
    }
    return;
  }

  // Scroll up (wraps, skips disabled items).
  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_menu_item = config_menu_next(config_menu_item, -1);
    draw_config_menu();
  }

  // Scroll down (wraps, skips disabled items).
  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_menu_item = config_menu_next(config_menu_item, 1);
    draw_config_menu();
  }

  // Enter selects the current item.
  if (enterbutton_flag) {
    enterbutton_flag = false;
    switch (config_menu_item) {
      case CONFIG_ITEM_EXIT:
        exit_config_menu();
        break;
      case CONFIG_ITEM_SAVE:
        if (playstatus) {
          // Flash a warning on line 2 — don't save while playing.
          lcd.print("?x00?y1");
          lcd.print("Stop first!     ");
        } else {
          if (!advanced_mode) {
            adv_pat_nav_active = false;
            adv_copy_waiting_source = false;
            adv_copy_armed = false;
            adv_chain_hold_step = -1;
          }
          bool sd_ok = save_to_sd();
          save_to_eeprom();
          exit_config_menu();
          // Show result: 202 = "saved!" (both ok), 203 = "EE ok, no SD!"
          lcdflag      = sd_ok ? 202 : 203;
          next_lcdflag = sd_ok ? 202 : 203;
        }
        break;
      case CONFIG_ITEM_TEMPO:
        config_editing_value = true;
        config_tempo_res = 0;
        draw_config_menu();
        break;
      case CONFIG_ITEM_CLEAR_RESET:
        config_reset_active  = true;
        config_reset_item    = 0;
        config_reset_confirm = false;
        draw_reset_submenu();
        break;
      case CONFIG_ITEM_MODE:
        config_confirm_pending = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_CLOCK:
        // Toggle between internal and external clock.
        setExternalClockMode(!external_clock_mode);
        Serial.print("clock: ");
        Serial.println(external_clock_mode ? "ext" : "int");
        draw_config_menu();
        break;
      case CONFIG_ITEM_DIAGNOSTICS:
        config_diag_submenu_active = true;
        config_diag_item           = 0;
        config_diag_editing_trim   = false;
        draw_diag_submenu();
        break;
      case CONFIG_ITEM_CHANNEL:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_LIVE_CC_CHAN:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_SWING:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_OCTAVE_SHIFT:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_NOTE_SHIFT:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_NOTE_RANGE:
        config_nr_submenu_active = true;
        config_nr_item           = 0;
        config_nr_custom_active  = false;
        config_note_range_phase  = 0;
        draw_note_range_submenu();
        break;
      case CONFIG_ITEM_NOTE_SCALES:
        config_editing_value = true;
        config_scale_phase = 0;
        draw_config_menu();
        break;
      case CONFIG_ITEM_PAT_LENGTH:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_PAT_DIR:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_CC_NUMBER:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_STEP_PROB:
        // Exit menu and activate PR slider mode so sliders control probability.
        exit_config_menu();
        set_slider_mode(5);
        break;
      case CONFIG_ITEM_PITCH_DRIFT:
        config_editing_value = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_FEATURES:
        config_features_active = true;
        config_feature_item = 0;
        draw_features_submenu();
        break;
      case CONFIG_ITEM_ABOUT:
        config_about_active = true;
        draw_about_screen();
        break;
    }
  }
}

// ---------------------------------------------------------------------------
// Reset/Clear submenu
// ---------------------------------------------------------------------------

static const char* _reset_names[RESET_ITEM_COUNT] = {
  "Clear all pats  ",
  "Clear pattern   ",
  "Reset sliders   "
};

void draw_reset_submenu() {
  lcd.print("?x00?y0");
  lcd.print(_reset_names[config_reset_item]);
  lcd.print("?x00?y1");
  if (config_reset_confirm) {
    lcd.print("Entr=ok  Lft=no ");
  } else {
    uint8_t next = (config_reset_item + 1) % RESET_ITEM_COUNT;
    lcd.print(_reset_names[next]);
  }
}

void run_reset_submenu() {
  if (!config_reset_active) return;

  if (dpad_left_flag) {
    dpad_left_flag = false;
    if (config_reset_confirm) {
      config_reset_confirm = false;
      draw_reset_submenu();
    } else {
      config_reset_active = false;
      draw_config_menu();
    }
    return;
  }

  if (config_reset_confirm) {
    dpad_up_flag   = false;
    dpad_down_flag = false;
    if (enterbutton_flag) {
      enterbutton_flag = false;
      switch (config_reset_item) {
        case RESET_ITEM_CLEAR_ALL:
          clear_pattern_memory();
          break;
        case RESET_ITEM_CLEAR_PAT:
          clear_pattern_memory_for_voice(0);
          break;
        case RESET_ITEM_SLIDERS:
          resetSliders();
          lcdflag = 255;
          next_lcdflag = 255;
          break;
      }
      config_reset_confirm = false;
      draw_reset_submenu();
    }
    return;
  }

  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_reset_item = (uint8_t)((config_reset_item + RESET_ITEM_COUNT - 1) % RESET_ITEM_COUNT);
    draw_reset_submenu();
  }

  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_reset_item = (uint8_t)((config_reset_item + 1) % RESET_ITEM_COUNT);
    draw_reset_submenu();
  }

  if (enterbutton_flag) {
    enterbutton_flag = false;
    config_reset_confirm = true;
    draw_reset_submenu();
  }
}

// ---------------------------------------------------------------------------
// Features submenu
// ---------------------------------------------------------------------------

#define FEATURE_COUNT 15

static const char* _feature_names[FEATURE_COUNT] = {
  "Advanced mode   ",
  "CC mode         ",
  "Diagnostics     ",
  "Ext clock       ",
  "Gate sliders    ",
  "Live CC mode    ",
  "Note audition   ",
  "Note scales     ",
  "Oct/note shift  ",
  "Pat direction   ",
  "Pat length      ",
  "Pitch drift     ",
  "Probability     ",
  "Swing           ",
  "Velocity sliders"
};

static bool* _feature_flag_ptrs[FEATURE_COUNT] = {
  &ft_advanced_mode,
  &ft_cc_mode,
  &ft_diagnostics,
  &ft_external_clock,
  &ft_gate_mode,
  &ft_live_cc_mode,
  &ft_note_audition,
  &ft_scale_quantization,
  &ft_octave_note_shift,
  &ft_pattern_direction,
  &ft_variable_pat_length,
  &ft_pitch_drift,
  &ft_probability,
  &ft_swing,
  &ft_velocity_mode
};

void draw_features_submenu() {
  lcd.print("?x00?y0");
  lcd.print(_feature_names[config_feature_item]);
  lcd.print("?x00?y1");
  if (*_feature_flag_ptrs[config_feature_item]) {
    lcd.print("  active        ");
  } else {
    lcd.print("  inactive      ");
  }
}

// Apply side-effects when a feature is toggled off.
static void _apply_feature_disable(uint8_t idx) {
  switch (idx) {
    case 0:  // advanced mode
      if (advanced_mode) {
        advanced_mode = false;
        adv_pat_nav_active = false;
        adv_copy_waiting_source = false;
        adv_copy_armed = false;
        adv_chain_hold_step = -1;
        read_step_memory(0, pattern_value);
        go_to_pattern(current_pattern, 1);
      }
      break;
    case 3:  // external clock
      if (external_clock_mode) setExternalClockMode(false);
      break;
    case 5:  // live CC mode — if currently in LV mode, drop back to NN
      if (slider_mode == 6) set_slider_mode(1);
      break;
    case 6:  // note audition — cancel any sounding audition note immediately
#if FEATURE_NOTE_AUDITION
      if (audition_sounding_note >= 0) {
        noteOff(MIDICHANNEL - 1, (uint8_t)audition_sounding_note, 0);
        MidiUSB.flush();
        audition_sounding_note = -1;
      }
#endif
      break;
    case 14:  // velocity sliders — if currently in VL mode, drop back to NN
      if (slider_mode == 2) set_slider_mode(1);
      break;
    default: break;
  }
}

void run_features_submenu() {
  if (!config_features_active) return;

  if (dpad_left_flag) {
    dpad_left_flag = false;
    config_features_active = false;
    draw_config_menu();
    return;
  }

  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_feature_item = (uint8_t)((config_feature_item + FEATURE_COUNT - 1) % FEATURE_COUNT);
    draw_features_submenu();
  }

  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_feature_item = (uint8_t)((config_feature_item + 1) % FEATURE_COUNT);
    draw_features_submenu();
  }

  if (enterbutton_flag) {
    enterbutton_flag = false;
    bool* flag = _feature_flag_ptrs[config_feature_item];
    *flag = !(*flag);
    if (!(*flag)) _apply_feature_disable(config_feature_item);
    draw_features_submenu();
  }
}

// ---------------------------------------------------------------------------
// Note range submenu
// ---------------------------------------------------------------------------

static const char* _nr_labels[NR_ITEM_COUNT] = {
  "Custom        ",
  "16 notes 36-51",
  "12 notes 36-47",
  "8 notes  36-43",
  "6 notes  36-41",
  "4 notes  36-39"
};

void draw_note_range_submenu() {
  lcd.print("?x00?y0");
  lcd.print("> ");
  lcd.print(_nr_labels[config_nr_item]);
  lcd.print("?x00?y1");
  if (config_nr_custom_active) {
    char line2[17];
    int len;
    if (config_note_range_phase == 0)
      len = snprintf(line2, sizeof(line2), "Edit Lo: %d", slider_map_low_value);
    else
      len = snprintf(line2, sizeof(line2), "Edit Hi: %d", slider_map_high_value);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else {
    uint8_t next = (uint8_t)((config_nr_item + 1) % NR_ITEM_COUNT);
    lcd.print("  ");
    lcd.print(_nr_labels[next]);
  }
}

void run_note_range_submenu() {
  if (!config_nr_submenu_active) return;

  if (dpad_left_flag) {
    dpad_left_flag = false;
    if (config_nr_custom_active) {
      config_nr_custom_active = false;
      config_note_range_phase = 0;
      draw_note_range_submenu();
    } else {
      config_nr_submenu_active = false;
      draw_config_menu();
    }
    return;
  }

  if (config_nr_custom_active) {
    if (dpad_up_flag) {
      dpad_up_flag = false;
      if (config_note_range_phase == 0 && slider_map_low_value < slider_map_high_value - 1) {
        slider_map_low_value++;
        init_blank_patterns_to_range();
        build_scale_notes();
        draw_note_range_submenu();
      } else if (config_note_range_phase == 1 && slider_map_high_value < 127) {
        slider_map_high_value++;
        init_blank_patterns_to_range();
        build_scale_notes();
        draw_note_range_submenu();
      }
    }
    if (dpad_down_flag) {
      dpad_down_flag = false;
      if (config_note_range_phase == 0 && slider_map_low_value > 0) {
        slider_map_low_value--;
        init_blank_patterns_to_range();
        build_scale_notes();
        draw_note_range_submenu();
      } else if (config_note_range_phase == 1 && slider_map_high_value > slider_map_low_value + 1) {
        slider_map_high_value--;
        init_blank_patterns_to_range();
        build_scale_notes();
        draw_note_range_submenu();
      }
    }
    if (enterbutton_flag) {
      enterbutton_flag = false;
      if (config_note_range_phase == 0) {
        config_note_range_phase = 1;
        draw_note_range_submenu();
      } else {
        config_nr_custom_active = false;
        config_note_range_phase = 0;
        draw_note_range_submenu();
      }
    }
    return;
  }

  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_nr_item = (uint8_t)((config_nr_item + NR_ITEM_COUNT - 1) % NR_ITEM_COUNT);
    draw_note_range_submenu();
  }
  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_nr_item = (uint8_t)((config_nr_item + 1) % NR_ITEM_COUNT);
    draw_note_range_submenu();
  }
  if (enterbutton_flag) {
    enterbutton_flag = false;
    switch (config_nr_item) {
      case NR_ITEM_CUSTOM:
        config_nr_custom_active = true;
        config_note_range_phase = 0;
        draw_note_range_submenu();
        break;
      case NR_ITEM_16:
        slider_map_low_value = 36; slider_map_high_value = 51;
        init_blank_patterns_to_range(); build_scale_notes();
        config_nr_submenu_active = false;
        draw_config_menu();
        break;
      case NR_ITEM_12:
        slider_map_low_value = 36; slider_map_high_value = 47;
        init_blank_patterns_to_range(); build_scale_notes();
        config_nr_submenu_active = false;
        draw_config_menu();
        break;
      case NR_ITEM_8:
        slider_map_low_value = 36; slider_map_high_value = 43;
        init_blank_patterns_to_range(); build_scale_notes();
        config_nr_submenu_active = false;
        draw_config_menu();
        break;
      case NR_ITEM_6:
        slider_map_low_value = 36; slider_map_high_value = 41;
        init_blank_patterns_to_range(); build_scale_notes();
        config_nr_submenu_active = false;
        draw_config_menu();
        break;
      case NR_ITEM_4:
        slider_map_low_value = 36; slider_map_high_value = 39;
        init_blank_patterns_to_range(); build_scale_notes();
        config_nr_submenu_active = false;
        draw_config_menu();
        break;
    }
  }
}

// ---------------------------------------------------------------------------
// Diagnostics submenu
// ---------------------------------------------------------------------------

static void _diag_item_label(uint8_t item, char* buf) {
  int len;
  if (item == DIAG_ITEM_LED_TEST) {
    memcpy(buf, "LED test        ", 16); buf[16] = '\0';
  } else if (item == DIAG_ITEM_INPUT_TEST) {
    memcpy(buf, "Input test      ", 16); buf[16] = '\0';
  } else {
    len = snprintf(buf, 17, "Hi trim: +%d", slider_hi_trim);
    while (len < 16) buf[len++] = ' ';
    buf[16] = '\0';
  }
}

void draw_diag_submenu() {
  char buf[17];
  lcd.print("?x00?y0");
  _diag_item_label(config_diag_item, buf);
  lcd.print(buf);
  lcd.print("?x00?y1");
  if (config_diag_editing_trim) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  Hi trim: +%d", slider_hi_trim);
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else {
    _diag_item_label((uint8_t)((config_diag_item + 1) % DIAG_ITEM_COUNT), buf);
    lcd.print(buf);
  }
}

void run_diag_submenu() {
  if (!config_diag_submenu_active) return;

  if (dpad_left_flag) {
    dpad_left_flag = false;
    if (config_diag_editing_trim) {
      config_diag_editing_trim = false;
      draw_diag_submenu();
    } else {
      config_diag_submenu_active = false;
      draw_config_menu();
    }
    return;
  }

  if (config_diag_editing_trim) {
    if (dpad_up_flag) {
      dpad_up_flag = false;
      if (slider_hi_trim < 4) { slider_hi_trim++; draw_diag_submenu(); }
    }
    if (dpad_down_flag) {
      dpad_down_flag = false;
      if (slider_hi_trim > 0) { slider_hi_trim--; draw_diag_submenu(); }
    }
    if (enterbutton_flag) {
      enterbutton_flag = false;
      config_diag_editing_trim = false;
      draw_diag_submenu();
    }
    return;
  }

  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_diag_item = (uint8_t)((config_diag_item + DIAG_ITEM_COUNT - 1) % DIAG_ITEM_COUNT);
    draw_diag_submenu();
  }
  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_diag_item = (uint8_t)((config_diag_item + 1) % DIAG_ITEM_COUNT);
    draw_diag_submenu();
  }
  if (enterbutton_flag) {
    enterbutton_flag = false;
    switch (config_diag_item) {
      case DIAG_ITEM_LED_TEST:
        enter_diag_led_test();
        break;
      case DIAG_ITEM_INPUT_TEST:
        enter_diag_input_test();
        break;
      case DIAG_ITEM_HI_TRIM:
        config_diag_editing_trim = true;
        draw_diag_submenu();
        break;
    }
  }
}

// ---------------------------------------------------------------------------
// About screen — shows "beatseqr.com" on line 1 and "firmware: <ver>" on
// line 2. Enter or D-pad left returns to the main config menu. FIRMWARE_VERSION
// comes from version.h, which is auto-generated from `git rev-list --count
// HEAD` (with a "-dev" suffix on a dirty tree) before every build.
// ---------------------------------------------------------------------------

void draw_about_screen() {
  char line2[17];
  // 16-char line: "firmware: " (10) + up to 6 chars of version. Most versions
  // ("3.147") fit easily; very long version strings get truncated by snprintf.
  int len = snprintf(line2, sizeof(line2), "firmware:%s", FIRMWARE_VERSION);
  while (len < 16) line2[len++] = ' ';
  line2[16] = '\0';

  lcd.print("?x00?y0");
  lcd.print("beatseqr.com    ");
  lcd.print("?x00?y1");
  lcd.print(line2);
}

void run_about_screen() {
  if (!config_about_active) return;

  // Any of these inputs returns to the menu. D-pad up/down also work so the
  // user can't get stuck if they instinctively scroll.
  if (enterbutton_flag || dpad_left_flag || dpad_up_flag || dpad_down_flag) {
    enterbutton_flag = false;
    dpad_left_flag   = false;
    dpad_up_flag     = false;
    dpad_down_flag   = false;
    config_about_active = false;
    draw_config_menu();
  }
}
