// voice_select_routine.ino
//
// Reads the resistor-ladder voice select input (A10).
// Eight buttons share one analog pin; each pulls the pin to a different voltage
// via a resistor divider. ADC thresholds (vselectval_lowerranges/upperranges)
// were calibrated for 10-bit ADC (0–1023) — analogReadResolution(10) must be
// called in setup() to keep them valid.
//
// Detection uses a small averaging buffer to suppress ADC noise and contact
// bounce. A voice change is committed only when VOICE_SELECT_SAMPLES
// consecutive reads all agree on the same button (or all agree on "none").
//
// When no button is pressed the analog value sits near 1023 (no pull-down),
// which is outside all calibrated ranges. The current voice is kept active
// until a different button is detected.

#define VOICE_SELECT_SAMPLES 4   // readings to average before committing

static int  _vs_buf[VOICE_SELECT_SAMPLES] = {0, 0, 0, 0};
static int  _vs_buf_idx    = 0;
static int  _vs_prev_voice = -1;   // -1 = "no button was held" last frame

// Classify an averaged ADC reading into a voice index (0–7) or -1 (none).
static int detect_voice_from_adc(int adc_val) {
  for (int i = 0; i < VOICE_COUNT; i++) {
    if (adc_val >= vselectval_lowerranges[i] && adc_val <= vselectval_upperranges[i]) {
      return i;
    }
  }
  return -1;
}

// run_voice_select_routine — call every loop().
void run_voice_select_routine() {
  // Fill rolling average buffer.
  _vs_buf[_vs_buf_idx] = analogRead(A10);
  _vs_buf_idx = (_vs_buf_idx + 1) % VOICE_SELECT_SAMPLES;

  int sum = 0;
  for (int i = 0; i < VOICE_SELECT_SAMPLES; i++) sum += _vs_buf[i];
  int avg = sum / VOICE_SELECT_SAMPLES;

  int detected = detect_voice_from_adc(avg);

  // Commit a voice change only on the rising edge of a new press.
  // Holding the same button does nothing after the initial switch.
  if (detected != _vs_prev_voice) {
    _vs_prev_voice = detected;
    if (detected != -1) {
      set_current_voice((uint8_t)detected);
    }
    // On release (detected == -1) we intentionally keep current_voice as-is.
  }
}

// set_current_voice — switch the active editing voice.
//
// Updates current_voice, refreshes voice-select LEDs, reloads step LEDs to
// show the new voice's pattern, and arms pickup guards for all sliders so a
// mode-switch or voice-switch can't silently overwrite stored data.
void set_current_voice(uint8_t v) {
  if (v >= VOICE_COUNT) return;

  current_voice      = v;
  last_voice_selected = v;

  // Light only the active voice LED.
  for (int i = 0; i < VOICE_COUNT; i++) {
    if (i == (int)v) voice_select_leds[i].on();
    else             voice_select_leds[i].off();
  }

  // Step LEDs now show this voice's active steps.
  read_step_memory(current_voice, pattern_value);

  // Arm pickup guards: sliders must physically reach the stored value for the
  // new voice before they can overwrite it (prevents silent data corruption).
  for (int i = 0; i < VOICE_COUNT; i++) slider_needs_pickup[i] = true;

  update_line1 = true;
  update_line2 = true;
}
