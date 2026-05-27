// set_slider_mode(mode)
//
// Central entry point for all slider mode changes. Sets slider_mode, arms
// pickup guards for all 16 sliders so physical positions can't silently
// overwrite pattern data mid-switch, and requests an LCD redraw.
// Called from: Enter button (simple mode), pattern buttons 1/2/3 (advanced).
//
void set_slider_mode(uint8_t mode) {
  uint8_t old_mode = slider_mode;
  slider_mode = mode;
  // LV mode has no stored slider value to "pick up" — sliders only transmit on
  // movement, and the last-sent sentinel ensures the first move always fires.
  // Catch mode arms pickup guards so a physical position mismatch doesn't
  // silently overwrite stored data; Jump/Relative bypass pickup entirely.
  bool needs_pickup = (mode != 6 && slider_takeover == 0);
  for (int i = 0; i < 16; i++) {
    slider_needs_pickup[i] = needs_pickup;
    slider_pickup_dir[i]   = 0;
    // Seed last_raw from the current physical position so Relative mode
    // doesn't see a phantom delta on the first read after a mode switch.
    slider_last_raw[i] = voice_sliders[i].getValue();
  }
  slider_pickup_overlay_active = needs_pickup;  // overlay only useful in Catch
  update_line1 = true;
  update_line2 = true;
  // Update step LEDs to reflect the data type for the new mode.
  // CC (4), D (7), and LV (6) override the default step_data display.
  // CH (8) uses normal step_data display — step buttons toggle the step on/off
  // just like NN/VL/GT/PR, and chord type comes from the slider + current_chord_type.
  if (mode == 4 && ft_cc_mode) {
    read_cc_step_memory();
  } else if (mode == 7 && ft_drift_mode) {
    read_drift_step_memory();
  } else if ((old_mode == 4 || old_mode == 7) && mode != 6) {
    // Leaving CC/D for a step_data mode: restore step LEDs.
    read_step_memory(0, pattern_value);
  }
  // LV mode: clear edit state, reset send-history so first move on any lane
  // transmits, and turn step LEDs off (lit only for the lane being edited).
  if (mode == 6) {
    live_cc_editing_lane = -1;
    live_cc_last_lane    = -1;
    for (int i = 0; i < 16; i++) live_cc_last_sent[i] = 255;
    clear_step_leds();
  } else if (old_mode == 6) {
    // Leaving LV: restore step LEDs to whatever the new mode wants.
    live_cc_editing_lane = -1;
    if (mode == 4)      read_cc_step_memory();
    else if (mode == 7) read_drift_step_memory();
    else                read_step_memory(0, pattern_value);
  }
}

