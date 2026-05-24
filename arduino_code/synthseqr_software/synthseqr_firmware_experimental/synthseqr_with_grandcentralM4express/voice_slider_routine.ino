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
  // Other modes need pickup guards so a physical position mismatch doesn't
  // silently overwrite stored data when the mode is entered.
  bool needs_pickup = (mode != 6);
  for (int i = 0; i < 16; i++) {
    slider_needs_pickup[i] = needs_pickup;
  }
  update_line1 = true;
  update_line2 = true;
  // Update step LEDs to reflect the data type for the new mode.
  // CC (4), D (7), and LV (6) all override the default step_data display.
  if (mode == 4 && ft_cc_mode) {
    read_cc_step_memory();
  } else if (mode == 7 && ft_drift_mode) {
    read_drift_step_memory();
  } else if ((old_mode == 4 || old_mode == 7) && mode != 6) {
    // Leaving CC or D for a step_data mode: restore step LEDs.
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

  // voice select sliders

  for (int j = 0; j <= 15; j++)
  {
    int sector = voice_sliders[j].getSector();

    if (slider_mode == 1)
    {
      // NN mode: map to scale note pool when active, else full chromatic range.
      // Pickup guard protects stored pitch across mode/pattern switches.
      if (scale_note_count > 0) {
        int pool_top = (int)scale_note_count - 1 + (int)slider_hi_trim;
        uint8_t idx = (uint8_t)map(sector, 0, 255, 0, pool_top);
        if (idx >= scale_note_count) idx = (uint8_t)(scale_note_count - 1);
        raw_voice_slider_values[j] = scale_note_pool[idx];
      } else {
        int eff_hi = (int)slider_map_high_value + (int)slider_hi_trim;
        if (eff_hi > 127) eff_hi = 127;
        raw_voice_slider_values[j] = (uint8_t)map(sector, 0, 255, slider_map_low_value, (uint8_t)eff_hi);
      }

      if (slider_needs_pickup[j])
      {
        // After a pattern switch the slider is locked to the stored pitch.
        // Unlock when the physical position comes within 1 note of that pitch.
        if (abs((int)raw_voice_slider_values[j] - (int)voice_slider_midinotenum[j]) <= 1)
        {
          slider_needs_pickup[j] = false;
        }
      }
      else if (raw_voice_slider_values[j] != voice_slider_midinotenum[j])
      {
        voice_slider_midinotenum[j] = raw_voice_slider_values[j];
        voice_slider_values[j] = raw_voice_slider_values[j];
        // Persist pitch for this pattern so it survives future pattern switches.
        pattern_step_pitches[pattern_value][j] = (uint8_t)raw_voice_slider_values[j];
      }
    }
    else if (slider_mode == 2 && ft_velocity_mode)
    {
      // VL mode: map to velocity range 1-127, pickup guard prevents writes
      // until the slider physically reaches the stored velocity.
      uint8_t vel = (uint8_t)map(sector, 0, 255, 1, 127);
      if (slider_needs_pickup[j]) {
        if (abs((int)vel - (int)voice_slider_midivelocity[j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (vel != voice_slider_midivelocity[j]) {
        voice_slider_midivelocity[j] = vel;
        pattern_step_velocities[pattern_value][j] = vel;
      }
    }
    else if (slider_mode == 3 && ft_gate_mode)
    {
      // GT mode: map to gate range 1-16, pickup guard prevents writes until
      // the slider physically reaches the stored gate value.
      uint8_t gate = (uint8_t)map(sector, 0, 255, 1, 16);
      if (gate < 1) gate = 1;
      if (gate > 16) gate = 16;
      if (slider_needs_pickup[j]) {
        if (gate == step_gate[pattern_value][j]) {
          slider_needs_pickup[j] = false;
        }
      } else if (gate != step_gate[pattern_value][j]) {
        step_gate[pattern_value][j] = gate;
      }
    }
    else if (slider_mode == 4 && ft_cc_mode)
    {
      // CC mode: map to CC value range 0-127, pickup guard prevents writes
      // until the slider physically reaches the stored CC value.
      uint8_t ccval = (uint8_t)map(sector, 0, 255, 0, 127);
      if (slider_needs_pickup[j]) {
        if (abs((int)ccval - (int)cc_step_values[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (ccval != cc_step_values[pattern_value][j]) {
        cc_step_values[pattern_value][j] = ccval;
      }
    }
    else if (slider_mode == 5)
    {
      // PR mode: map to probability range 0-100, pickup guard prevents writes
      // until the slider physically reaches the stored probability value.
      uint8_t prob = (uint8_t)map(sector, 0, 255, 0, 100);
      if (slider_needs_pickup[j]) {
        if (abs((int)prob - (int)step_probability[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (prob != step_probability[pattern_value][j]) {
        step_probability[pattern_value][j] = prob;
      }
    }
    else if (slider_mode == 6)
    {
      // LV mode: send MIDI CC live on movement. Read raw 12-bit ADC directly
      // (bypass the 256-sector quantization) and scale to 0–127. Each lane
      // transmits on its own CC#; channel is independent of the note channel.
      // No pickup guard — first movement past last_sent fires immediately.
      uint16_t raw = voice_sliders[j].getValue();
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
      // D mode: per-step drift amount 0–12 semitones. Step buttons toggle
      // step_drift_enabled; the slider sets step_drift_amount. Pickup guard
      // prevents physical-position mismatch from corrupting other patterns'
      // drift values across mode/pattern switches.
      uint8_t drift = (uint8_t)map(sector, 0, 255, 0, 12);
      if (drift > 12) drift = 12;
      if (slider_needs_pickup[j]) {
        if (abs((int)drift - (int)step_drift_amount[pattern_value][j]) <= 1) {
          slider_needs_pickup[j] = false;
        }
      } else if (drift != step_drift_amount[pattern_value][j]) {
        step_drift_amount[pattern_value][j] = drift;
      }
    }

    last_voice_slider_values[j] = voice_slider_values[j];
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
