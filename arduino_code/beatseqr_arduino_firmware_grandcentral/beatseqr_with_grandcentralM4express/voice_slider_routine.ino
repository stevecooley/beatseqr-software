// voice_slider_routine.ino
//
// Reads the 8 voice sliders (A7–A0, one per voice) and applies their values
// to the active pattern's per-voice data arrays.
//
// Slider j always controls voice j — the mapping is fixed regardless of
// current_voice. This means all 8 voice levels are simultaneously adjustable
// (e.g. in VL mode every fader acts as a live volume for its voice).
//
// Slider modes:
//   1 = NN  note number → voice_pitch[pattern][j]
//   2 = VL  velocity   → voice_velocity[pattern][j]   (0 = muted)
//   3 = GT  gate       → voice_gate[pattern][j]
//   4 = CC  CC value   → voice_cc_value[pattern][j]
//   5 = PR  probability→ voice_probability[pattern][j]
//
// Takeover behaviour (slider_takeover, set in config menu):
//   0 = Catch    — slider must reach the stored value (pickup) before writing
//   1 = Jump     — slider engages on first movement (> JUMP_RAW_THRESHOLD)
//   2 = Relative — value tracks the delta from the slider's physical position;
//                  pickup is bypassed entirely

// Jump-mode movement threshold from the seed raw recorded at mode entry.
// ~0.8% of full 12-bit range — above ADC jitter, well below an intentional touch.
#define JUMP_RAW_THRESHOLD 32

// set_slider_mode — central entry point for all slider mode changes.
// Arms pickup guards (Catch only) for all 8 sliders, seeds slider_last_raw[]
// from the physical positions, and requests a full LCD redraw.
// Always call this; never write slider_mode directly.
void set_slider_mode(uint8_t mode) {
  slider_mode = mode;
  // Catch arms pickup so a physical position mismatch can't silently overwrite
  // stored data; Jump/Relative bypass pickup entirely.
  bool needs_pickup = (slider_takeover == 0);
  for (int i = 0; i < VOICE_COUNT; i++) {
    slider_needs_pickup[i] = needs_pickup;
    // Seed last_raw from the current physical position so Relative mode doesn't
    // see a phantom delta, and Jump has a baseline to measure movement from.
    slider_last_raw[i] = voice_sliders[i].getValue();
  }
  update_line1 = true;
  update_line2 = true;
  // In Beatseqr CC mode and non-CC mode both display step_data, so no
  // separate LED refresh is needed here (unlike Synthseqr's cc_step_memory).
}

// slider_take — apply the active takeover behaviour for an integer-valued mode
// (VL/GT/CC/PR, and NN's non-scale path). On a write, sets *out to the new
// stored value and returns true. Updates slider_needs_pickup[j] /
// slider_last_raw[j] as side effects.
//   value      : freshly mapped slider value for this read (Catch/Jump compare)
//   lo, hi     : clamp range for this mode
//   stored     : current stored value
//   raw        : raw 12-bit ADC reading this loop
//   raw_delta  : raw - slider_last_raw[j]
//   jump_moved : abs(raw_delta) > JUMP_RAW_THRESHOLD
//   tol        : Catch pickup tolerance (0 = exact match, e.g. GT)
static bool slider_take(int j, int value, int lo, int hi, int stored,
                        uint16_t raw, int raw_delta, bool jump_moved,
                        int tol, int *out) {
  if (slider_takeover == 2) {
    // Relative — scale ADC delta to value units, carry sub-unit motion in
    // slider_last_raw so small moves still accumulate over time.
    int range = hi - lo + 1;
    if (range < 2) range = 2;
    int adc_per_unit = 4096 / range;
    if (adc_per_unit < 1) adc_per_unit = 1;
    int value_delta = raw_delta / adc_per_unit;
    if (value_delta != 0) {
      int nv = stored + value_delta;
      if (nv < lo) nv = lo;
      if (nv > hi) nv = hi;
      int remainder = raw_delta - value_delta * adc_per_unit;
      slider_last_raw[j] = (uint16_t)((int)raw - remainder);
      if (nv != stored) { *out = nv; return true; }
    }
    return false;
  }

  if (slider_needs_pickup[j]) {
    if (slider_takeover == 0) {            // Catch
      if (abs(value - stored) <= tol) slider_needs_pickup[j] = false;
    } else if (jump_moved) {               // Jump
      slider_needs_pickup[j] = false;
    }
    return false;
  }

  if (value != stored) { *out = value; return true; }
  return false;
}

