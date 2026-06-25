// clock_div.ino
//
// Global clock divide / multiply. The sequencer clock runs at 24 PPQN; one
// step normally advances every 6 pulses (a 1/16 note). clock_div selects an
// entry in the CLOCK_DIV[] table (config.h) whose pulses-per-step value is an
// exact musical division of the tempo. Because MIDI clock output stays at a
// correct 24 PPQN and gates are measured in steps, slower divisions ring
// chords for many beats while staying locked to tempo.
//
// Gated by ft_clock_div: when the feature is off, stepping is forced back to
// 6 pulses (1/16) so behavior matches a unit without the feature.

// apply_clock_div()
//
// Pushes the active pulses-per-step into both clock paths:
//   - seq.setPulsesPerStep() drives the internal TC4-timer stepping
//   - clock_pulses_per_step drives the external-MIDI-clock step boundary
// Call after boot-load, after a config-menu change, and after toggling the
// ft_clock_div feature flag.
void apply_clock_div() {
  if (clock_div >= CLOCK_DIV_COUNT) clock_div = CLOCK_DIV_DEFAULT;
  uint8_t pulses = ft_clock_div ? CLOCK_DIV[clock_div].pulses : 6;
  clock_pulses_per_step = pulses;
  seq.setPulsesPerStep(pulses);
}
