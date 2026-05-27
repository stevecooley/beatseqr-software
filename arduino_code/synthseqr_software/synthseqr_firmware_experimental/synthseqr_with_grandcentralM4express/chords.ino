// chords.ino
//
// Chord engine for "chord mode". Each entry in CHORDS[] defines a chord as
// a set of semitone offsets from the root. Index 0 is reserved for "single
// note" (no chord) so step_chord_type[][] can default to 0 = pre-chord-mode
// behavior.
//
// build_chord_pitches(root, chord_type, out) fills out[] with up to
// MAX_CHORD_NOTES MIDI pitches and returns the count. Out-of-range notes are
// clamped to the MIDI range [0, 127]. (We deliberately do NOT clamp to
// [slider_map_low_value, slider_map_high_value] — that range governs slider
// sweep, not playback. With octave/note shift in effect, the root can sit
// well above slider_map_high_value, and chord intervals must be free to
// extend from there or the cluster collapses to a single note at the cap.)
// Duplicates that collapse after clamping are removed. Scale snap is applied
// when the active scale is non-Chromatic so chords stay in key — each
// interval-derived note is mapped to the nearest scale tone via quantize_to_scale().

// ChordDef struct is declared in config.h so other .ino files can read the
// CHORDS[] table for display and bounds checks.

const ChordDef CHORDS[] = {
  {"---",  1, {0,   0,  0,  0,  0,  0}},  // 0 = single note (root only)
  {"Maj",  3, {0,   4,  7,  0,  0,  0}},
  {"Min",  3, {0,   3,  7,  0,  0,  0}},
  {"Sus2", 3, {0,   2,  7,  0,  0,  0}},
  {"Sus4", 3, {0,   5,  7,  0,  0,  0}},
  {"Maj7", 4, {0,   4,  7, 11,  0,  0}},
  {"Min7", 4, {0,   3,  7, 10,  0,  0}},
  {"Dom7", 4, {0,   4,  7, 10,  0,  0}},
  {"Dim",  3, {0,   3,  6,  0,  0,  0}},
  {"Pow5", 2, {0,   7,  0,  0,  0,  0}},
  {"Oct",  2, {0,  12,  0,  0,  0,  0}},
};
const uint8_t CHORD_COUNT = sizeof(CHORDS) / sizeof(CHORDS[0]);

// Build the chord pitches for a given root note and chord type.
// Returns the number of notes written to out[] (1..MAX_CHORD_NOTES).
// chord_type 0 always returns a single note (the root) — keeps single-note
// playback identical to pre-chord-mode behavior when step_chord_type defaults.
uint8_t build_chord_pitches(uint8_t root, uint8_t chord_type, uint8_t out[MAX_CHORD_NOTES]) {
  if (chord_type >= CHORD_COUNT) chord_type = 0;
  const ChordDef& def = CHORDS[chord_type];

  uint8_t count = 0;
  for (uint8_t i = 0; i < def.count && count < MAX_CHORD_NOTES; i++) {
    int16_t n = (int16_t)root + (int16_t)def.iv[i];
    if (n < 0)   n = 0;
    if (n > 127) n = 127;
    uint8_t note = (uint8_t)n;
    if (ft_scale_quantization && scale_type != 0 && scale_note_count > 0) {
      note = quantize_to_scale(note);
    }
    // Dedupe: skip if this exact pitch is already in out[].
    bool dup = false;
    for (uint8_t j = 0; j < count; j++) {
      if (out[j] == note) { dup = true; break; }
    }
    if (!dup) out[count++] = note;
  }
  if (count == 0) {
    // Defensive: never return zero notes — fall back to the (clamped) root.
    int16_t n = (int16_t)root;
    if (n < 0)   n = 0;
    if (n > 127) n = 127;
    out[0] = (uint8_t)n;
    count = 1;
  }
  return count;
}
