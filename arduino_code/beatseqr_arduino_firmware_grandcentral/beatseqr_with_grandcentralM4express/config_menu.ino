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
// Navigation (pattern-select buttons as a d-pad; the tempo knob does NOT
// navigate the menu):
//   PAT3 (down) / PAT2 (up) — scroll items / increment-decrement values
//   PAT1 (back)             — back one level (cancel edit → cancel confirm → exit)
//   PAT4 (select)           — select / confirm
//   knob_mode 1-tap         — back one level (same as PAT1)
//   knob_mode 2-tap         — exit menu entirely
//   param_rec               — select / confirm (same as PAT4)
//
// The swing knob does NOT navigate the menu — it is a configurable play control
// (see swing_knob_function / knob_routine.ino). Swing amount is still editable
// via the "Swing:" menu item.

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
#define CONFIG_ITEM_VOICE_PROB    18
#define CONFIG_ITEM_TAKEOVER      19
#define CONFIG_ITEM_SWING_KNOB    20
#define CONFIG_ITEM_FEATURES      21
#define CONFIG_MENU_ITEM_COUNT    22

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
  "Swing:        ",
  "Voice prob    ",
  "Takeover:     ",
  "Swing knob:   ",
  "Features      "
};

// 5-char display name for a slider_takeover value.
static const char* takeover_name(uint8_t t) {
  switch (t) {
    case 1:  return "Jump ";
    case 2:  return "Reltv";
    default: return "Catch";
  }
}

// 5-char display name for a swing_knob_function value.
static const char* swing_fn_name(uint8_t f) {
  switch (f) {
    case 1:  return "Tempo";
    case 2:  return "Patt ";
    case 3:  return "Voice";
    case 4:  return "Note ";
    default: return "Swing";
  }
}

// ---------------------------------------------------------------------------
// Feature flags (Features submenu)
// ---------------------------------------------------------------------------

#define FEATURE_COUNT 12

// 14-char display names; order matches _feature_flag_ptrs[] and the disable
// side-effect switch in _apply_feature_disable().
static const char* _feature_names[FEATURE_COUNT] = {
  "Advanced mode ",
  "CC mode       ",
  "Probability   ",
  "Gate sliders  ",
  "Velocity      ",
  "Note scales   ",
  "Pat direction ",
  "Pat length    ",
  "Ext clock     ",
  "Oct/note shift",
  "Diagnostics   ",
  "Voice sliders "
};

static bool* _feature_flag_ptrs[FEATURE_COUNT] = {
  &ft_advanced_mode,
  &ft_cc_mode,
  &ft_probability,
  &ft_gate_mode,
  &ft_velocity_mode,
  &ft_scale_quantization,
  &ft_pattern_direction,
  &ft_variable_pat_length,
  &ft_external_clock,
  &ft_octave_note_shift,
  &ft_diagnostics,
  &ft_voice_sliders
};

// Is a slider mode (1=NN..5=PR) currently enabled? NN is always on.
bool slider_mode_enabled(uint8_t m) {
  switch (m) {
    case 2: return ft_velocity_mode;
    case 3: return ft_gate_mode;
    case 4: return ft_cc_mode;
    case 5: return ft_probability;
    default: return true;   // NN (1) and anything else
  }
}

// Next enabled slider mode after `cur`, wrapping 1..slider_mode_total.
// Always terminates because NN (1) is always enabled.
uint8_t next_slider_mode(uint8_t cur) {
  for (int n = 0; n < slider_mode_total; n++) {
    cur = (cur % slider_mode_total) + 1;
    if (slider_mode_enabled(cur)) return cur;
  }
  return 1;
}

