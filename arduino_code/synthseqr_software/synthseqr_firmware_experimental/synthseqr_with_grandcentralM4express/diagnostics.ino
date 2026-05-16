
bool diag_mode = false;
uint8_t diag_submode = 0;  // 0=input test, 1=LED test

// File-scope state so it survives multiple sessions and can be reset on entry.
static bool          diag_showing_idle       = true;
static unsigned long diag_last_activity_ms   = 0;
static int           diag_last_raw[16]       = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static unsigned long diag_last_slider_lcd_ms = 0;
static bool          diag_enter_tap_pending  = false;
static unsigned long diag_enter_tap_ms       = 0;

// LED test state.
static uint8_t       diag_led_idx      = 0;
static unsigned long diag_led_last_ms  = 0;
#define DIAG_LED_COUNT 22

// Save-file viewer sub-mode (triggered by PAT1 press in diagnostics).
static bool    diag_savefile_mode = false;
static uint8_t diag_savefile_idx  = 0;
static char    diag_sf_l1[SD_DIAG_FIELD_COUNT][17];
static char    diag_sf_l2[SD_DIAG_FIELD_COUNT][17];

// Pin lookup tables — must match config.h declarations.
static const uint8_t DIAG_STEP_PINS[16]   = {23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53};
static const uint8_t DIAG_PAT_PINS[4]     = {15,14,6,8};
#if PCB_VERSION == 1
static const uint8_t DIAG_SLIDER_PINS[16]   = {A15,A14,A13,A12,A11,A10,A9,A8,A7,A6,A5,A4,A2,A3,A1,A0};
static const char*   DIAG_SLIDER_PNAMES[16] = {
  "A15","A14","A13","A12","A11","A10","A9","A8",
  "A7","A6","A5","A4","A2","A3","A1","A0"
};
#else
static const uint8_t DIAG_SLIDER_PINS[16]   = {A15,A14,A13,A12,A11,A10,A9,A8,A7,A6,A5,A4,A3,A2,A1,A0};
static const char*   DIAG_SLIDER_PNAMES[16] = {
  "A15","A14","A13","A12","A11","A10","A9","A8",
  "A7","A6","A5","A4","A3","A2","A1","A0"
};
#endif

// ---------------------------------------------------------------------------
// LCD helpers — write directly, bypassing run_LCD_update().
// run_LCD_update() returns early when diag_mode is true (checked in LCD.ino).
// ---------------------------------------------------------------------------

void diag_show_idle_screen()
{
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("  INPUT TEST    ");
  lcd.print("?x00?y1");
  lcd.print("Lft=back PAT1:sf");
}

// Save-file viewer: display the currently selected field on both LCD lines.
// Line 1 = field name, line 2 = value from the saved JSON.
// A field counter "(N/16)" would not fit, so the user scrolls with d-pad up/down.
void diag_show_savefile_field()
{
  char idx_buf[17];
  // Show field index in brackets on line 1: "[02] advanced_mo"
  snprintf(idx_buf, sizeof(idx_buf), "[%02d] %-11s", diag_savefile_idx, diag_sf_l1[diag_savefile_idx]);
  lcd.print("?x00?y0");
  lcd.print(idx_buf);
  lcd.print("?x00?y1");
  lcd.print(diag_sf_l2[diag_savefile_idx]);
}

// Line 1: "%-8s pin:%3d"  → 16 chars
// e.g. "STEP01   pin: 23"  "DPAD-UP  pin: 19"  "PLAY     pin: 21"
void diag_write_button(const char* name, int pin)
{
  char buf[17];
  snprintf(buf, sizeof(buf), "%-8s pin:%3d", name, pin);
  lcd.print("?x00?y0");
  lcd.print(buf);
  // Clear line 2 so stale slider info doesn't confuse.
  lcd.print("?x00?y1");
  lcd.print("                ");
  Serial.print("DIAG BTN: ");
  Serial.println(buf);
}

// Line 2: "SL%02d %-3s r:%4d "  → 16 chars
// e.g. "SL00 A15 r:4095 "  "SL09 A6  r:2048 "
void diag_write_slider(uint8_t idx, const char* pin_name, int raw_val)
{
  char buf[17];
  snprintf(buf, sizeof(buf), "SL%02d %-3s r:%4d ", idx, pin_name, raw_val);
  lcd.print("?x00?y1");
  lcd.print(buf);
  Serial.print("DIAG SL:  ");
  Serial.println(buf);
}

