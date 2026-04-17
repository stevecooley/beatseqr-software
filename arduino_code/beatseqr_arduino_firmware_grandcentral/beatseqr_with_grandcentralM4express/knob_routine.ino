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
// In config menu mode: keeps jog baselines current so deltas stay meaningful
// even when run_config_menu() isn't actively reading them (i.e. idle frames).
// The config menu calls knob_jog_vertical() / knob_jog_horizontal() directly
// when it needs navigation input; this function just keeps things ticking.
//
// In normal mode: maps knob ADC values to TEMPO and SWING and applies them.
// TEMPO changes are rate-limited to ≥1 BPM difference to avoid constant redraws
// from ADC noise.
void run_knob_routine() {
  if (config_menu_active) {
    // Nothing extra needed here — jog functions are called on-demand by
    // run_config_menu(). We return so tempo/swing aren't clobbered.
    return;
  }

  // --- Tempo knob ---
  // Only active in internal clock mode; external clock drives its own tempo.
  if (!external_clock_mode) {
    int raw_tempo = analogRead(A8);
    // 12-bit ADC range 0-4095. Mapping upper→lower so CW = lower raw = higher BPM;
    // swap lower_BPM_number / upper_BPM_number if knob direction is still wrong.
    float new_tempo = (float)map(raw_tempo, 0, 4095,
                                 upper_BPM_number, lower_BPM_number);
    if (fabsf(new_tempo - TEMPO) >= 1.0f) {
      TEMPO = new_tempo;
      seq.setTempo(TEMPO);
      setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
      update_line1 = true;
    }
  }

  // --- Swing knob ---
  // 8 evenly-spaced sectors, clamped to 0–5 (same range as Synthseqr).
  {
    int raw_swing = analogRead(A9);
    // Map 0-4095 (12-bit) to 0-5 in six equal bands.
    uint8_t new_swing = (uint8_t)constrain(map(raw_swing, 0, 4095, 0, 5), 0, 5);
    if (new_swing != SWING) {
      SWING = new_swing;
      // No LCD redraw needed — SWING isn't shown on the main display.
    }
  }
}
