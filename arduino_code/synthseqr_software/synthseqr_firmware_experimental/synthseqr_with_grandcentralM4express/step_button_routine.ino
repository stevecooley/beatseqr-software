#if FEATURE_NOTE_AUDITION
// audition_step_note()
//
// Plays a brief MIDI note for step i with gate_steps × 16th-note duration
// at the current tempo. Applies octave/note shift and scale quantization
// exactly as stepsend() does, but skips pitch drift so the preview is stable.
// Cancels any previously-sounding audition note before sending the new one.
// Only called when ft_note_audition is true and the sequencer is stopped.
//
void audition_step_note(int step, uint8_t gate_steps) {
  if (audition_sounding_note >= 0) {
    noteOff(MIDICHANNEL - 1, (uint8_t)audition_sounding_note, 0);
    MidiUSB.flush();
    audition_sounding_note = -1;
  }

  int16_t shifted = (int16_t)voice_slider_midinotenum[step];
  if (ft_octave_note_shift) {
    shifted += (int16_t)(octave_shift * 12) + (int16_t)note_shift;
  }
  if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0) {
    if (shifted < 0)   shifted = 0;
    if (shifted > 127) shifted = 127;
    shifted = (int16_t)quantize_to_scale((uint8_t)shifted);
  }
  if (shifted < 0)   shifted = 0;
  if (shifted > 127) shifted = 127;

  uint8_t pitch = (uint8_t)shifted;
  uint8_t vel   = voice_slider_midivelocity[step];

  unsigned long gate_ms = (unsigned long)((float)gate_steps * 60000.0f / TEMPO / 4.0f);
  if (gate_ms < 50) gate_ms = 50;

  noteOn(MIDICHANNEL - 1, pitch, vel);
  MidiUSB.flush();
  audition_sounding_note = (int8_t)pitch;
  audition_note_off_ms   = millis() + gate_ms;
}
#endif  // FEATURE_NOTE_AUDITION

void run_step_button_routine()
{
#if FEATURE_NOTE_AUDITION
  // Send note-off for any sounding audition note once its gate duration elapses.
  if (audition_sounding_note >= 0 && (long)(millis() - audition_note_off_ms) >= 0) {
    noteOff(MIDICHANNEL - 1, (uint8_t)audition_sounding_note, 0);
    MidiUSB.flush();
    audition_sounding_note = -1;
  }
#endif

  // Advanced mode pattern copy phase 1: tap a step button to pick the source
  // pattern to copy FROM. Navigates to that pattern, then arms phase 2.
  if (adv_copy_waiting_source) {
    for (int i = 0; i < 16; i++) {
      if (step_buttons[i].uniquePress()) {
        copy_pattern_from = i;
        go_to_pattern(i, 0);
        if (!adv_pat_nav_active) read_step_memory(0, i);
        adv_copy_waiting_source = false;
        adv_copy_armed = true;
        lcdflag = 102;  next_lcdflag = 102;  update_line1 = true;  // "Copy N->where?" = pick destination
        Serial.print("copy source: pattern ");
        Serial.println(i + 1);
        break;
      }
    }
    return;  // consume all other input while picking source
  }

  // Advanced mode pattern copy phase 2: tap a step button to pick the
  // destination pattern to copy TO.
  if (adv_copy_armed) {
    for (int i = 0; i < 16; i++) {
      if (step_buttons[i].uniquePress()) {
        // Copy all step data, pitches, velocities, gates, and CC from current_pattern → i.
        for (int s = 0; s < 16; s++) {
          step_data[i][0][s] = step_data[current_pattern][0][s];
          pattern_step_pitches[i][s] = pattern_step_pitches[current_pattern][s];
          pattern_step_velocities[i][s] = pattern_step_velocities[current_pattern][s];
          step_gate[i][s] = step_gate[current_pattern][s];
#if FEATURE_CC_MODE
          cc_step_enabled[i][s] = cc_step_enabled[current_pattern][s];
          cc_step_values[i][s] = cc_step_values[current_pattern][s];
#endif
          step_probability[i][s] = step_probability[current_pattern][s];
          step_drift_enabled[i][s] = step_drift_enabled[current_pattern][s];
          step_drift_amount[i][s] = step_drift_amount[current_pattern][s];
          step_chord_type[i][s] = step_chord_type[current_pattern][s];
        }
#if FEATURE_CC_MODE
        cc_number[i] = cc_number[current_pattern];
#endif
        copy_pattern_to = i;
        adv_copy_armed = false;
        adv_pat_nav_active = false;
        go_to_pattern(i, 0);
        read_step_memory(0, i);
        lcdflag = 101;  next_lcdflag = 101;  // "Copy {n}-> done" then back to main
        Serial.print("copied to pattern ");
        Serial.println(i + 1);
        break;
      }
    }
    return;  // consume all other input while copy is armed
  }

  // In pattern nav mode, step buttons are consumed by run_pattern_select_routine()
  // for pattern selection/chaining. Skip normal step-toggle processing.
  if (!adv_pat_nav_active) {
    detect_step_button_presses();
  }
}


