# Synthseqr TODO

Feature ideas and planned work. Not prioritized — just captured for future sessions.

---

- [ ] Pattern length — configurable steps per pattern (e.g. 8, 16)
- [ ] Pattern direction — forward, reverse, ping-pong, random
- [x] Per-step note length — implemented via GT slider mode; sliders set gate length 1–8 per step per pattern; stored in step_gate[pattern][step]
- [ ] Enter as shift key — hold Enter + other buttons for secondary functions
- [x] Octave shifting — Config menu → Octave shift; d-pad up/down ±1 octave, range -5 to +5; applied at MIDI send time, saved to SD+EEPROM
- [x] Note ranges — Config menu → Note range; two-phase editor for low/high MIDI note bounds; defaults 36–52; saved to SD+EEPROM
- [ ] Randomize pattern — fill current pattern with random steps and/or pitches
- [ ] MIDI CC output — send CC messages from sliders or steps
- [x] Flexible pattern chaining — select any subset of patterns to chain and loop (e.g. 3+4 only, not just 1→4)
- [x] Pattern buttons as function keys — hold button 0 = pattern select (step buttons 1–16); double-click button 0 = arm pattern copy, then step button = destination; buttons 1–3 reserved for future functions
- [x] Slider mode UI — enter button (simple mode) or pattern buttons 1/2/3 (advanced mode) select NN/VL/GT mode; LCD line 1 cols 14–15 show current mode icon
- [x] Velocity mode — sliders set per-step note velocity (0–127) in VL mode; stored in pattern_step_velocities[pattern][step]
- [ ] CC sequence mode — sliders set per-step MIDI CC values; select CC number to control; output on each step alongside or instead of notes
- [x] Config menu — double-tap Enter to enter; d-pad up/down scrolls; d-pad left always exits; items: Exit, Save, Clear pattern, Clear all, Reset sliders, Mode toggle, Octave shift, (Note shift/range/scales placeholders)
- [x] Simple / Advanced mode — Simple: 4 pattern buttons select patterns 1–4, chain toggle; Advanced: buttons are function keys, 16 patterns, hold button 0 = select, double-click button 0 = copy
- [ ] Scales — constrain slider note output to a selected musical scale (major, minor, pentatonic, etc); sliders snap to in-scale notes only
- [ ] Chord mode — each step triggers a chord (multiple notes) instead of a single note; define chord type per step or globally
