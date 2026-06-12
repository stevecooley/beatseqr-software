// diagnostics.ino — hardware self-test for Beatseqr.
//
// Entered from the config menu: Diagnostics → opens a small submenu with three
// modes (Button test / Slider test / LED test). The save-file viewer is reached
// from the button test by pressing pattern button 0 (PAT1).
//
// Control scheme (Beatseqr has no D-pad):
//   - Inside any test, double-tap param_rec (Enter) to exit back to the submenu.
//   - In the button test, a single param_rec tap is reported like any button.
//     The button test also reports the two knobs (tempo/swing) and the
//     voice-select resistor ladder (raw A10 + detected voice).
//   - In the slider test, press a voice-select button (1–8) to choose which
//     slider to watch; only that one slider is shown. This keeps a noisy slider
//     from drowning out the one you want to see (all sliders are no longer
//     reporting at once).
//   - In the save-file viewer, the tempo knob scrolls fields; param_rec
//     double-tap exits back to the button-test idle screen.
//
// While diag_mode is true, loop() runs run_diagnostics() then returns early, and
// run_LCD_update() early-returns (LCD.ino), so diagnostics owns the display.

bool    diag_mode    = false;
uint8_t diag_submode = 0;   // 0 = button test, 1 = slider test, 2 = LED test

// Button-test state.
static bool          diag_showing_idle       = true;
static unsigned long diag_last_activity_ms   = 0;
static int           diag_last_raw[10]       = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static unsigned long diag_last_slider_lcd_ms = 0;
static bool          diag_enter_tap_pending  = false;
static unsigned long diag_enter_tap_ms       = 0;

// Slider-test state — watch one slider at a time, chosen by voice-select button.
static uint8_t       diag_slider_focus  = 0;   // which slider (voice) 0–7
static unsigned long diag_slider_lcd_ms = 0;
static int           diag_vs_buf[4]     = {0,0,0,0};   // A10 consensus buffer
static int           diag_vs_buf_idx    = 0;
static int           diag_vs_prev_voice = -1;

// LED test state.
static uint8_t       diag_led_idx     = 0;
static unsigned long diag_led_last_ms = 0;
#define DIAG_LED_COUNT 29   // 16 step + 4 pattern + 8 voice + 1 play

// Save-file viewer state.
static bool    diag_savefile_mode = false;
static uint8_t diag_savefile_idx  = 0;
static char    diag_sf_l1[SD_DIAG_FIELD_COUNT][17];
static char    diag_sf_l2[SD_DIAG_FIELD_COUNT][17];

// Pin lookup tables — must match config.h declarations.
static const uint8_t DIAG_STEP_PINS[16] = {23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53};
static const uint8_t DIAG_PAT_PINS[4]   = {21,20,19,18};
// 8 sliders A7..A0, then the two knobs (tempo A8, swing A9).
static const uint8_t DIAG_ANALOG_PINS[10]   = {A7,A6,A5,A4,A3,A2,A1,A0,A8,A9};
static const char*   DIAG_ANALOG_NAMES[10]  = {"A7","A6","A5","A4","A3","A2","A1","A0","A8","A9"};
#define DIAG_ANALOG_COUNT 10

// Classify a raw A10 reading into a voice index (0–7) or -1 (none).
// (voice_select_routine.ino's classifier is file-static, so we keep a local one.)
static int diag_voice_from_adc(int adc_val) {
  for (int i = 0; i < VOICE_COUNT; i++) {
    if (adc_val >= vselectval_lowerranges[i] && adc_val <= vselectval_upperranges[i]) return i;
  }
  return -1;
}

// Rising-edge voice-select detection for the slider test (local consensus copy
// of voice_select_routine's logic). Returns a newly-pressed voice 0–7, else -1.
static int diag_vselect_press() {
  diag_vs_buf[diag_vs_buf_idx] = analogRead(A10);
  diag_vs_buf_idx = (diag_vs_buf_idx + 1) % 4;

  int  first     = diag_voice_from_adc(diag_vs_buf[0]);
  bool unanimous = true;
  for (int i = 1; i < 4; i++) {
    if (diag_voice_from_adc(diag_vs_buf[i]) != first) { unanimous = false; break; }
  }
  int detected = unanimous ? first : diag_vs_prev_voice;

  int pressed = -1;
  if (detected != diag_vs_prev_voice) {
    diag_vs_prev_voice = detected;
    if (detected != -1) pressed = detected;   // rising edge of a new press
  }
  return pressed;
}