// Turn a step ON if it isn't already. Seeds pitch from live slider position on
// blank patterns. Used by the gate-set gesture so the source step is guaranteed
// to be on after the gesture completes.
void do_step_on(int i)
{
  if (step_data[pattern_value][0][i] != 0) return;  // already on — nothing to do
  step_data[pattern_value][0][i] = 1;
  step_leds[i].on();
  bool was_blank = true;
  for (int s = 0; s < 16; s++) {
    if (s != i && step_data[pattern_value][0][s] != 0) { was_blank = false; break; }
  }
  if (was_blank) {
    uint8_t live_pitch = (uint8_t)raw_voice_slider_values[i];
    voice_slider_midinotenum[i] = live_pitch;
    pattern_step_pitches[pattern_value][i] = live_pitch;
    slider_needs_pickup[i] = false;
  }
  // Paint the active chord type onto this newly activated step so the
  // gate-set source step picks up the current "Chord" d-pad binding.
  if (ft_chord_mode) step_chord_type[pattern_value][i] = current_chord_type;
  seq.setNote(MIDICHANNEL - 1, voice_slider_midinotenum[i], 127, i);
}

// Toggle a step on/off: flips step_data, updates the LED, seeds pitch from
// the live slider if this is the first note in a blank pattern, and notifies
// the sequencer library. Turning a step OFF also resets its gate to 1.
// Called by detect_step_button_presses() for both immediate taps and
// deferred-hold releases.
void do_step_toggle(int i)
{
  step_data[pattern_value][0][i] = step_data[pattern_value][0][i] ? 0 : 1;

  if (step_data[pattern_value][0][i]) {
    step_leds[i].on();
  } else {
    step_leds[i].off();
  }

  if (step_data[pattern_value][0][i] == 1) {
    // Blank-pattern seed: if every other step is off, use the live slider
    // position instead of the stored default so the note lands where the
    // slider already sits.
    bool was_blank = true;
    for (int s = 0; s < 16; s++) {
      if (s != i && step_data[pattern_value][0][s] != 0) {
        was_blank = false;
        break;
      }
    }
    if (was_blank) {
      uint8_t live_pitch = (uint8_t)raw_voice_slider_values[i];
      voice_slider_midinotenum[i] = live_pitch;
      pattern_step_pitches[pattern_value][i] = live_pitch;
      slider_needs_pickup[i] = false;
    }
    // Paint the active chord type onto this newly activated step. With
    // dpad_main_mode == DPAD_MAIN_MODE_CHORD the d-pad cycles current_chord_type,
    // so step toggles inherit whatever chord the user has selected.
    if (ft_chord_mode) step_chord_type[pattern_value][i] = current_chord_type;
    seq.setNote(MIDICHANNEL - 1, voice_slider_midinotenum[i], 127, i);
#if FEATURE_NOTE_AUDITION
    if (ft_note_audition && !playstatus) audition_step_note(i, 1);
#endif
  } else {
    step_gate[pattern_value][i] = 1;  // gate resets when step is turned off
    // Clear chord type too — keeps step_chord_type aligned with step_data
    // so re-activating the step starts from a clean slate.
    if (ft_chord_mode) step_chord_type[pattern_value][i] = 0;
    seq.setNote(MIDICHANNEL - 1, voice_slider_midinotenum[i], 0, i);
  }
}

