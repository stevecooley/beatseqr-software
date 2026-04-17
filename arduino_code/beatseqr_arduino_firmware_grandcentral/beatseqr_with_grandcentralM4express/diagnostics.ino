// diagnostics.ino — stub (Phase 5 will replace this with full implementation)
//
// Diagnostics mode provides a hardware self-test. It is entered by holding a
// hardware button combination for 1 second while the sequencer is stopped.
//
// TODO Phase 5: implement full diagnostics for Beatseqr hardware:
//   - Resistor-ladder voice select (display ADC reading + detected voice)
//   - 8 voice sliders (raw ADC, pin name, value)
//   - Tempo and swing knobs
//   - Step buttons (16) and LEDs
//   - Pattern select buttons (4)
//   - Play button, param_rec, slider_mode_select, voice_mode_select, knob_mode_select
//
// Entry gesture: hold knob_mode_select for 2 seconds (TBD in Phase 5).

bool diag_mode = false;

void enter_diagnostics() {
  // Stub — no-op until Phase 5.
  Serial.println("diagnostics not yet implemented");
}

void run_diagnostics() {
  // Stub — no-op until Phase 5.
}
