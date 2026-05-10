#if FEATURE_SCALE_QUANTIZATION
// scales.ino
//
// Musical scale support for synthseqr.
//
// scale_type: 0=Chromatic 1=Major 2=NatMinor 3=PentMaj 4=PentMin
//             5=Dorian 6=Mixolydian 7=HarmMinor 8=Blues
// scale_root: 0=C 1=C# 2=D 3=D# 4=E 5=F 6=F# 7=G 8=G# 9=A 10=A# 11=B
//
// build_scale_notes()          — fills scale_note_pool[] from current settings
// quantize_to_scale(note)      — snaps a MIDI note to the nearest pool note
// apply_scale_to_all_patterns()— quantizes all stored pitches and re-arms pickups

// SCALE_COUNT is defined in config.h so config_menu.ino can see it.

// Intervals from root (semitones within one octave). 0xFF = end-of-list.
static const uint8_t SCALE_INTERVALS[SCALE_COUNT][13] = {
  {0,1,2,3,4,5,6,7,8,9,10,11,0xFF},  // 0 Chromatic
  {0,2,4,5,7,9,11,0xFF,0,0,0,0,0},   // 1 Major
  {0,2,3,5,7,8,10,0xFF,0,0,0,0,0},   // 2 Natural Minor
  {0,2,4,7,9,0xFF,0,0,0,0,0,0,0},    // 3 Pentatonic Major
  {0,3,5,7,10,0xFF,0,0,0,0,0,0,0},   // 4 Pentatonic Minor
  {0,2,3,5,7,9,10,0xFF,0,0,0,0,0},   // 5 Dorian
  {0,2,4,5,7,9,10,0xFF,0,0,0,0,0},   // 6 Mixolydian
  {0,2,3,5,7,8,11,0xFF,0,0,0,0,0},   // 7 Harmonic Minor
  {0,3,5,6,7,10,0xFF,0,0,0,0,0,0}    // 8 Blues
};

// LCD display names — max 10 chars to fit "  Sc: %-10s" (16 chars total on line 2)
const char* SCALE_NAMES[SCALE_COUNT] = {
  "Chromatic",
  "Major",
  "NatMinor",
  "PentMaj",
  "PentMin",
  "Dorian",
  "Mixolydian",
  "HarmMinor",
  "Blues"
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

// Return the nearest note in scale_note_pool to the given MIDI note.
// Ties (equal distance) resolve toward the lower note. Returns note unchanged
// if pool is empty.
uint8_t quantize_to_scale(uint8_t note) {
  if (scale_note_count == 0) return note;
  uint8_t best = scale_note_pool[0];
  uint8_t best_dist = (uint8_t)abs((int)note - (int)scale_note_pool[0]);
  for (uint8_t i = 1; i < scale_note_count; i++) {
    uint8_t dist = (uint8_t)abs((int)note - (int)scale_note_pool[i]);
    if (dist < best_dist) {
      best_dist = dist;
      best = scale_note_pool[i];
    }
  }
  return best;
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

#endif  // FEATURE_SCALE_QUANTIZATION