void run_voice_slider_routine()
{
  // Rate-limit ADC reads to every 20 ms.
  // 16 analogRead() calls on SAMD51 take ~160–800 µs and are the dominant
  // source of main-loop jitter. Sliders don't need sub-20ms update rates.
  static unsigned long last_slider_ms = 0;
  unsigned long now_ms = millis();
  if (now_ms - last_slider_ms < 20) return;
  last_slider_ms = now_ms;

  // Per-slider "rest" snapshot for pickup-overlay activity detection. Separate
  // from slider_last_raw[] (which Jump/Relative modes drive). The snapshot is
  // only advanced when a sample exceeds slider_noise_threshold from the rest
  // point — so jitter around a stationary value sits inside a deadband and
  // never trips activity, while a real touch crosses the band and updates the
  // rest. Threshold is user-configurable via Diagnostics → Noise (Low/Med/High
  // = 12 / 24 / 48 raw ADC counts; default 24 ≈ 0.6% of 12-bit range).
  static uint16_t      overlay_rest_raw[16] = {0};
  static bool          overlay_seeded = false;
  bool                 overlay_activity = false;

  // voice select sliders

  // Jump-mode movement threshold from the seed raw recorded at mode entry.
  // ~0.8% of full 12-bit range — above ADC jitter, well below an intentional touch.
  const int JUMP_RAW_THRESHOLD = 32;

  for (int j = 0; j <= 15; j++)
  {
    int sector      = voice_sliders[j].getSector();
    uint16_t raw    = voice_sliders[j].getValue();
    int raw_delta   = (int)raw - (int)slider_last_raw[j];
    bool jump_moved = (abs(raw_delta) > JUMP_RAW_THRESHOLD);

    if (overlay_seeded) {
      int adiff = (int)raw - (int)overlay_rest_raw[j];
      if (adiff < 0) adiff = -adiff;
      if (adiff > (int)slider_noise_threshold) {
        overlay_activity = true;
        overlay_rest_raw[j] = raw;  // shift rest only when threshold is crossed
      }
    } else {
      overlay_rest_raw[j] = raw;    // seed once on first pass
    }

    if (slider_mode == 1)
    {
      // NN mode: map to scale note pool when active, else full chromatic range.
      uint8_t new_value;
      if (scale_note_count > 0) {
        int pool_top = (int)scale_note_count - 1 + (int)slider_hi_trim;
        uint8_t idx = (uint8_t)map(sector, 0, 255, 0, pool_top);
        if (idx >= scale_note_count) idx = (uint8_t)(scale_note_count - 1);
        new_value = scale_note_pool[idx];
      } else {
        int eff_hi = (int)slider_map_high_value + (int)slider_hi_trim;
        if (eff_hi > 127) eff_hi = 127;
        new_value = (uint8_t)map(sector, 0, 255, slider_map_low_value, (uint8_t)eff_hi);
      }
      raw_voice_slider_values[j] = new_value;
      uint8_t stored = voice_slider_midinotenum[j];

      if (slider_takeover == 2) {
        // Relative — scale ADC delta to value units, accumulate sub-unit motion
        // in slider_last_raw so small moves still add up over time.
        int range;
        if (scale_note_count > 0) range = (int)scale_note_count + (int)slider_hi_trim;
        else range = (int)slider_map_high_value + (int)slider_hi_trim - (int)slider_map_low_value + 1;
        if (range < 2) range = 2;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int new_stored;
          if (scale_note_count > 0) {
            // Move by value_delta scale steps from current pool index.
            int idx = 0;
            for (int k = 0; k < (int)scale_note_count; k++) {
              if (scale_note_pool[k] == stored) { idx = k; break; }
            }
            idx += value_delta;
            int pool_top = (int)scale_note_count - 1;
            if (idx < 0) idx = 0;
            if (idx > pool_top) idx = pool_top;
            new_stored = scale_note_pool[idx];
          } else {
            int eff_lo = (int)slider_map_low_value;
            int eff_hi = (int)slider_map_high_value + (int)slider_hi_trim;
            if (eff_hi > 127) eff_hi = 127;
            new_stored = (int)stored + value_delta;
            if (new_stored < eff_lo) new_stored = eff_lo;
            if (new_stored > eff_hi) new_stored = eff_hi;
          }
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)new_stored != stored) {
            voice_slider_midinotenum[j] = (uint8_t)new_stored;
            voice_slider_values[j] = new_stored;
            pattern_step_pitches[pattern_value][j] = (uint8_t)new_stored;
          }
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)new_value - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (new_value != stored) {
        voice_slider_midinotenum[j] = new_value;
        voice_slider_values[j] = new_value;
        pattern_step_pitches[pattern_value][j] = new_value;
      }
    }
    else if (slider_mode == 2 && ft_velocity_mode)
    {
      uint8_t vel = (uint8_t)map(sector, 0, 255, 1, 127);
      uint8_t stored = voice_slider_midivelocity[j];

      if (slider_takeover == 2) {
        int range = 127;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 1) nv = 1; if (nv > 127) nv = 127;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) {
            voice_slider_midivelocity[j] = (uint8_t)nv;
            pattern_step_velocities[pattern_value][j] = (uint8_t)nv;
          }
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)vel - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (vel != stored) {
        voice_slider_midivelocity[j] = vel;
        pattern_step_velocities[pattern_value][j] = vel;
      }
    }
    else if (slider_mode == 3 && ft_gate_mode)
    {
      uint8_t gate = (uint8_t)map(sector, 0, 255, 1, 16);
      if (gate < 1) gate = 1; if (gate > 16) gate = 16;
      uint8_t stored = step_gate[pattern_value][j];

      if (slider_takeover == 2) {
        int range = 16;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 1) nv = 1; if (nv > 16) nv = 16;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) step_gate[pattern_value][j] = (uint8_t)nv;
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          if (gate == stored) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                  slider_pickup_dir[j] = (gate < stored) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (gate != stored) {
        step_gate[pattern_value][j] = gate;
      }
    }
    else if (slider_mode == 4 && ft_cc_mode)
    {
      uint8_t ccval = (uint8_t)map(sector, 0, 255, 0, 127);
      uint8_t stored = cc_step_values[pattern_value][j];

      if (slider_takeover == 2) {
        int range = 128;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 0) nv = 0; if (nv > 127) nv = 127;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) cc_step_values[pattern_value][j] = (uint8_t)nv;
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)ccval - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (ccval != stored) {
        cc_step_values[pattern_value][j] = ccval;
      }
    }
    else if (slider_mode == 5)
    {
      uint8_t prob = (uint8_t)map(sector, 0, 255, 0, 100);
      uint8_t stored = step_probability[pattern_value][j];

      if (slider_takeover == 2) {
        int range = 101;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 0) nv = 0; if (nv > 100) nv = 100;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) step_probability[pattern_value][j] = (uint8_t)nv;
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)prob - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (prob != stored) {
        step_probability[pattern_value][j] = prob;
      }
    }
    else if (slider_mode == 6)
    {
      // LV mode: send MIDI CC live on movement. Read raw 12-bit ADC directly
      // (bypass the 256-sector quantization) and scale to 0–127. Each lane
      // transmits on its own CC#; channel is independent of the note channel.
      // No pickup guard and no takeover mode — first movement past last_sent fires.
      uint8_t value = (uint8_t)(raw >> 5);  // 0..4095 -> 0..127
      if (value != live_cc_last_sent[j]) {
        controlChange(live_cc_channel - 1, live_cc_number[j], value);
        live_cc_last_sent[j] = value;
        live_cc_last_lane = (int8_t)j;
        update_line2 = true;
      }
    }
    else if (slider_mode == 7 && ft_drift_mode)
    {
      uint8_t drift = (uint8_t)map(sector, 0, 255, 0, 12);
      if (drift > 12) drift = 12;
      uint8_t stored = step_drift_amount[pattern_value][j];

      if (slider_takeover == 2) {
        int range = 13;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 0) nv = 0; if (nv > 12) nv = 12;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) step_drift_amount[pattern_value][j] = (uint8_t)nv;
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)drift - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (drift != stored) {
        step_drift_amount[pattern_value][j] = drift;
      }
    }
    else if (slider_mode == 8 && ft_chord_mode)
    {
      // CH mode: slider sets per-step chord type (0..CHORD_COUNT-1). Step LEDs
      // reflect step_data (set by the step button), so the slider does not
      // touch LEDs — it only writes step_chord_type.
      uint8_t max_type = (uint8_t)(CHORD_COUNT - 1);
      uint8_t ctype = (uint8_t)map(sector, 0, 255, 0, max_type);
      if (ctype > max_type) ctype = max_type;
      uint8_t stored = step_chord_type[pattern_value][j];

      if (slider_takeover == 2) {
        int range = (int)CHORD_COUNT;
        int adc_per_unit = 4096 / range; if (adc_per_unit < 1) adc_per_unit = 1;
        int value_delta = raw_delta / adc_per_unit;
        if (value_delta != 0) {
          int nv = (int)stored + value_delta;
          if (nv < 0) nv = 0; if (nv > max_type) nv = max_type;
          int remainder = raw_delta - value_delta * adc_per_unit;
          slider_last_raw[j] = (uint16_t)((int)raw - remainder);
          if ((uint8_t)nv != stored) {
            step_chord_type[pattern_value][j] = (uint8_t)nv;
            update_line2 = true;
          }
        }
      } else if (slider_needs_pickup[j]) {
        if (slider_takeover == 0) {
          int diff = (int)ctype - (int)stored;
          if (abs(diff) <= 1) { slider_needs_pickup[j] = false; slider_pickup_dir[j] = 0; }
          else                 slider_pickup_dir[j] = (diff < 0) ? 1 : 2;
        } else if (jump_moved) {
          slider_needs_pickup[j] = false;
        }
      } else if (ctype != stored) {
        step_chord_type[pattern_value][j] = ctype;
        update_line2 = true;
      }
    }

    last_voice_slider_values[j] = voice_slider_values[j];
  }

  // Mark the activity tracker seeded after the first full pass so the very
  // first iteration after boot/mode-switch doesn't trip a phantom delta.
  overlay_seeded = true;

  // Catch-mode pickup overlay visibility logic:
  //   - Shown when any slider still needs pickup AND activity is recent.
  //   - Auto-hides after OVERLAY_IDLE_MS so line 2 can return to its normal
  //     step-trigger / mode display.
  //   - Re-shows when any slider moves while pickup is still pending.
  // The rising-edge seed below covers external activations (set_slider_mode
  // and the config-menu Takeover change) so the initial display gets its
  // full idle window even without movement.
  static unsigned long last_overlay_activity_ms = 0;
  static bool          prev_overlay = false;
  const unsigned long  OVERLAY_IDLE_MS = 2000;

  if (slider_pickup_overlay_active && !prev_overlay) {
    last_overlay_activity_ms = now_ms;
  }
  prev_overlay = slider_pickup_overlay_active;

  bool any_pending = false;
  for (int i = 0; i < 16; i++) {
    if (slider_needs_pickup[i]) { any_pending = true; break; }
  }

  if (!any_pending) {
    if (slider_pickup_overlay_active) {
      slider_pickup_overlay_active = false;
      update_line2 = true;
    }
  } else {
    if (overlay_activity) {
      last_overlay_activity_ms = now_ms;
      if (!slider_pickup_overlay_active) {
        slider_pickup_overlay_active = true;
        prev_overlay = true;
        update_line2 = true;
      }
    }
    if (slider_pickup_overlay_active) {
      if (now_ms - last_overlay_activity_ms >= OVERLAY_IDLE_MS) {
        slider_pickup_overlay_active = false;
        update_line2 = true;
      } else {
        update_line2 = true;
      }
    }
  }
}

