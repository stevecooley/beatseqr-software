
bool diag_mode = false;

// File-scope state so it survives multiple sessions and can be reset on entry.
static bool          diag_showing_idle       = true;
static unsigned long diag_last_activity_ms   = 0;
static int           diag_last_raw[16]       = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static unsigned long diag_last_slider_lcd_ms = 0;

// Pin lookup tables — must match config.h declarations.
static const uint8_t DIAG_STEP_PINS[16]   = {23,25,27,29,31,33,35,37,39,41,43,45,47,49,51,53};
static const uint8_t DIAG_PAT_PINS[4]     = {15,14,6,8};
static const uint8_t DIAG_SLIDER_PINS[16] = {A15,A14,A13,A12,A11,A10,A9,A8,A7,A6,A5,A4,A2,A3,A1,A0};
static const char*   DIAG_SLIDER_PNAMES[16] = {
  "A15","A14","A13","A12","A11","A10","A9","A8",
  "A7","A6","A5","A4","A2","A3","A1","A0"
};

// ---------------------------------------------------------------------------
// LCD helpers — write directly, bypassing run_LCD_update().
// run_LCD_update() returns early when diag_mode is true (checked in LCD.ino).
// ---------------------------------------------------------------------------

void diag_show_idle_screen()
{
  lcd.print("?f");
  lcd.print("?x00?y0");
  lcd.print("  DIAGNOSTICS   ");
  lcd.print("?x00?y1");
  lcd.print(" press a button ");
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

void run_diagnostics()
{
  // Entry / exit: hold d-pad left + right simultaneously for 1 second.
  if (dpad_left.heldFor(1000) && dpad_right.heldFor(1000))
  {
    if (!diag_mode)
    {
      // Wait for release so the exit check doesn't fire immediately on entry.
      while (dpad_left.isPressed() || dpad_right.isPressed()) {}

      diag_mode = true;

      // Reset per-session state.
      for (int i = 0; i < 16; i++) diag_last_raw[i] = -1;
      diag_last_activity_ms   = millis();
      diag_last_slider_lcd_ms = 0;
      diag_showing_idle       = false;

      Serial.println("entering diagnostics");

      // Entry splash — brief blocking delay is acceptable here.
      lcd.print("?f");
      lcd.print("?x00?y0");
      lcd.print("  DIAGNOSTICS   ");
      lcd.print("?x00?y1");
      lcd.print("hold L+R to exit");
      delay(1500);

      diag_show_idle_screen();
      diag_showing_idle = true;
    }
    else
    {
      // Exit.
      while (dpad_left.isPressed() || dpad_right.isPressed()) {}

      diag_mode = false;
      Serial.println("exiting diagnostics");

      // Restore step LEDs and pattern LEDs to match current sequencer state.
      read_step_memory(0, pattern_value);
      go_to_pattern(current_pattern, 1);

      // Hand LCD back to the normal display system.
      lcd.print("?f");
      update_line1 = true;
      update_line2 = true;
      lcdflag      = 255;
      next_lcdflag = 255;
    }
    return;
  }

  if (diag_mode)
  {
    run_diagnostics_display();
  }
}

// ---------------------------------------------------------------------------
// Per-loop polling — called only when diag_mode is true.
// ---------------------------------------------------------------------------

void run_diagnostics_display()
{
  unsigned long now_ms = millis();
  bool activity = false;

  // --- D-pad ---
  if (dpad_up.uniquePress())    { diag_write_button("DPAD-UP",  19); activity = true; }
  if (dpad_down.uniquePress())  { diag_write_button("DPAD-DN",  18); activity = true; }
  if (dpad_left.uniquePress())  { diag_write_button("DPAD-LT",  17); activity = true; }
  if (dpad_right.uniquePress()) { diag_write_button("DPAD-RT",  16); activity = true; }

  // --- Enter ---
  if (enterbutton.uniquePress()) { diag_write_button("ENTER",    20); activity = true; }

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
  if (!activity)
  {
    for (int i = 0; i < 4; i++)
    {
      if (pattern_select_buttons[i].uniquePress())
      {
        pattern_select_leds[i].toggle();
        char name[5];
        snprintf(name, sizeof(name), "PAT%d", i + 1);
        diag_write_button(name, DIAG_PAT_PINS[i]);
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