// Is a config menu item currently visible (its feature flag is on)?
// Items without a feature flag are always visible.
static bool config_item_enabled(uint8_t item) {
  switch (item) {
    case CONFIG_ITEM_MODE:         return ft_advanced_mode;
    case CONFIG_ITEM_CLOCK:        return ft_external_clock;
    case CONFIG_ITEM_DIAGNOSTICS:  return ft_diagnostics;
    case CONFIG_ITEM_OCTAVE_SHIFT: return ft_octave_note_shift;
    case CONFIG_ITEM_NOTE_SHIFT:   return ft_octave_note_shift;
    case CONFIG_ITEM_NOTE_SCALES:  return ft_scale_quantization;
    case CONFIG_ITEM_PAT_DIR:      return ft_pattern_direction;
    case CONFIG_ITEM_PAT_LENGTH:   return ft_variable_pat_length;
    case CONFIG_ITEM_CC_NUMBER:    return ft_cc_mode;
    case CONFIG_ITEM_VOICE_PROB:   return ft_probability;
    default:                       return true;
  }
}

// Step the menu cursor by dir (+1 next / -1 prev), skipping disabled items.
static uint8_t config_menu_step(uint8_t item, int dir) {
  for (int n = 0; n < CONFIG_MENU_ITEM_COUNT; n++) {
    item = (uint8_t)((item + (dir > 0 ? 1 : CONFIG_MENU_ITEM_COUNT - 1)) % CONFIG_MENU_ITEM_COUNT);
    if (config_item_enabled(item)) return item;
  }
  return item;
}

void draw_features_submenu() {
  lcd.print("?x00?y0");
  lcd.print("> ");
  lcd.print(_feature_names[config_feature_item]);
  lcd.print("?x00?y1");
  lcd.print(*_feature_flag_ptrs[config_feature_item] ? "  active        "
                                                     : "  inactive      ");
}

// Apply side-effects when a feature is toggled off. Indices match
// _feature_names[] / _feature_flag_ptrs[] order.
static void _apply_feature_disable(uint8_t idx) {
  switch (idx) {
    case 0:  // advanced mode — drop back to simple, reset nav/copy state
      if (advanced_mode) {
        advanced_mode           = false;
        adv_pat_nav_active      = false;
        adv_copy_waiting_source = false;
        adv_copy_armed          = false;
        adv_chain_hold_step     = -1;
        read_step_memory(current_voice, pattern_value);
        go_to_pattern(current_pattern, 1);
      }
      break;
    case 1:  if (slider_mode == 4) set_slider_mode(1); break;  // CC mode
    case 2:  if (slider_mode == 5) set_slider_mode(1); break;  // Probability
    case 3:  if (slider_mode == 3) set_slider_mode(1); break;  // Gate sliders
    case 4:  if (slider_mode == 2) set_slider_mode(1); break;  // Velocity
    case 8:  if (external_clock_mode) setExternalClockMode(false); break;  // Ext clock
    default: break;
  }
}

// Pattern-button menu navigation — the d-pad that drives the config menu:
//   PAT1 = back/cancel, PAT4 = select/enter, PAT2 = up, PAT3 = down.
// The tempo knob no longer navigates the menu (by user preference). Back and
// select reuse the existing knob_mode_back_flag / param_rec_flag so all
// downstream menu logic is unchanged. Up/down are returned as a jog delta;
// `editing` flips their meaning so it always feels natural — while editing a
// value up = increase / down = decrease, while scrolling a list up = previous /
// down = next item.
//
// Reading the presses here (uniquePress) also consumes them, but the loop gates
// its own pattern-button handling on !config_menu_active, so menu navigation
// never doubles as a pattern switch.
static int config_pattern_nav(bool editing) {
  bool p_back = pattern_select_buttons[0].uniquePress();
  bool p_up   = pattern_select_buttons[1].uniquePress();
  bool p_down = pattern_select_buttons[2].uniquePress();
  bool p_fwd  = pattern_select_buttons[3].uniquePress();

  if (p_back) knob_mode_back_flag = true;
  if (p_fwd)  param_rec_flag      = true;

  int jog = 0;
  if (editing) {
    if (p_up)   jog = +1;   // increase value
    if (p_down) jog = -1;   // decrease value
  } else {
    if (p_up)   jog = -1;   // previous item
    if (p_down) jog = +1;   // next item
  }
  return jog;
}