// ---------------------------------------------------------------------------
// LCD helpers — write directly, bypassing run_LCD_update().
// ---------------------------------------------------------------------------

void diag_show_idle_screen() {
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("  BUTTON TEST   ");
  lcd.print("?x00?y1");
  lcd.print("2xEnt=bk PAT1=sf");
}

// Slider-test header (line 1): which voice/slider and its analog pin.
static void diag_show_slider_header() {
  char l1[17];
  snprintf(l1, sizeof(l1), "SLIDER V%d  %-3s ", diag_slider_focus + 1,
           DIAG_ANALOG_NAMES[diag_slider_focus]);
  lcd.print("?x00?y0");
  lcd.print(l1);
}

void diag_show_savefile_field() {
  char idx_buf[17];
  snprintf(idx_buf, sizeof(idx_buf), "[%02d] %-11s", diag_savefile_idx, diag_sf_l1[diag_savefile_idx]);
  lcd.print("?x00?y0");
  lcd.print(idx_buf);
  lcd.print("?x00?y1");
  lcd.print(diag_sf_l2[diag_savefile_idx]);
}

void diag_write_button(const char* name, int pin) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-8s pin:%3d", name, pin);
  lcd.print("?x00?y0");
  lcd.print(buf);
  lcd.print("?x00?y1");
  lcd.print("                ");
  Serial.print("DIAG BTN: ");
  Serial.println(buf);
}

// Voice-select ladder readout: raw A10 value + detected voice.
void diag_write_vselect(int raw, int voice) {
  lcd.print("?x00?y0");
  lcd.print("VOICE SELECT    ");
  char l2[17];
  if (voice < 0) snprintf(l2, sizeof(l2), "A10 r:%4d  V:- ", raw);
  else           snprintf(l2, sizeof(l2), "A10 r:%4d  V:%d ", raw, voice + 1);
  lcd.print("?x00?y1");
  lcd.print(l2);
  Serial.print("DIAG VSEL: ");
  Serial.println(l2);
}

// Knob readout for the button test (idx 8 tempo, 9 swing).
void diag_write_knob(uint8_t idx, const char* pin_name, int raw_val) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-5s %-3s r:%4d ", idx == 8 ? "TEMPO" : "SWING", pin_name, raw_val);
  lcd.print("?x00?y1");
  lcd.print(buf);
  Serial.print("DIAG KN:  ");
  Serial.println(buf);
}

// ---------------------------------------------------------------------------
// Entry
// ---------------------------------------------------------------------------

void enter_diag_button_test() {
  diag_submode = 0;
  diag_mode    = true;
  diag_enter_tap_pending = false;
  diag_savefile_mode     = false;
  for (int i = 0; i < DIAG_ANALOG_COUNT; i++) diag_last_raw[i] = -1;
  diag_last_activity_ms   = millis();
  diag_last_slider_lcd_ms = 0;
  enter_knob_jog_mode();      // anchor jog baseline for save-file scrolling
  Serial.println("entering button test");
  diag_show_idle_screen();
  diag_showing_idle = true;
}

void enter_diag_slider_test() {
  diag_submode = 1;
  diag_mode    = true;
  diag_enter_tap_pending = false;
  diag_savefile_mode     = false;
  diag_slider_focus  = (current_voice < VOICE_COUNT) ? current_voice : 0;
  diag_slider_lcd_ms = 0;
  diag_vs_prev_voice = -1;
  for (int i = 0; i < 4; i++) diag_vs_buf[i] = 0;
  // Light only the focused voice LED so the picked slider is obvious.
  for (int i = 0; i < VOICE_COUNT; i++) voice_select_leds[i].off();
  voice_select_leds[diag_slider_focus].on();
  Serial.println("entering slider test");
  lcd.print("?f");
  diag_show_slider_header();
}