// ---------------------------------------------------------------------------
// Entry / exit
// ---------------------------------------------------------------------------

// Called from the diag submenu to enter the input test mode.
void enter_diag_input_test()
{
  diag_submode = 0;
  diag_mode    = true;
  diag_enter_tap_pending = false;
  diag_savefile_mode     = false;

  for (int i = 0; i < 16; i++) diag_last_raw[i] = -1;
  diag_last_activity_ms   = millis();
  diag_last_slider_lcd_ms = 0;
  diag_showing_idle       = false;

  Serial.println("entering input test");
  diag_show_idle_screen();
  diag_showing_idle = true;
}

// Called from the diag submenu to enter the LED test mode.
void enter_diag_led_test()
{
  diag_submode    = 1;
  diag_mode       = true;
  diag_led_idx    = 0;
  diag_led_last_ms = 0;

  for (int i = 0; i < 16; i++) step_leds[i].off();
  for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
  playbutton_LED.off();
  enterbutton_LED.off();

  Serial.println("entering LED test");
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("LED test        ");
  lcd.print("?x00?y1");
  lcd.print("Left=exit       ");
}

void run_diagnostics()
{
  if (!diag_mode) return;

  if (diag_submode == 1) {
    run_diag_led_test();
  } else {
    // Run display FIRST so uniquePress() gets the CHANGED flag before any
    // subsequent isPressed() call starts a debounce window that would clear
    // CHANGED and cause uniquePress() to miss the press.
    run_diagnostics_display();
  }
}

// ---------------------------------------------------------------------------
// LED test — non-blocking sequential chase, Left exits back to diag submenu.
// ---------------------------------------------------------------------------

void run_diag_led_test()
{
  if (dpad_left.uniquePress()) {
    for (int i = 0; i < 16; i++) step_leds[i].off();
    for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
    playbutton_LED.off();
    enterbutton_LED.off();
    diag_mode = false;
    read_step_memory(0, pattern_value);
    go_to_pattern(current_pattern, 1);
    draw_diag_submenu();
    return;
  }

  unsigned long now = millis();
  if (now - diag_led_last_ms < 80) return;
  diag_led_last_ms = now;

  // Turn off all LEDs, then light the current one.
  for (int i = 0; i < 16; i++) step_leds[i].off();
  for (int i = 0; i < 4;  i++) pattern_select_leds[i].off();
  playbutton_LED.off();
  enterbutton_LED.off();

  if      (diag_led_idx < 16) step_leds[diag_led_idx].on();
  else if (diag_led_idx < 20) pattern_select_leds[diag_led_idx - 16].on();
  else if (diag_led_idx == 20) playbutton_LED.on();
  else                         enterbutton_LED.on();

  diag_led_idx = (uint8_t)((diag_led_idx + 1) % DIAG_LED_COUNT);
}

// ---------------------------------------------------------------------------
// Per-loop polling — called only when diag_mode is true.
// ---------------------------------------------------------------------------