// detect_step_button_presses()
//
// Handles step button presses in normal and gate-set gesture modes.
//
// Gate-set gesture: hold one step button for >= 150 ms, then tap a second
// step button. The gate for the held step is set to the forward distance
// between the two buttons (1–16, wrapping). The held step does NOT toggle;
// the tapped step does NOT toggle. LEDs flash the gate range for 300 ms then
// restore to normal step display.
//
// Plain tap (no gesture): toggle fires on button release (deferred up to 150 ms)
// so the firmware has time to detect whether the press will become a hold.
// Quick double-taps (second press < 150 ms after first) toggle both steps.
//
// CC mode bypasses the gesture entirely; step buttons toggle cc_step_enabled.
void detect_step_button_presses()
{
  static int8_t   gate_hold_step     = -1;
  static unsigned long gate_hold_start_ms = 0;
  static bool     gate_gesture_fired = false;
  static unsigned long gate_flash_end_ms  = 0;

  // Restore LEDs once the gate-flash window expires.
  if (gate_flash_end_ms != 0 && (long)(millis() - gate_flash_end_ms) >= 0) {
    gate_flash_end_ms = 0;
#if FEATURE_CC_MODE
    if (slider_mode == 4)      read_cc_step_memory();
    else if (slider_mode == 7) read_drift_step_memory();
    else if (slider_mode == 8) read_chord_step_memory();
    else                       read_step_memory(0, pattern_value);
#else
    if (slider_mode == 7)      read_drift_step_memory();
    else if (slider_mode == 8) read_chord_step_memory();
    else                       read_step_memory(0, pattern_value);
#endif
  }

  // Release check: if the tracked hold step was released, fire its deferred
  // toggle unless a gate gesture already consumed this hold.
  if (gate_hold_step >= 0 && !step_buttons[gate_hold_step].wasPressed()) {
    if (!gate_gesture_fired) {
      do_step_toggle(gate_hold_step);
    }
    gate_hold_step = -1;
    gate_gesture_fired = false;
  }

  for (int i = 0; i <= 15; i++) {
    if (!step_buttons[i].uniquePress()) continue;

#if FEATURE_CC_MODE
    if (slider_mode == 4) {
      // CC mode: immediate toggle, no gate-set gesture.
      cc_step_enabled[pattern_value][i] = cc_step_enabled[pattern_value][i] ? 0 : 1;
      if (cc_step_enabled[pattern_value][i]) step_leds[i].on();
      else                                   step_leds[i].off();
      continue;
    }
#endif  // FEATURE_CC_MODE

    if (slider_mode == 6) {
      // LV mode: step buttons select which lane's CC# is being edited.
      // Tap same lane = exit editing. Tap a different lane = switch focus.
      // No step_data toggling, no gate-set gesture, no audition.
      if (live_cc_editing_lane == (int8_t)i) {
        live_cc_editing_lane = -1;
        step_leds[i].off();
      } else {
        if (live_cc_editing_lane >= 0) step_leds[live_cc_editing_lane].off();
        live_cc_editing_lane = (int8_t)i;
        step_leds[i].on();
      }
      update_line2 = true;
      continue;
    }

    if (slider_mode == 7 && ft_drift_mode) {
      // D mode: step buttons toggle per-step drift on/off. LED reflects the
      // enable state. No step_data toggling, no gate-set gesture, no audition.
      step_drift_enabled[pattern_value][i] = step_drift_enabled[pattern_value][i] ? 0 : 1;
      if (step_drift_enabled[pattern_value][i]) step_leds[i].on();
      else                                      step_leds[i].off();
      continue;
    }

    if (slider_mode == 8 && ft_chord_mode) {
      // CH mode: step buttons clear the per-step chord type back to 0 (single
      // note). Sliders set non-zero values. LED reflects "has chord" state.
      // No step_data toggling, no gate-set gesture, no audition.
      step_chord_type[pattern_value][i] = 0;
      step_leds[i].off();
      update_line2 = true;
      continue;
    }

    if (gate_hold_step >= 0 && gate_hold_step != i && !gate_gesture_fired) {
      if ((long)(millis() - gate_hold_start_ms) >= 150) {
        // Gate-set gesture: compute forward distance from held step to tapped step.
        uint8_t src  = (uint8_t)gate_hold_step;
        uint8_t gate = (uint8_t)((i - (int)src + 16) % 16);
        if (gate == 0) gate = 16;
        step_gate[pattern_value][src] = gate;
        do_step_on(src);  // ensure source step is on

        // Flash LEDs: light src through src+gate (inclusive) to show the range.
        for (int s = 0; s < 16; s++) {
          uint8_t dist = (uint8_t)((s - (int)src + 16) % 16);
          if (dist <= gate) step_leds[s].on();
          else              step_leds[s].off();
        }
        gate_flash_end_ms = millis() + 300;
        gate_gesture_fired = true;
#if FEATURE_NOTE_AUDITION
        if (ft_note_audition && !playstatus) audition_step_note(src, gate);
#endif

        Serial.print("gate set: step ");
        Serial.print(src);
        Serial.print(" gate=");
        Serial.println(gate);
      } else {
        // Two quick taps (< 150 ms): fire deferred toggle for held step, then
        // start tracking the new step as the next potential hold source.
        do_step_toggle(gate_hold_step);
        gate_hold_step = i;
        gate_hold_start_ms = millis();
        gate_gesture_fired = false;
        // Toggle for i is deferred until its release.
      }
    } else {
      // Fresh press (or gesture already fired — treat as new hold source).
      gate_hold_step = i;
      gate_hold_start_ms = millis();
      gate_gesture_fired = false;
      // Toggle deferred until release.
    }
  }
}