void enter_diag_led_test() {
  diag_submode     = 2;
  diag_mode        = true;
  diag_led_idx     = 0;
  diag_led_last_ms = 0;
  for (int i = 0; i < 16; i++) step_leds[i].off();
  for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
  for (int i = 0; i < VOICE_COUNT; i++) voice_select_leds[i].off();
  playbutton_LED.off();
  enter_knob_jog_mode();
  Serial.println("entering LED test");
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("LED test        ");
  lcd.print("?x00?y1");
  lcd.print("2xEnter = exit  ");
}

// Common exit: restore LEDs/state and return to the diag submenu.
static void diag_exit_to_submenu() {
  for (int i = 0; i < 16; i++) step_leds[i].off();
  for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
  playbutton_LED.off();
  diag_mode          = false;
  diag_savefile_mode = false;
  read_step_memory(current_voice, pattern_value);
  go_to_pattern(current_pattern, 1);
  // Re-light the active voice LED (LED/slider test left them off/changed).
  for (int i = 0; i < VOICE_COUNT; i++) voice_select_leds[i].off();
  voice_select_leds[current_voice].on();
  draw_diag_submenu();
}

// Did the user double-tap param_rec this loop? Single taps are reported by the
// caller (button test) via the returned "single" flag.
static bool diag_check_enter_exit(bool report_single, const char* single_name) {
  if (param_rec.uniquePress()) {
    unsigned long now_ms = millis();
    if (diag_enter_tap_pending && now_ms - diag_enter_tap_ms <= 400) {
      diag_enter_tap_pending = false;
      return true;   // double-tap → exit
    }
    diag_enter_tap_pending = true;
    diag_enter_tap_ms = now_ms;
    if (report_single) diag_write_button(single_name, 11);
  }
  if (diag_enter_tap_pending && millis() - diag_enter_tap_ms > 400) {
    diag_enter_tap_pending = false;
  }
  return false;
}

void run_diagnostics() {
  if (!diag_mode) return;
  if      (diag_submode == 2) run_diag_led_test();
  else if (diag_submode == 1) run_diag_slider_test();
  else                        run_diag_button_test();
}

// ---------------------------------------------------------------------------
// LED test — non-blocking chase across step / pattern / voice / play LEDs.
// ---------------------------------------------------------------------------

void run_diag_led_test() {
  if (diag_check_enter_exit(false, NULL)) { diag_exit_to_submenu(); return; }

  unsigned long now = millis();
  if (now - diag_led_last_ms < 80) return;
  diag_led_last_ms = now;

  for (int i = 0; i < 16; i++) step_leds[i].off();
  for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
  for (int i = 0; i < VOICE_COUNT; i++) voice_select_leds[i].off();
  playbutton_LED.off();

  if      (diag_led_idx < 16) step_leds[diag_led_idx].on();
  else if (diag_led_idx < 20) pattern_select_leds[diag_led_idx - 16].on();
  else if (diag_led_idx < 28) voice_select_leds[diag_led_idx - 20].on();
  else                        playbutton_LED.on();

  diag_led_idx = (uint8_t)((diag_led_idx + 1) % DIAG_LED_COUNT);
}

// ---------------------------------------------------------------------------
// Slider test — watch a single slider, chosen by voice-select button.
// ---------------------------------------------------------------------------

void run_diag_slider_test() {
  if (diag_check_enter_exit(false, NULL)) { diag_exit_to_submenu(); return; }

  // Voice-select button picks which slider to watch (1–8 → slider 0–7).
  int pressed = diag_vselect_press();
  if (pressed >= 0 && (uint8_t)pressed != diag_slider_focus) {
    diag_slider_focus = (uint8_t)pressed;
    for (int i = 0; i < VOICE_COUNT; i++) voice_select_leds[i].off();
    voice_select_leds[diag_slider_focus].on();
    diag_show_slider_header();
    diag_slider_lcd_ms = 0;   // force an immediate value redraw
  }

  // Continuously show the focused slider's raw value (rate-limited).
  unsigned long now = millis();
  if (now - diag_slider_lcd_ms >= 100) {
    diag_slider_lcd_ms = now;
    int raw = analogRead(DIAG_ANALOG_PINS[diag_slider_focus]);
    char l2[17];
    snprintf(l2, sizeof(l2), "r:%4d  2xEnt=bk", raw);
    lcd.print("?x00?y1");
    lcd.print(l2);
    Serial.print("DIAG SL");
    Serial.print(diag_slider_focus);
    Serial.print(" r:");
    Serial.println(raw);
  }
}