void resetSliders()
{

  lcdflag = 93;  next_lcdflag = 93;  // reset sliders
  slider_reset_counter = 0;

  for (int i = 0; i <= 15; i++)
  {
    voice_slider_midinotenum[i] = slider_map_low_value;
    pattern_step_pitches[pattern_value][i] = slider_map_low_value;
    voice_slider_midivelocity[i] = 127;
    pattern_step_velocities[pattern_value][i] = 127;
    step_gate[pattern_value][i] = 1;
    step_probability[pattern_value][i] = 100;
    step_drift_enabled[pattern_value][i] = 0;
    step_drift_amount[pattern_value][i] = 0;
    step_chord_type[pattern_value][i] = 0;
    slider_needs_pickup[i] = false;
    slider_serial_message_factory("NN", i);
    slider_serial_message_factory("CC", i);
    slider_serial_message_factory("VL", i);
  }
  // Also seed any other blank patterns with the current low note so they're
  // ready to use without needing to manually reset them too.
  init_blank_patterns_to_range();
  // Rebuild scale pool in case the note range changed.
  build_scale_notes();
}

void slider_serial_message_factory(const char *slider_message_header, int j)
{

  /*

  the_serial_message = "Z";

  if(enterbutton.isPressed() && (slider_message_header == "VL"))
  {
    the_serial_message += "RV";
  }
  else if(enterbutton.isPressed() && (slider_message_header == "CC"))
  {
    the_serial_message += "RC";
  }
  else if(enterbutton.isPressed() && (slider_message_header == "NN"))
  {
    the_serial_message += "RN";


  }
  else 
  {
    the_serial_message += slider_message_header;
  }

  the_serial_message += ",";
  the_serial_message += j;
  the_serial_message += ",";


  if(slider_message_header == "VL")     
  {
    the_serial_message += voice_slider_midivelocity[j];
  }
  else if(slider_message_header == "CC")
  {
    the_serial_message += voice_slider_midicc[j];
  }
  else if(slider_message_header == "NN")
  {
    the_serial_message += voice_slider_midinotenum[j];
  } 
  else if(slider_message_header == "MC")
  {
    the_serial_message += voice_slider_midichannel[j];
  } 


  the_serial_message += ";";   
  serial_printer(the_serial_message);  
  */
}