#if FEATURE_CC_MODE
// read_cc_step_memory()
//
// Sets step LEDs to reflect cc_step_enabled[] for the current pattern.
// Called when in CC slider mode (mode 4) instead of read_step_memory().
//
void read_cc_step_memory() {
  for (int i = 0; i < 16; i++) {
    if (cc_step_enabled[pattern_value][i]) step_leds[i].on();
    else                                   step_leds[i].off();
  }
}
#endif  // FEATURE_CC_MODE

// read_drift_step_memory()
//
// Sets step LEDs to reflect step_drift_enabled[] for the current pattern.
// Called when in D slider mode (mode 7) instead of read_step_memory().
//
void read_drift_step_memory() {
  for (int i = 0; i < 16; i++) {
    if (step_drift_enabled[pattern_value][i]) step_leds[i].on();
    else                                      step_leds[i].off();
  }
}

// read_chord_step_memory()
//
// Sets step LEDs to reflect "has chord" (step_chord_type[][] > 0) for the
// current pattern. Called when in CH slider mode (mode 8). A lit LED means
// the step has a chord type assigned; an unlit LED means single-note (type 0).
//
void read_chord_step_memory() {
  for (int i = 0; i < 16; i++) {
    if (step_chord_type[pattern_value][i] > 0) step_leds[i].on();
    else                                       step_leds[i].off();
  }
}

void read_step_memory(int voice, int pattern)
{
  for (int i = 0; i <= 15; i++)
  {
    int this_step = step_data[pattern][voice][i];

    if (this_step == 1)
    {
      step_leds[i].on();
    }
    else
    {
      step_leds[i].off();
    }
  }
}

void clear_step_leds()
{
  for (int i = 0; i <= 15; i++)
  {
    step_leds[i].off();
  }
}

void copy_step_data(int pattern_value, int voice, int step, int step_value)
{

  step_data[pattern_value][last_voice_selected][step] = step_value;
  
}

// For every pattern where all 16 steps are off, reset all pitches to
// slider_map_low_value. Called after note range changes and after reset sliders
// so blank patterns are immediately ready to use at the current low note.
void init_blank_patterns_to_range()
{
  for (int p = 0; p < 16; p++)
  {
    bool all_off = true;
    for (int i = 0; i < 16; i++)
    {
      if (step_data[p][0][i] != 0) { all_off = false; break; }
    }
    if (all_off)
    {
      for (int i = 0; i < 16; i++)
        pattern_step_pitches[p][i] = slider_map_low_value;
    }
  }
}

void clear_pattern_memory_for_voice(int voice)
{
  for (int i = 0; i <= 15; i++)
  {
    step_data[pattern_value][voice][i] = 0;
    pattern_step_pitches[pattern_value][i] = slider_map_low_value;
    pattern_step_velocities[pattern_value][i] = 127;
    step_gate[pattern_value][i] = 1;
#if FEATURE_CC_MODE
    cc_step_enabled[pattern_value][i] = 0;
    cc_step_values[pattern_value][i] = 0;
#endif
    step_probability[pattern_value][i] = 100;
    step_drift_enabled[pattern_value][i] = 0;
    step_drift_amount[pattern_value][i] = 0;
    step_chord_type[pattern_value][i] = 0;
    step_leds[i].off();
  }
  return;
}

void clear_pattern_memory()
{
  for (int p = 0; p < 16; p++)
  {
    for (int v = 0; v < 1; v++) // synthseqr configuration
    {
      for (int i = 0; i <= 15; i++)
      {
        step_data[p][v][i] = 0;
        pattern_step_pitches[p][i] = slider_map_low_value;
        pattern_step_velocities[p][i] = 127;
        step_gate[p][i] = 1;
#if FEATURE_CC_MODE
        cc_step_enabled[p][i] = 0;
        cc_step_values[p][i] = 0;
#endif
        step_probability[p][i] = 100;
        step_drift_enabled[p][i] = 0;
        step_drift_amount[p][i] = 0;
        step_chord_type[p][i] = 0;
      }
    }
  }
  // Refresh LEDs to reflect the cleared active pattern
  read_step_memory(0, pattern_value);
}