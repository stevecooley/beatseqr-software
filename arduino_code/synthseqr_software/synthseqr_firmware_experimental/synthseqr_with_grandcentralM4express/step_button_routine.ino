#if FEATURE_NOTE_AUDITION
// audition_cancel()
//
// Sends note-off for every currently-sounding audition pitch and clears the
// state. Safe to call when no audition is active (slots will already be -1).
//
void audition_cancel() {
  bool any = false;
  for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) {
    if (audition_sounding_chord[n] >= 0) {
      noteOff(MIDICHANNEL - 1, (uint8_t)audition_sounding_chord[n], 0);
      audition_sounding_chord[n] = -1;
      any = true;
    }
  }
  if (any) MidiUSB.flush();
}

// audition_step_note()
//
// Plays a brief preview for step i with gate_steps × 16th-note duration at
// the current tempo. Applies octave/note shift, scale quantization, and the
// step's assigned chord type — exactly as stepsend() would, minus pitch drift
// so the preview is deterministic. When step_chord_type[p][i] is 0 (single
// note) this collapses to one note-on, identical to the pre-chord behavior.
// Cancels any previously-sounding audition before sending the new chord.
// Only called when ft_note_audition is true and the sequencer is stopped.
//
void audition_step_note(int step, uint8_t gate_steps) {
  audition_cancel();

  // Compute root (shift + scale snap; matches stepsend's root computation).
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
  uint8_t root = (uint8_t)shifted;

  // Chord source: a captured MIDI keyboard chord wins over the standard
  // root + step_chord_type path — matches stepsend() so preview = playback.
  uint8_t chord_out[MAX_CHORD_NOTES];
  uint8_t chord_n = 0;
  if (step_custom_chord[pattern_value][step][0] >= 0) {
    for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) {
      int8_t pp = step_custom_chord[pattern_value][step][n];
      if (pp < 0) break;
      int16_t sp = (int16_t)pp;
      if (ft_octave_note_shift) {
        sp += (int16_t)(octave_shift * 12) + (int16_t)note_shift;
      }
      if (sp < 0)   sp = 0;
      if (sp > 127) sp = 127;
      if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0) {
        sp = (int16_t)quantize_to_scale((uint8_t)sp);
      }
      chord_out[chord_n++] = (uint8_t)sp;
    }
  } else {
    chord_n = build_chord_pitches(root, step_chord_type[pattern_value][step], chord_out);
  }

  uint8_t vel = voice_slider_midivelocity[step];
  unsigned long gate_ms = (unsigned long)((float)gate_steps * 60000.0f / TEMPO / 4.0f);
  if (gate_ms < 50) gate_ms = 50;

  for (uint8_t n = 0; n < chord_n; n++) {
    noteOn(MIDICHANNEL - 1, chord_out[n], vel);
    audition_sounding_chord[n] = (int8_t)chord_out[n];
  }
  MidiUSB.flush();
  audition_note_off_ms = millis() + gate_ms;
}
#endif  // FEATURE_NOTE_AUDITION

void run_step_button_routine()
{
#if FEATURE_NOTE_AUDITION
  // Send note-off for every sounding audition pitch once the gate elapses.
  // All notes of an auditioned chord share one off-time.
  if (audition_sounding_chord[0] >= 0 && (long)(millis() - audition_note_off_ms) >= 0) {
    audition_cancel();
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
          for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++)
            step_custom_chord[i][s][n] = step_custom_chord[current_pattern][s][n];
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


// Reset MIDI keyboard capture state (buffer, held mask, dirty flag, armed
// step). Called when discarding a partial capture (gate-set gesture wins,
// menu opens, etc.) and as part of the post-commit cleanup.
void midi_capture_discard() {
  for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) midi_capture_buf[n] = -1;
  for (uint8_t b = 0; b < 16; b++)              midi_capture_held[b] = 0;
  midi_capture_count          = 0;
  midi_capture_first_velocity = 127;
  midi_capture_dirty          = false;
  midi_capture_step           = -1;
}

// Arm MIDI keyboard capture for a step. If a DIFFERENT step already has a
// pending dirty capture, commit it first — typically happens when the user
// holds step A, plays a chord, then presses step B to define a gate range.
// Returns true if a commit ran, so the caller can suppress a same-loop
// deferred toggle that would otherwise undo the just-committed activation.
bool midi_capture_arm(int step) {
  bool committed = false;
  if (midi_capture_step >= 0 && midi_capture_step != (int8_t)step && midi_capture_dirty) {
    midi_capture_commit(midi_capture_step);
    committed = true;
  }
  midi_capture_discard();
  midi_capture_step = (int8_t)step;
  return committed;
}

