# Synthseqr TODO

Feature ideas and planned work. Not prioritized — just captured for future sessions.

---

- [ ] Pattern length — configurable steps per pattern (e.g. 8, 16)
- [ ] Pattern direction — forward, reverse, ping-pong, random
- [ ] Per-step note length — hold a step button and press another to set gate length; need to reassign or relocate current step 0+15 (clear pattern) and step 0+11 (clear all) combos to free up step-hold gestures for note length
- [ ] Enter as shift key — hold Enter + other buttons for secondary functions
- [ ] Octave shifting — figure out UI and implementation for shifting pattern up/down by octave
- [ ] Note ranges — user-adjustable slider note range (low/high bounds)
- [ ] Randomize pattern — fill current pattern with random steps and/or pitches
- [ ] MIDI CC output — send CC messages from sliders or steps
- [ ] Flexible pattern chaining — select any subset of patterns to chain and loop (e.g. 3+4 only, not just 1→4)
- [ ] Pattern buttons as function keys — hold pattern button 1 = pattern select mode, step buttons choose from 16 patterns; explore other pattern button holds for additional functions
- [ ] Slider mode UI — use a pattern button hold to cycle/select slider mode (note number, velocity, CC); show current mode on LCD
- [ ] Velocity mode — sliders set per-step note velocity (0–127) instead of pitch; store per-pattern per-step
- [ ] CC sequence mode — sliders set per-step MIDI CC values; select CC number to control; output on each step alongside or instead of notes
- [ ] Config menu — double-tap Enter to enter; d-pad up/down scrolls; d-pad left always exits; menu order:
  1. Exit (Enter/left/right all exit)
  2. Clear pattern (Enter to select, then Enter=confirm / left=cancel)
  3. Clear all patterns (same confirmation)
  4. Reset sliders (same confirmation)
  5. Mode: Simple / Advanced (Enter toggles immediately, LCD updates)
  - Moves clear pattern/clear all out of step button combos, freeing 0+15 and 0+11 for note length
- [ ] Simple / Advanced mode — Simple: 4 pattern buttons as today; Advanced: pattern buttons as function keys, 16 patterns, note length, slider modes, etc
- [ ] Scales — constrain slider note output to a selected musical scale (major, minor, pentatonic, etc); sliders snap to in-scale notes only
- [ ] Chord mode — each step triggers a chord (multiple notes) instead of a single note; define chord type per step or globally