void run_voice_slider_routine() {
  // Voice sliders disabled (e.g. noisy/faulty hardware) — skip all reads so they
  // can't overwrite stored data. Set notes via the swing-knob Note mode instead.
  if (!ft_voice_sliders) return;

  // Rate-limit ADC reads to every 20 ms to reduce main-loop jitter.
  static unsigned long last_slider_ms = 0;
  unsigned long now_ms = millis();
  if (now_ms - last_slider_ms < 20) return;
  last_slider_ms = now_ms;

  for (int j = 0; j < VOICE_COUNT; j++) {
    // In swing-knob Note mode, slider 0 is repurposed to set the selected
    // voice's note (run_note_slider_override) — don't also process it here.
    if (swing_knob_function == 4 && j == 0) continue;

    int sector      = voice_sliders[j].getSector();  // 0–255
    uint16_t raw    = voice_sliders[j].getValue();   // raw 12-bit
    int raw_delta   = (int)raw - (int)slider_last_raw[j];
    bool jump_moved = (abs(raw_delta) > JUMP_RAW_THRESHOLD);
    int  out        = 0;

    if (slider_mode == 1) {
      // NN mode: map to MIDI note range (respects scale pool if active).
      uint8_t value;
      if (scale_note_count > 0) {
        uint8_t idx = (uint8_t)map(sector, 0, 255, 0, (int)scale_note_count - 1);
        value = scale_note_pool[idx];
      } else {
        value = (uint8_t)map(sector, 0, 255, slider_map_low_value, slider_map_high_value);
      }
      raw_voice_slider_values[j] = value;
      uint8_t stored = voice_pitch[pattern_value][j];

      if (slider_takeover == 2 && scale_note_count > 0) {
        // Relative through the scale pool: move by value_delta indices.
        int range = (int)scale_note_count;
        if (range < 2) range = 2;
        int adc_per_unit = 4096 / range;
        if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int idx = 0;
          for (int k = 0; k < (int)scale_note_count; k++) {
            if (scale_note_pool[k] == stored) { idx = k; break; }
          }
          idx += value_delta;
          if (idx < 0) idx = 0;
          if (idx > (int)scale_note_count - 1) idx = (int)scale_note_count - 1;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          uint8_t nv = scale_note_pool[idx];
          if (nv != stored) {
            voice_pitch[pattern_value][j] = nv;
            voice_slider_values[j] = nv;
          }
        }
      } else if (slider_take(j, value, slider_map_low_value, slider_map_high_value,
                             stored, raw, raw_delta, jump_moved, 1, &out)) {
        voice_pitch[pattern_value][j] = (uint8_t)out;
        voice_slider_values[j] = out;
      }

    } else if (slider_mode == 2) {
      // VL mode: 0–127. Slider fully down = 0 = voice muted.
      uint8_t vel = (uint8_t)map(sector, 0, 255, 0, 127);
      raw_voice_slider_values[j] = vel;
      if (slider_take(j, vel, 0, 127, voice_velocity[pattern_value][j],
                      raw, raw_delta, jump_moved, 1, &out)) {
        voice_velocity[pattern_value][j] = (uint8_t)out;
        voice_slider_values[j] = out;
      }

    } else if (slider_mode == 3) {
      // GT mode: 1–8 steps. Catch uses exact match (tol 0).
      uint8_t gate = (uint8_t)constrain(map(sector, 0, 255, 1, 8), 1, 8);
      raw_voice_slider_values[j] = gate;
      if (slider_take(j, gate, 1, 8, voice_gate[pattern_value][j],
                      raw, raw_delta, jump_moved, 0, &out)) {
        voice_gate[pattern_value][j] = (uint8_t)out;
        voice_slider_values[j] = out;
      }

    } else if (slider_mode == 4) {
      // CC mode: 0–127.
      uint8_t ccval = (uint8_t)map(sector, 0, 255, 0, 127);
      raw_voice_slider_values[j] = ccval;
      if (slider_take(j, ccval, 0, 127, voice_cc_value[pattern_value][j],
                      raw, raw_delta, jump_moved, 1, &out)) {
        voice_cc_value[pattern_value][j] = (uint8_t)out;
        voice_slider_values[j] = out;
      }

    } else if (slider_mode == 5) {
      // PR mode: probability 0–100%.
      uint8_t prob = (uint8_t)map(sector, 0, 255, 0, 100);
      raw_voice_slider_values[j] = prob;
      if (slider_take(j, prob, 0, 100, voice_probability[pattern_value][j],
                      raw, raw_delta, jump_moved, 1, &out)) {
        voice_probability[pattern_value][j] = (uint8_t)out;
        voice_slider_values[j] = out;
      }
    }

    last_voice_slider_values[j] = voice_slider_values[j];
  }
}