void run_features_submenu() {
  // Pattern-button d-pad (list scroll: not editing).
  int jog_v = config_pattern_nav(false);

  // knob_mode single-tap: back out to the main config menu.
  if (knob_mode_back_flag) {
    knob_mode_back_flag = false;
    config_features_active = false;
    draw_config_menu();
    return;
  }

  if (jog_v > 0) {
    config_feature_item = (uint8_t)((config_feature_item + 1) % FEATURE_COUNT);
    draw_features_submenu();
  } else if (jog_v < 0) {
    config_feature_item = (uint8_t)((config_feature_item + FEATURE_COUNT - 1) % FEATURE_COUNT);
    draw_features_submenu();
  }

  // param_rec toggles the current flag.
  if (param_rec_flag) {
    param_rec_flag = false;
    bool* flag = _feature_flag_ptrs[config_feature_item];
    *flag = !(*flag);
    if (!(*flag)) _apply_feature_disable(config_feature_item);
    draw_features_submenu();
  }
}

// ---------------------------------------------------------------------------
// Diagnostics submenu (Button test / Slider test / LED test)
// ---------------------------------------------------------------------------

static const char* _diag_labels[DIAG_SUBMENU_ITEM_COUNT] = {
  "Button test   ",
  "Slider test   ",
  "LED test      ",
  "Voice cal     "
};

void draw_diag_submenu() {
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("> ");
  lcd.print(_diag_labels[config_diag_item]);
  lcd.print("?x00?y1");
  lcd.print("  ");
  lcd.print(_diag_labels[(config_diag_item + 1) % DIAG_SUBMENU_ITEM_COUNT]);
}

void run_diag_submenu() {
  int jog_v = config_pattern_nav(false);

  // knob_mode single-tap: leave the diag submenu, back to the main config menu.
  if (knob_mode_back_flag) {
    knob_mode_back_flag = false;
    config_diag_active = false;
    draw_config_menu();
    return;
  }

  if (jog_v > 0) {
    config_diag_item = (uint8_t)((config_diag_item + 1) % DIAG_SUBMENU_ITEM_COUNT);
    draw_diag_submenu();
  } else if (jog_v < 0) {
    config_diag_item = (uint8_t)((config_diag_item + DIAG_SUBMENU_ITEM_COUNT - 1) % DIAG_SUBMENU_ITEM_COUNT);
    draw_diag_submenu();
  }

  // param_rec enters the selected test (diag_mode takes over the main loop).
  if (param_rec_flag) {
    param_rec_flag = false;
    if      (config_diag_item == 0) enter_diag_button_test();
    else if (config_diag_item == 1) enter_diag_slider_test();
    else if (config_diag_item == 2) enter_diag_led_test();
    else                            enter_diag_voice_cal();
  }
}

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
  } else if (item == CONFIG_ITEM_VOICE_PROB) {
    bool non_default = false;
    for (int v = 0; v < VOICE_COUNT; v++) {
      if (voice_probability[pattern_value][v] < 100) { non_default = true; break; }
    }
    lcd.print(non_default ? "Voice prob   *" : "Voice prob    ");
  } else if (item == CONFIG_ITEM_TAKEOVER) {
    snprintf(_buf, sizeof(_buf), "Takeover:%-5s", takeover_name(slider_takeover));
    lcd.print(_buf);
  } else if (item == CONFIG_ITEM_SWING_KNOB) {
    snprintf(_buf, sizeof(_buf), "Swing kb:%-5s", swing_fn_name(swing_knob_function));
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
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_TAKEOVER) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  %s", takeover_name(slider_takeover));
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else if (config_editing_value && config_menu_item == CONFIG_ITEM_SWING_KNOB) {
    char line2[17];
    int len = snprintf(line2, sizeof(line2), "  %s", swing_fn_name(swing_knob_function));
    while (len < 16) line2[len++] = ' ';
    line2[16] = '\0';
    lcd.print(line2);
  } else {
    uint8_t next = config_menu_step(config_menu_item, +1);
    lcd.print("  ");
    print_config_label(next);
  }
}