void run_diagnostics_display()
{
  unsigned long now_ms = millis();
  bool activity = false;

  // --- Save-file viewer sub-mode ---
  // D-pad up/down scrolls through fields; any other button exits.
  if (diag_savefile_mode) {
    if (dpad_up.uniquePress()) {
      if (diag_savefile_idx > 0) diag_savefile_idx--;
      diag_show_savefile_field();
    } else if (dpad_down.uniquePress()) {
      if (diag_savefile_idx < SD_DIAG_FIELD_COUNT - 1) diag_savefile_idx++;
      diag_show_savefile_field();
    } else {
      // Any other button press exits back to idle.
      bool exit_sf = false;
      if (enterbutton.uniquePress() || dpad_left.uniquePress()) {
        exit_sf = true;
      }
      if (play_button_isr_fired) { play_button_isr_fired = false; exit_sf = true; }
      if (!exit_sf) {
        for (int i = 0; i < 16; i++) { if (step_buttons[i].uniquePress()) { exit_sf = true; break; } }
      }
      if (!exit_sf) {
        for (int i = 0; i < 4; i++) { if (pattern_select_buttons[i].uniquePress()) { exit_sf = true; break; } }
      }
      if (exit_sf) {
        diag_savefile_mode = false;
        diag_last_activity_ms = millis();
        diag_showing_idle = false;
        diag_show_idle_screen();
        diag_showing_idle = true;
      }
    }
    return;
  }

  // --- D-pad ---
  if (dpad_up.uniquePress())    { diag_write_button("DPAD-UP",  19); activity = true; }
  if (dpad_down.uniquePress())  { diag_write_button("DPAD-DN",  18); activity = true; }
  // pin 16 is now TRS MIDI TX — no dpad_right button
  // Left exits input test, returning to the diag submenu.
  if (dpad_left.uniquePress()) {
    diag_mode          = false;
    diag_savefile_mode = false;
    Serial.println("exiting input test");
    read_step_memory(0, pattern_value);
    go_to_pattern(current_pattern, 1);
    draw_diag_submenu();
    return;
  }

  // --- Enter: single tap shows button, double-tap exits diagnostics ---
  if (enterbutton.uniquePress()) {
    unsigned long now_ms = millis();
    if (diag_enter_tap_pending && now_ms - diag_enter_tap_ms <= 400) {
      // Double-tap confirmed — exit input test, return to diag submenu.
      diag_enter_tap_pending = false;
      diag_mode          = false;
      diag_savefile_mode = false;
      Serial.println("exiting input test");
      read_step_memory(0, pattern_value);
      go_to_pattern(current_pattern, 1);
      draw_diag_submenu();
      return;
    } else {
      diag_enter_tap_pending = true;
      diag_enter_tap_ms = now_ms;
      diag_write_button("ENTER", 20);
      activity = true;
    }
  }
  // Clear pending tap if window expired without a second press.
  if (diag_enter_tap_pending && millis() - diag_enter_tap_ms > 400) {
    diag_enter_tap_pending = false;
  }

  // --- Play (interrupt-driven) ---
  if (play_button_isr_fired)
  {
    play_button_isr_fired = false;
    diag_write_button("PLAY", 21);
    activity = true;
  }

  // --- Step buttons (first pressed wins this frame) ---
  if (!activity)
  {
    for (int i = 0; i < 16; i++)
    {
      if (step_buttons[i].uniquePress())
      {
        step_leds[i].toggle();
        char name[9];
        snprintf(name, sizeof(name), "STEP%02d", i + 1);
        diag_write_button(name, DIAG_STEP_PINS[i]);
        activity = true;
        break;
      }
    }
  }

  // --- Pattern select buttons (first pressed wins this frame) ---
  // PAT1 enters the save-file viewer; PAT2-4 do the normal button test.
  if (!activity)
  {
    for (int i = 0; i < 4; i++)
    {
      if (pattern_select_buttons[i].uniquePress())
      {
        if (i == 0) {
          // Enter save-file viewer: load all fields from SD, show first.
          diag_savefile_idx = 0;
          sd_diag_load_fields(diag_sf_l1, diag_sf_l2);
          diag_savefile_mode = true;
          diag_show_savefile_field();
        } else {
          pattern_select_leds[i].toggle();
          char name[5];
          snprintf(name, sizeof(name), "PAT%d", i + 1);
          diag_write_button(name, DIAG_PAT_PINS[i]);
        }
        activity = true;
        break;
      }
    }
  }

  // --- Voice sliders: raw analogRead, change threshold ±16 ADC counts ---
  // Rate-limited to once per 100 ms to avoid flooding the LCD serial buffer.
  if (!activity && (now_ms - diag_last_slider_lcd_ms >= 100))
  {
    for (int j = 0; j < 16; j++)
    {
      int raw = analogRead(DIAG_SLIDER_PINS[j]);
      if (diag_last_raw[j] < 0 || abs(raw - diag_last_raw[j]) > 16)
      {
        diag_last_raw[j]        = raw;
        diag_last_slider_lcd_ms = now_ms;
        diag_write_slider(j, DIAG_SLIDER_PNAMES[j], raw);
        activity = true;
        break;  // one slider update per frame
      }
    }
  }

  // --- Auto-clear after 2 s of inactivity ---
  if (activity)
  {
    diag_last_activity_ms = now_ms;
    diag_showing_idle     = false;
  }
  else if (!diag_showing_idle && (now_ms - diag_last_activity_ms >= 2000))
  {
    diag_show_idle_screen();
    diag_showing_idle = true;
  }
}

