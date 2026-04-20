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
//
// Pickup guard: after a pattern switch or mode switch, slider_needs_pickup[j]
// is armed. The slider must physically reach the stored value (within tolerance)
// before it can write. This prevents silent data corruption when sliders are at
// different positions than the stored values.

// set_slider_mode — central entry point for all slider mode changes.
// Arms pickup guards for all 8 sliders and requests a full LCD redraw.
// Always call this; never write slider_mode directly.
void set_slider_mode(uint8_t mode) {
  slider_mode = mode;
  for (int i = 0; i < VOICE_COUNT; i++) {
    slider_needs_pickup[i] = true;
  }
  update_line1 = true;
  update_line2 = true;
  // In Beatseqr CC mode and non-CC mode both display step_data, so no
  // separate LED refresh is needed here (unlike Synthseqr's cc_step_memory).
}

void run_voice_slider_routine() {
  // Rate-limit ADC reads to every 20 ms to reduce main-loop jitter.
  static unsigned long last_slider_ms = 0;
  unsigned long now_ms = millis();
  if (now_ms - last_slider_ms < 20) return;
  last_slider_ms = now_ms;

  for (int j = 0; j < VOICE_COUNT; j++) {
    int sector = voice_sliders[j].getSector();  // 0–255

    if (slider_mode == 1) {
      // NN mode: map to MIDI note range (respects scale pool if active).
      uint8_t raw;
      if (scale_note_count > 0) {
        uint8_t idx = (uint8_t)map(sector, 0, 255, 0, (int)scale_note_count - 1);
        raw = scale_note_pool[idx];
      } else {
        raw = (uint8_t)map(sector, 0, 255, slider_map_low_value, slider_map_high_value);
      }
      raw_voice_slider_values[j] = raw;

      if (slider_needs_pickup[j]) {
        if (abs((int)raw - (int)voice_pitch[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (raw != voice_pitch[pattern_value][j]) {
        voice_pitch[pattern_value][j] = raw;
        voice_slider_values[j] = raw;
      }

    } else if (slider_mode == 2) {
      // VL mode: 0–127. Slider fully down = 0 = voice muted.
      uint8_t vel = (uint8_t)map(sector, 0, 255, 0, 127);
      raw_voice_slider_values[j] = vel;

      if (slider_needs_pickup[j]) {
        if (abs((int)vel - (int)voice_velocity[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (vel != voice_velocity[pattern_value][j]) {
        voice_velocity[pattern_value][j] = vel;
        voice_slider_values[j] = vel;
      }

    } else if (slider_mode == 3) {
      // GT mode: 1–8 steps.
      uint8_t gate = (uint8_t)constrain(map(sector, 0, 255, 1, 8), 1, 8);
      raw_voice_slider_values[j] = gate;

      if (slider_needs_pickup[j]) {
        if (gate == voice_gate[pattern_value][j]) {
          slider_needs_pickup[j] = false;
        }
      } else if (gate != voice_gate[pattern_value][j]) {
        voice_gate[pattern_value][j] = gate;
        voice_slider_values[j] = gate;
      }

    } else if (slider_mode == 4) {
      // CC mode: 0–127.
      uint8_t ccval = (uint8_t)map(sector, 0, 255, 0, 127);
      raw_voice_slider_values[j] = ccval;

      if (slider_needs_pickup[j]) {
        if (abs((int)ccval - (int)voice_cc_value[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (ccval != voice_cc_value[pattern_value][j]) {
        voice_cc_value[pattern_value][j] = ccval;
        voice_slider_values[j] = ccval;
      }

    } else if (slider_mode == 5) {
      // PR mode: probability 0–100%.
      uint8_t prob = (uint8_t)map(sector, 0, 255, 0, 100);
      raw_voice_slider_values[j] = prob;

      if (slider_needs_pickup[j]) {
        if (abs((int)prob - (int)voice_probability[pattern_value][j]) <= 1)
          slider_needs_pickup[j] = false;
      } else if (prob != voice_probability[pattern_value][j]) {
        voice_probability[pattern_value][j] = prob;
        voice_slider_values[j] = prob;
      }
    }

    last_voice_slider_values[j] = voice_slider_values[j];
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
  }
  init_blank_patterns_to_range();
  build_scale_notes();
}