void enter_config_menu() {
  config_menu_active      = true;
  config_confirm_pending  = false;
  config_editing_value    = false;
  config_features_active  = false;
  config_diag_active      = false;
  knob_mode_back_flag     = false;
  // Anchor jog baselines so initial knob position isn't misread as movement.
  enter_knob_jog_mode();
  lcd.print("?f");
  draw_config_menu();
}

void exit_config_menu() {
  config_menu_active      = false;
  config_confirm_pending  = false;
  config_editing_value    = false;
  config_features_active  = false;
  config_diag_active      = false;
  config_note_range_phase = 0;
  config_scale_phase      = 0;
  knob_mode_back_flag     = false;
  // Re-anchor the normal-play swing jog so menu-session knob movement isn't
  // misread as a play action on the first loop after exit.
  anchor_swing_norm_jog();
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

  // Modal submenus: while a submenu is open it owns all knob/param input. They
  // read their own jog events, so return before we consume them here. (The diag
  // submenu only runs here when NOT inside a test — diag_mode gates the loop.)
  if (config_features_active) {
    run_features_submenu();
    return;
  }
  if (config_diag_active) {
    run_diag_submenu();
    return;
  }

  // Pattern-button d-pad nav. jog_v: +1 = down/"more", -1 = up/"less". The tempo
  // and swing knobs do NOT navigate the menu; back/exit is PAT1 or knob_mode.
  // While editing a value, up/down adjust it; while scrolling, they move between
  // items (see config_pattern_nav).
  int jog_v = config_pattern_nav(config_editing_value);

  // knob_mode single-tap (back one level): exit editing → exit confirmation →
  // exit menu. Replaces the former swing-knob-CCW cancel gesture.
  if (knob_mode_back_flag) {
    knob_mode_back_flag = false;
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

  // Value editing sub-state: PAT2/PAT3 (up/down) adjust the value; param_rec/PAT4 exits.
  if (config_editing_value) {
    // jog_v > 0 = down/PAT3 = increment; jog_v < 0 = up/PAT2 = decrement.
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
      } else if (config_menu_item == CONFIG_ITEM_TAKEOVER) {
        if (jog_v > 0) slider_takeover = (slider_takeover + 1) % 3;
        if (jog_v < 0) slider_takeover = (slider_takeover + 3 - 1) % 3;
        // Re-seed slider baselines and pickup state for the new behaviour:
        // Catch arms pickup; Jump/Relative bypass it.
        bool needs_pickup = (slider_takeover == 0);
        for (int v = 0; v < VOICE_COUNT; v++) {
          slider_needs_pickup[v] = needs_pickup;
          slider_last_raw[v]     = voice_sliders[v].getValue();
        }
        draw_config_menu();
      } else if (config_menu_item == CONFIG_ITEM_SWING_KNOB) {
        if (jog_v > 0) swing_knob_function = (uint8_t)((swing_knob_function + 1) % SWING_KNOB_FN_COUNT);
        if (jog_v < 0) swing_knob_function = (uint8_t)((swing_knob_function + SWING_KNOB_FN_COUNT - 1) % SWING_KNOB_FN_COUNT);
        draw_config_menu();
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

  // Menu scroll: PAT3 (down) = next item, PAT2 (up) = previous item.
  // Items whose feature flag is off are skipped.
  if (jog_v > 0) {
    config_menu_item = config_menu_step(config_menu_item, +1);
    draw_config_menu();
  } else if (jog_v < 0) {
    config_menu_item = config_menu_step(config_menu_item, -1);
    draw_config_menu();
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
        config_diag_active = true;
        config_diag_item   = 0;
        draw_diag_submenu();
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
      case CONFIG_ITEM_TAKEOVER:
      case CONFIG_ITEM_SWING_KNOB:
        config_editing_value    = true;
        config_note_range_phase = 0;
        config_scale_phase      = 0;
        draw_config_menu();
        break;
      case CONFIG_ITEM_VOICE_PROB:
        exit_config_menu();
        set_slider_mode(5);
        break;
      case CONFIG_ITEM_FEATURES:
        config_features_active = true;
        config_feature_item    = 0;
        draw_features_submenu();
        break;
    }
  }
}
