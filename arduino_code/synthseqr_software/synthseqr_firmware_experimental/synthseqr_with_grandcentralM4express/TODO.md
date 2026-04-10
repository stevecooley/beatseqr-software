# Synthseqr TODO

Feature ideas and planned work. Not prioritized — just captured for future sessions.

---

- [ ] Pattern length — configurable steps per pattern (e.g. 8, 16)
- [ ] Pattern direction — forward, reverse, ping-pong, random
- [ ] Per-step note length — hold a step button and press another to set gate length; need to reassign or relocate current step 0+15 (clear pattern) and step 0+11 (clear all) combos to free up step-hold gestures for note length
- [ ] Enter as shift key — hold Enter + other buttons for secondary functions
- [x] Octave shifting — Config menu → Octave shift; d-pad up/down ±1 octave, range -5 to +5; applied at MIDI send time, saved to SD+EEPROM
- [x] Note ranges — Config menu → Note range; two-phase editor for low/high MIDI note bounds; defaults 36–52; saved to SD+EEPROM
- [ ] Randomize pattern — fill current pattern with random steps and/or pitches
- [ ] MIDI CC output — send CC messages from sliders or steps
- [ ] Flexible pattern chaining — select any subset of patterns to chain and loop (e.g. 3+4 only, not just 1→4)
- [x] Pattern buttons as function keys — hold button 0 = pattern select (step buttons 1–16); double-click button 0 = arm pattern copy, then step button = destination; buttons 1–3 reserved for future functions
- [ ] Slider mode UI — use a pattern button hold to cycle/select slider mode (note number, velocity, CC); show current mode on LCD
- [ ] Velocity mode — sliders set per-step note velocity (0–127) instead of pitch; store per-pattern per-step
- [ ] CC sequence mode — sliders set per-step MIDI CC values; select CC number to control; output on each step alongside or instead of notes
- [x] Config menu — double-tap Enter to enter; d-pad up/down scrolls; d-pad left always exits; items: Exit, Save, Clear pattern, Clear all, Reset sliders, Mode toggle, Octave shift, (Note shift/range/scales placeholders)
- [x] Simple / Advanced mode — Simple: 4 pattern buttons select patterns 1–4, chain toggle; Advanced: buttons are function keys, 16 patterns, hold button 0 = select, double-click button 0 = copy
- [ ] Scales — constrain slider note output to a selected musical scale (major, minor, pentatonic, etc); sliders snap to in-scale notes only
- [ ] Chord mode — each step triggers a chord (multiple notes) instead of a single note; define chord type per step or globally