// run_note_slider_override — slider-0 note entry for the swing-knob Note mode.
//
// When swing_knob_function == 4 (Note) the swing knob is NOT used (its hardware
// is unreliable). Instead voice 1's slider (slider 0) sets the SELECTED voice's
// note across the configurable Note range (slider_map_low_value ..
// slider_map_high_value — the same range NN slider mode uses, set via config
// menu → Note range); the slider's full travel spans that range. This works even
// when the voice sliders are otherwise disabled (ft_voice_sliders off) — only
// slider 0 is read here.
//
// A jump guard re-arms whenever the selected voice changes, so switching voices
// to view their notes doesn't clobber them: the note starts tracking slider 0
// only after the slider physically moves past JUMP_RAW_THRESHOLD.
void run_note_slider_override() {
  if (swing_knob_function != 4) return;
  if (config_menu_active) return;

  static unsigned long last_ms = 0;
  unsigned long now = millis();
  if (now - last_ms < 20) return;   // match the slider routine's read cadence
  last_ms = now;

  static uint8_t armed_voice = 0xFF;   // voice the guard is anchored to
  static int     seed_raw    = -1;     // slider-0 raw at (re)arm; -1 = unlocked

  uint16_t raw = voice_sliders[0].getValue();    // raw 12-bit

  // Re-arm on selected-voice change so a voice switch can't overwrite the note.
  if (armed_voice != current_voice) {
    armed_voice = current_voice;
    seed_raw    = (int)raw;
    return;
  }

  // Wait for a real slider move before taking over (faulty-slider jitter guard).
  if (seed_raw >= 0) {
    if (abs((int)raw - seed_raw) <= JUMP_RAW_THRESHOLD) return;
    seed_raw = -1;   // unlocked: track absolutely from now on
  }

  // Map full slider travel (0–4095) across the configurable Note range so both
  // endpoints are reachable. Both low and high config values take effect here.
  int lo = slider_map_low_value;
  int hi = slider_map_high_value;
  int note = map((int)raw, 0, 4095, lo, hi);
  if (note < lo) note = lo;
  if (note > hi) note = hi;
  if (note != (int)voice_pitch[pattern_value][current_voice]) {
    voice_pitch[pattern_value][current_voice] = (uint8_t)note;
    update_line2 = true;
  }
}

// resetSliders — reset all 8 voices' pitch/velocity/gate/CC to defaults
// for the current pattern. Called from config menu "Reset sliders" item.
void resetSliders() {
  lcdflag = 93; next_lcdflag = 93;
  for (int v = 0; v < VOICE_COUNT; v++) {
    voice_pitch[pattern_value][v]        = slider_map_low_value;
    voice_velocity[pattern_value][v]     = 100;
    voice_gate[pattern_value][v]         = 1;
    voice_cc_value[pattern_value][v]     = 0;
    voice_probability[pattern_value][v]  = 100;
    slider_needs_pickup[v]               = false;
    slider_last_raw[v]                   = voice_sliders[v].getValue();
  }
  init_blank_patterns_to_range();
  build_scale_notes();
}