// Commit the current capture buffer to step_custom_chord[][] and activate
// the step. Called when a step button is released and at least one MIDI
// note was captured. Promotes the lowest captured pitch into the slider
// pitch state so moving the NN slider afterwards erases the custom chord
// cleanly. Also resets step_chord_type to 0 so the custom-chord branch
// in stepsend() / audition_step_note() is the unambiguous winner.
void midi_capture_commit(int step) {
  if (midi_capture_count == 0) return;
  for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++) {
    step_custom_chord[pattern_value][step][n] =
        (n < midi_capture_count) ? midi_capture_buf[n] : -1;
  }
  uint8_t low = (uint8_t)midi_capture_buf[0];
  voice_slider_midinotenum[step]                = low;
  pattern_step_pitches[pattern_value][step]     = low;
  pattern_step_velocities[pattern_value][step]  = midi_capture_first_velocity;
  if (ft_chord_mode) step_chord_type[pattern_value][step] = 0;
  step_data[pattern_value][0][step] = 1;
  step_leds[step].on();
  seq.setNote(MIDICHANNEL - 1, voice_slider_midinotenum[step], 127, step);
  // Arm pickup for this slider — the physical NN slider is almost certainly
  // NOT at the captured low pitch, and without this the next slider read
  // would overwrite voice_slider_midinotenum (and trigger the custom-chord
  // eraser) within 20 ms, wiping the capture before it ever played.
  // For Catch mode: user must physically cross the captured pitch to engage.
  // For Jump mode:  user must move past JUMP_RAW_THRESHOLD to engage.
  // For Relative:   pickup is ignored; only an intentional value_delta clears.
  // Seed last_raw too so Relative mode doesn't compute a phantom delta from
  // any motion that happened during the capture session.
  slider_needs_pickup[step] = true;
  slider_last_raw[step]     = voice_sliders[step].getValue();

  // LCD feedback: case 204 reads these and shows "Cap S{s} N{n}" briefly.
  // Setting both lcdflag and next_lcdflag is required (LCD.ino syncs them
  // at the top of every frame, so a bare lcdflag write is overwritten).
  midi_capture_lcd_step  = (int8_t)step;
  midi_capture_lcd_count = midi_capture_count;
  lcdflag      = 204;
  next_lcdflag = 204;
  update_line2 = true;
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
    // so re-activating the step starts from a clean slate. Also discard
    // any captured MIDI chord since the step is being silenced.
    if (ft_chord_mode) step_chord_type[pattern_value][i] = 0;
    for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++)
      step_custom_chord[pattern_value][i][n] = -1;
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
  // gate_hold_step is a global (config.h) so run_voice_slider_routine() can
  // read which step is held to drive the strum gesture.
  static unsigned long gate_hold_start_ms = 0;
  static bool     gate_gesture_fired = false;
  static unsigned long gate_flash_end_ms  = 0;

  // Restore LEDs once the gate-flash window expires.
  if (gate_flash_end_ms != 0 && (long)(millis() - gate_flash_end_ms) >= 0) {
    gate_flash_end_ms = 0;
#if FEATURE_CC_MODE
    if (slider_mode == 4)      read_cc_step_memory();
    else if (slider_mode == 7) read_drift_step_memory();
    else                       read_step_memory(0, pattern_value);
#else
    if (slider_mode == 7)      read_drift_step_memory();
    else                       read_step_memory(0, pattern_value);
#endif
  }

  // MIDI keyboard capture release: if the armed step has been released and
  // notes were captured, commit them and suppress any deferred toggle that
  // would otherwise turn the step off (the commit already activates it).
  // Runs in every slider mode — capture is armed on press regardless of mode.
  if (midi_capture_step >= 0 && !step_buttons[midi_capture_step].wasPressed()) {
    int8_t captured_step = midi_capture_step;
    bool   committed     = midi_capture_dirty;
    if (committed) midi_capture_commit(captured_step);
    midi_capture_discard();
    if (committed && gate_hold_step == captured_step) {
      gate_hold_step     = -1;
      gate_gesture_fired = false;
      strum_fired        = false;
    }
  }

  // Release check: if the tracked hold step was released, fire its deferred
  // toggle unless a gate gesture or a strum already consumed this hold. A strum
  // already left the step ON at its final pitch, so toggling now would turn it
  // back off.
  if (gate_hold_step >= 0 && !step_buttons[gate_hold_step].wasPressed()) {
    if (!gate_gesture_fired && !strum_fired) {
      do_step_toggle(gate_hold_step);
    }
    gate_hold_step = -1;
    gate_gesture_fired = false;
    strum_fired = false;
  }

  for (int i = 0; i <= 15; i++) {
    if (!step_buttons[i].uniquePress()) continue;

    // Arm MIDI keyboard capture for this step in every slider mode. If a
    // pending capture on a DIFFERENT step gets committed here (gate-set
    // sequence: hold A, play chord, tap B), suppress this loop's double-tap
    // toggle below — the commit already activated that step.
    bool just_committed_other = false;
    if (ft_midi_program_mode) just_committed_other = midi_capture_arm(i);

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
        strum_fired = false;  // gate-set consumed the hold; a prior strum already left src on
        // Gate-set wins over MIDI capture — discard any partial capture so
        // the release check doesn't commit to the wrong step.
        midi_capture_discard();
#if FEATURE_NOTE_AUDITION
        if (ft_note_audition && !playstatus) audition_step_note(src, gate);
#endif

        Serial.print("gate set: step ");
        Serial.print(src);
        Serial.print(" gate=");
        Serial.println(gate);
      } else {
        // Two quick taps (< 150 ms): fire deferred toggle for held step,
        // unless a MIDI capture for that same step just committed at the top
        // of this loop iteration — committing already turned the step on,
        // and toggling here would turn it back off and wipe the custom chord.
        if (!just_committed_other) do_step_toggle(gate_hold_step);
        gate_hold_step = i;
        gate_hold_start_ms = millis();
        gate_gesture_fired = false;
        strum_fired = false;
        // Toggle for i is deferred until its release.
      }
    } else {
      // Fresh press (or gesture already fired — treat as new hold source).
      gate_hold_step = i;
      gate_hold_start_ms = millis();
      gate_gesture_fired = false;
      strum_fired = false;
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
    for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++)
      step_custom_chord[pattern_value][i][n] = -1;
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
        for (uint8_t n = 0; n < MAX_CHORD_NOTES; n++)
          step_custom_chord[p][i][n] = -1;
      }
    }
  }
  // Refresh LEDs to reflect the cleared active pattern
  read_step_memory(0, pattern_value);
}