// ---------------------------------------------------------------------------
// Button test — per-loop polling of all buttons + knobs + voice ladder.
// ---------------------------------------------------------------------------

void run_diag_button_test() {
  unsigned long now_ms = millis();
  bool activity = false;

  // --- Save-file viewer sub-mode ---
  if (diag_savefile_mode) {
    int jog = knob_jog_vertical();   // tempo knob scrolls fields
    if (jog > 0) {
      if (diag_savefile_idx < SD_DIAG_FIELD_COUNT - 1) diag_savefile_idx++;
      diag_show_savefile_field();
    } else if (jog < 0) {
      if (diag_savefile_idx > 0) diag_savefile_idx--;
      diag_show_savefile_field();
    }
    if (diag_check_enter_exit(false, NULL)) { diag_exit_to_submenu(); return; }
    return;
  }

  // --- Exit: double-tap param_rec; single tap reported as a button ---
  if (diag_check_enter_exit(true, "param_rec")) { diag_exit_to_submenu(); return; }
  if (diag_enter_tap_pending) activity = true;   // a single tap was just shown

  // --- Other mode buttons ---
  if (!activity && slider_mode_select.uniquePress()) { diag_write_button("SLMODE", A13); activity = true; }
  if (!activity && voice_mode_select.uniquePress())  { diag_write_button("VCMODE", A12); activity = true; }
  if (!activity && knob_mode_select.uniquePress())   { diag_write_button("KNBMODE", A14); activity = true; }

  // --- Play (interrupt-driven) ---
  if (!activity && play_button_isr_fired) {
    play_button_isr_fired = false;
    diag_write_button("PLAY", A11);
    activity = true;
  }

  // --- Step buttons (first pressed wins this frame) ---
  if (!activity) {
    for (int i = 0; i < 16; i++) {
      if (step_buttons[i].uniquePress()) {
        step_leds[i].toggle();
        char name[9];
        snprintf(name, sizeof(name), "STEP%02d", i + 1);
        diag_write_button(name, DIAG_STEP_PINS[i]);
        activity = true;
        break;
      }
    }
  }

  // --- Pattern buttons. PAT1 (index 0) opens the save-file viewer ---
  if (!activity) {
    for (int i = 0; i < 4; i++) {
      if (pattern_select_buttons[i].uniquePress()) {
        if (i == 0) {
          diag_savefile_idx = 0;
          sd_diag_load_fields(diag_sf_l1, diag_sf_l2);
          diag_savefile_mode = true;
          diag_show_savefile_field();
        } else {
          pattern_select_leds[i].toggle();
          char name[6];
          snprintf(name, sizeof(name), "PAT%d", i + 1);
          diag_write_button(name, DIAG_PAT_PINS[i]);
        }
        activity = true;
        break;
      }
    }
  }

  // --- Voice-select resistor ladder (A10): show raw + detected voice ---
  if (!activity) {
    int raw = analogRead(A10);
    int v   = diag_voice_from_adc(raw);
    if (v >= 0) { diag_write_vselect(raw, v); activity = true; }
  }

  // --- 2 knobs (tempo A8, swing A9): raw analogRead, ±16 threshold, 100 ms rate-limit ---
  if (!activity && (now_ms - diag_last_slider_lcd_ms >= 100)) {
    for (int j = 8; j < DIAG_ANALOG_COUNT; j++) {
      int raw = analogRead(DIAG_ANALOG_PINS[j]);
      if (diag_last_raw[j] < 0 || abs(raw - diag_last_raw[j]) > 16) {
        diag_last_raw[j]        = raw;
        diag_last_slider_lcd_ms = now_ms;
        diag_write_knob(j, DIAG_ANALOG_NAMES[j], raw);
        activity = true;
        break;
      }
    }
  }

  // --- Auto-clear to idle after 2 s of inactivity ---
  if (activity) {
    diag_last_activity_ms = now_ms;
    diag_showing_idle     = false;
  } else if (!diag_showing_idle && (now_ms - diag_last_activity_ms >= 2000)) {
    diag_show_idle_screen();
    diag_showing_idle = true;
  }
}
