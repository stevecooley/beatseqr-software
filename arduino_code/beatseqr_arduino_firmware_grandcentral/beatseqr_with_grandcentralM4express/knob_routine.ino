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

// Saved knob positions at the moment the config menu was entered.
// These are the baseline for delta tracking during menu navigation.
static int _jog_tempo_last = 512;
static int _jog_swing_last = 512;

// Call once when entering config menu to anchor the jog baseline.
void enter_knob_jog_mode() {
  _jog_tempo_last = analogRead(A8);
  _jog_swing_last = analogRead(A9);
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

// Returns +1 (clockwise / right / increase), -1 (counter-clockwise / left /
// decrease), or 0 (no movement) based on swing knob movement since last call.
int knob_jog_horizontal() {
  int cur = analogRead(A9);
  int delta = cur - _jog_swing_last;
  if (delta > KNOB_JOG_THRESHOLD) {
    _jog_swing_last = cur;
    return 1;
  } else if (delta < -KNOB_JOG_THRESHOLD) {
    _jog_swing_last = cur;
    return -1;
  }
  return 0;
}

// run_knob_routine — called every loop().
//
// The two hardware knobs (A8 = Tempo, A9 = Swing) serve as jog wheels for
// config menu navigation only. Tempo and Swing values are set exclusively
// via the config menu (items 16 and 17). Direct ADC-to-TEMPO mapping was
// removed because ADC noise caused constant clock instability.
void run_knob_routine() {
  if (config_menu_active) {
    // Jog functions are called on-demand by run_config_menu().
    return;
  }
  // Outside the config menu, knobs have no direct effect.
}
