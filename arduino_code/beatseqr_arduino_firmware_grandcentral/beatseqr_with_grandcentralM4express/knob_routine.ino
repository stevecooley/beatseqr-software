// knob_routine.ino
//
// Reads the two hardware potentiometers:
//   Tempo knob — A8 — maps to TEMPO (lower_BPM_number … upper_BPM_number)
//   Swing knob — A9 — maps to SWING (0 … 5)
//
// In normal mode the knobs directly set TEMPO and SWING each loop.
// In config menu mode the knobs become jog-wheel navigators:
//   Tempo knob → up / down (scroll menu items, increment values)
//   Swing knob → left / right (exit or decrease values)
// Jog events fire when accumulated movement exceeds KNOB_JOG_THRESHOLD ADC
// counts. The saved position is updated after each event so the user can keep
// spinning in one direction to get multiple increments.

#define KNOB_JOG_THRESHOLD 80   // ADC counts (12-bit) needed to trigger one step

// Saved tempo-knob position at the moment the config menu was entered — the
// baseline for delta tracking during menu navigation. (The swing knob no longer
// navigates the menu, so only the tempo jog is anchored here.)
static int _jog_tempo_last = 512;

// Call once when entering config menu to anchor the tempo-jog baseline.
void enter_knob_jog_mode() {
  _jog_tempo_last = analogRead(A8);
}

// Returns +1 (clockwise / down / increment), -1 (counter-clockwise / up /
// decrement), or 0 (no movement) based on tempo knob movement since last call.
int knob_jog_vertical() {
  int cur = analogRead(A8);
  int delta = cur - _jog_tempo_last;
  if (delta > KNOB_JOG_THRESHOLD) {
    _jog_tempo_last = cur;
    return 1;
  } else if (delta < -KNOB_JOG_THRESHOLD) {
    _jog_tempo_last = cur;
    return -1;
  }
  return 0;
}

// Normal-play swing jog wheel. Separate static baseline from the menu jog
// (_jog_swing_last) because both read A9 every loop in their own context.
static int _swing_norm_last = 512;

// Seed the normal-play baseline to the current A9 position. Called at startup
// and on config-menu exit so menu-session movement doesn't leak into play.
void anchor_swing_norm_jog() {
  _swing_norm_last = analogRead(A9);
}

// Returns +1 (CW / increment), -1 (CCW / decrement), or 0 (no movement) for the
// swing knob during normal play. Updates the baseline on each event so the user
// can keep spinning for multiple steps.
static int knob_jog_swing_norm() {
  int cur = analogRead(A9);
  int delta = cur - _swing_norm_last;
  if (delta > KNOB_JOG_THRESHOLD) {
    _swing_norm_last = cur;
    return 1;
  } else if (delta < -KNOB_JOG_THRESHOLD) {
    _swing_norm_last = cur;
    return -1;
  }
  return 0;
}

// run_knob_routine — called every loop().
//
// In the config menu the knobs are jog wheels invoked on-demand by
// run_config_menu(). During normal play the tempo knob (A8) has no direct
// effect (ADC noise caused clock instability), while the swing knob (A9) acts
// as a configurable relative jog wheel — see swing_knob_function.
void run_knob_routine() {
  if (config_menu_active) {
    // Jog functions are called on-demand by run_config_menu().
    return;
  }

  // Swing knob → configurable jog action. CW = +1, CCW = -1.
  int dir = knob_jog_swing_norm();
  if (dir == 0) return;

  switch (swing_knob_function) {
    case 1:  // Tempo nudge ±1 BPM
      if (dir > 0 && TEMPO < (float)upper_BPM_number) {
        TEMPO += 1.0f;
        seq.setTempo(TEMPO);
        setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
        update_line1 = true;
      } else if (dir < 0 && TEMPO > (float)lower_BPM_number) {
        TEMPO -= 1.0f;
        seq.setTempo(TEMPO);
        setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
        update_line1 = true;
      }
      break;
    case 2:  // Pattern next / previous
      go_to_pattern((current_pattern + (dir > 0 ? 1 : 15)) % 16, 1);
      break;
    case 3:  // Voice next / previous
      set_current_voice((uint8_t)((current_voice + (dir > 0 ? 1 : VOICE_COUNT - 1)) % VOICE_COUNT));
      break;
    case 4:  // Note — selects "adjust selected voice's note" mode, but the
             // adjustment value comes from voice 1's slider, not this (bad)
             // knob. See run_note_slider_override() in voice_slider_routine.ino.
      break;
    default: // 0 = Swing amount 0–5
      if (dir > 0 && SWING < 5) { SWING++; update_line1 = true; }
      else if (dir < 0 && SWING > 0) { SWING--; update_line1 = true; }
      break;
  }
}
