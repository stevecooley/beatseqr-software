// scales.ino
//
// Musical scale support for synthseqr.
//
// scale_type: 0=Chromatic 1=Blues 2=Dorian 3=HarmMinor 4=Major
//             5=Mixolydian 6=NatMinor 7=PentMaj 8=PentMin 9=Phrygian
// scale_root: 0=C 1=C# 2=D 3=D# 4=E 5=F 6=F# 7=G 8=G# 9=A 10=A# 11=B
//
// build_scale_notes()          — fills scale_note_pool[] from current settings
// quantize_to_scale(note)      — snaps a MIDI note to the nearest pool note
// apply_scale_to_all_patterns()— quantizes all stored pitches and re-arms pickups

// SCALE_COUNT is defined in config.h so config_menu.ino can see it.

// Intervals from root (semitones within one octave). 0xFF = end-of-list.
static const uint8_t SCALE_INTERVALS[SCALE_COUNT][13] = {
  {0,1,2,3,4,5,6,7,8,9,10,11,0xFF},  // 0 Chromatic
  {0,3,5,6,7,10,0xFF,0,0,0,0,0,0},   // 1 Blues
  {0,2,3,5,7,9,10,0xFF,0,0,0,0,0},   // 2 Dorian
  {0,2,3,5,7,8,11,0xFF,0,0,0,0,0},   // 3 Harmonic Minor
  {0,2,4,5,7,9,11,0xFF,0,0,0,0,0},   // 4 Major
  {0,2,4,5,7,9,10,0xFF,0,0,0,0,0},   // 5 Mixolydian
  {0,2,3,5,7,8,10,0xFF,0,0,0,0,0},   // 6 Natural Minor
  {0,2,4,7,9,0xFF,0,0,0,0,0,0,0},    // 7 Pentatonic Major
  {0,3,5,7,10,0xFF,0,0,0,0,0,0,0},   // 8 Pentatonic Minor
  {0,1,3,5,7,8,10,0xFF,0,0,0,0,0}    // 9 Phrygian
};

// LCD display names — max 10 chars to fit "  Sc: %-10s" (16 chars total on line 2)
const char* SCALE_NAMES[SCALE_COUNT] = {
  "Chromatic",
  "Blues",
  "Dorian",
  "HarmMinor",
  "Major",
  "Mixolydian",
  "NatMinor",
  "PentMaj",
  "PentMin",
  "Phrygian"
};

// Root note names — max 2 chars each
const char* ROOT_NAMES[12] = {
  "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
};

// Rebuild scale_note_pool[] from current scale_type, scale_root, and note range.
// Called after any of these change, and on boot after loading from SD/EEPROM.
void build_scale_notes() {
  scale_note_count = 0;
  for (uint8_t n = slider_map_low_value; n <= slider_map_high_value && n < 128; n++) {
    uint8_t degree = (uint8_t)((n + 120 - scale_root) % 12);
    for (uint8_t i = 0; i < 13; i++) {
      if (SCALE_INTERVALS[scale_type][i] == 0xFF) break;
      if (SCALE_INTERVALS[scale_type][i] == degree) {
        scale_note_pool[scale_note_count++] = n;
        break;
      }
    }
  }
}

// True if a MIDI note's pitch class belongs to the active scale. Octave-
// independent — does NOT consider the note range, so it is valid across the
// full 0–127 range. Chromatic (scale_type 0) admits every note.
bool note_in_scale(uint8_t note) {
  if (scale_type == 0) return true;
  uint8_t degree = (uint8_t)((note + 120 - scale_root) % 12);
  for (uint8_t i = 0; i < 13; i++) {
    if (SCALE_INTERVALS[scale_type][i] == 0xFF) break;
    if (SCALE_INTERVALS[scale_type][i] == degree) return true;
  }
  return false;
}

// Snap a MIDI note to the nearest in-scale note across the FULL 0–127 range
// (pitch-class membership, octave-independent). Unlike scale_note_pool[], this
// is NOT limited to the note range — so octave/note shift applied at playback
// time can place notes in any octave and still be quantized in key. Ties
// (equal distance up vs down) resolve toward the lower note.
uint8_t quantize_to_scale(uint8_t note) {
  if (scale_type == 0) return note;  // Chromatic: no snap
  for (int d = 0; d <= 127; d++) {
    int dn = (int)note - d;
    if (dn >= 0   && note_in_scale((uint8_t)dn)) return (uint8_t)dn;
    int up = (int)note + d;
    if (up <= 127 && note_in_scale((uint8_t)up)) return (uint8_t)up;
  }
  return note;
}

// Rebuild scale pool and re-arm pickup guards. Stored pitches are NOT modified —
// scale quantization is applied at playback time in stepsend() so the original
// note data is always preserved regardless of scale changes.
void apply_scale_to_all_patterns() {
  build_scale_notes();
  for (uint8_t s = 0; s < 16; s++) {
    slider_needs_pickup[s] = true;
  }
}

