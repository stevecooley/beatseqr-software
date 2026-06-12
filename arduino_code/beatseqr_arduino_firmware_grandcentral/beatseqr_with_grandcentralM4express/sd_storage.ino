// sd_storage.ino
//
// SD card save/load for beatseqr.
//
// Primary storage: /beatseqr/autosave.json on the SD card.
// Fallback:        EEPROM (via storage.ino) if no SD card or file missing.
//
// Uses SdFat (Adafruit Fork) instead of the standard SD.h library.
// The standard SD.h is unreliable on SAMD51 — SdFat is the correct choice.
//
// JSON format — per-pattern structure uses flat per-voice arrays:
//
//   {
//     "version": 1,
//     "tempo": 120.00,
//     "swing": 0,
//     "midi_channel": 10,
//     "octave_shift": 0,
//     "note_shift": 0,
//     "note_range_low": 36,
//     "note_range_high": 51,
//     "chain_active": 0,
//     "chain_start": 0,
//     "chain_end": 3,
//     "advanced_mode": 0,
//     "pattern_length": 16,
//     "pattern_direction": 0,
//     "scale_root": 0,
//     "scale_type": 0,
//     "voice_cc_enabled": [0,0,0,0,0,0,0,0],
//     "patterns": [
//       {
//         "cc_number": 1,
//         "pitches":    [36,38,42,...8 values],
//         "velocities": [100,100,...8 values],
//         "gates":      [1,1,...8 values],
//         "cc_values":  [0,0,...8 values],
//         "probabilities": [100,100,...8 values],
//         "steps_v0": [1,0,0,...16 values],
//         "steps_v1": [...],
//         ...
//         "steps_v7": [...]
//       },
//       ...16 patterns
//     ]
//   }
//
// The parser is deliberately minimal: it scans for known keys and reads the
// value that follows, ignoring anything it doesn't recognise. Forward-compatible
// with extra keys added by external tools.
//
// Implementation note: helper functions use a module-level File handle (_f)
// rather than File& parameters — avoids Arduino build-tool prototype issues.

#define SD_AUTOSAVE_PATH "/beatseqr/autosave.json"
#define SD_FOLDER        "/beatseqr"

static bool    sd_available = false;
static SdFat32 _sd;   // SdFat Adafruit Fork — replaces the standard SD global
static File32  _f;    // module-level handle used by all parser helpers

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

bool sd_init() {
  sd_available = _sd.begin(SdSpiConfig(SDCARD_SS_PIN, DEDICATED_SPI, SD_SCK_MHZ(4), &SDCARD_SPI));
  if (sd_available) {
    Serial.println("SD card init OK");
    if (!_sd.exists(SD_FOLDER)) {
      _sd.mkdir(SD_FOLDER);
    }
  } else {
    Serial.println("SD card not found");
  }
  return sd_available;
}

bool sd_is_available() { return sd_available; }

// ---------------------------------------------------------------------------
// Minimal JSON parser helpers — all operate on module-level _f
// ---------------------------------------------------------------------------

static void sd_skip_ws() {
  while (_f.available()) {
    char c = (char)_f.peek();
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') _f.read();
    else break;
  }
}

static bool sd_read_until(char sentinel) {
  while (_f.available()) {
    if ((char)_f.read() == sentinel) return true;
  }
  return false;
}

static float sd_parse_number() {
  sd_skip_ws();
  float result = 0;
  float frac = 0;
  float frac_div = 1;
  bool negative = false;
  bool in_frac = false;
  bool started = false;

  while (_f.available()) {
    char c = (char)_f.peek();
    if (!started && c == '-') { negative = true; _f.read(); continue; }
    if (c >= '0' && c <= '9') {
      _f.read(); started = true;
      if (in_frac) { frac = frac * 10 + (c - '0'); frac_div *= 10; }
      else result = result * 10 + (c - '0');
    } else if (c == '.') { _f.read(); in_frac = true; }
    else break;
  }
  result += frac / frac_div;
  return negative ? -result : result;
}

// Scan forward looking for needle string.
static bool sd_find(const char *needle) {
  int len = strlen(needle);
  int matched = 0;
  while (_f.available()) {
    char c = (char)_f.read();
    if (c == needle[matched]) {
      matched++;
      if (matched == len) return true;
    } else {
      matched = (c == needle[0]) ? 1 : 0;
    }
  }
  return false;
}

// Scan forward at most max_bytes for needle. Used for optional per-pattern
// keys so a missing key doesn't consume the rest of the file.
static bool sd_find_bounded(const char *needle, int max_bytes) {
  int len = strlen(needle);
  int matched = 0;
  int read_bytes = 0;
  while (_f.available() && read_bytes < max_bytes) {
    char c = (char)_f.read();
    read_bytes++;
    if (c == needle[matched]) {
      matched++;
      if (matched == len) return true;
    } else {
      matched = (c == needle[0]) ? 1 : 0;
    }
  }
  return false;
}

// Read an integer array from an already-opened "[v0,v1,...]".
// Caller must have just read past the '['.
static void sd_read_int8_array(uint8_t *arr, int count, uint8_t lo, uint8_t hi) {
  for (int i = 0; i < count; i++) {
    sd_skip_ws();
    int v = (int)sd_parse_number();
    if (v >= (int)lo && v <= (int)hi) arr[i] = (uint8_t)v;
    sd_skip_ws();
    char c = (char)_f.peek();
    if (c == ',' || c == ']') _f.read();
  }
}

// ---------------------------------------------------------------------------
// Save to SD
// ---------------------------------------------------------------------------

bool save_to_sd() {
  if (!sd_available) return false;

  if (_f) _f.close();
  if (_sd.exists(SD_AUTOSAVE_PATH)) _sd.remove(SD_AUTOSAVE_PATH);
  _f = _sd.open(SD_AUTOSAVE_PATH, O_WRONLY | O_CREAT | O_TRUNC);
  if (!_f) { Serial.println("SD: failed to open for write"); return false; }

  _f.println("{");
  _f.println("  \"version\": 1,");
  _f.print("  \"tempo\": ");             _f.print(TEMPO, 2);                  _f.println(",");
  _f.print("  \"swing\": ");             _f.print(SWING);                     _f.println(",");
  _f.print("  \"midi_channel\": ");      _f.print(MIDICHANNEL);               _f.println(",");
  _f.print("  \"octave_shift\": ");      _f.print(octave_shift);              _f.println(",");
  _f.print("  \"note_shift\": ");        _f.print(note_shift);                _f.println(",");
  _f.print("  \"note_range_low\": ");    _f.print(slider_map_low_value);      _f.println(",");
  _f.print("  \"note_range_high\": ");   _f.print(slider_map_high_value);     _f.println(",");
  _f.print("  \"chain_active\": ");      _f.print(extended_step_length_mode); _f.println(",");
  _f.print("  \"chain_start\": ");       _f.print(chain_start);               _f.println(",");
  _f.print("  \"chain_end\": ");         _f.print(chain_end);                 _f.println(",");
  _f.print("  \"advanced_mode\": ");     _f.print(advanced_mode ? 1 : 0);     _f.println(",");
  _f.print("  \"pattern_length\": ");    _f.print(pattern_length);            _f.println(",");
  _f.print("  \"pattern_direction\": "); _f.print(pattern_direction);         _f.println(",");
  _f.print("  \"scale_root\": ");        _f.print(scale_root);                _f.println(",");
  _f.print("  \"scale_type\": ");        _f.print(scale_type);                _f.println(",");
  _f.print("  \"slider_takeover\": ");   _f.print(slider_takeover);           _f.println(",");
  _f.print("  \"swing_knob_function\": "); _f.print(swing_knob_function);       _f.println(",");

  // Feature flags (all optional on load — default to ON / Catch when absent).
  _f.print("  \"ft_advanced_mode\": ");      _f.print(ft_advanced_mode ? 1 : 0);       _f.println(",");
  _f.print("  \"ft_cc_mode\": ");            _f.print(ft_cc_mode ? 1 : 0);             _f.println(",");
  _f.print("  \"ft_probability\": ");        _f.print(ft_probability ? 1 : 0);         _f.println(",");
  _f.print("  \"ft_gate_mode\": ");          _f.print(ft_gate_mode ? 1 : 0);           _f.println(",");
  _f.print("  \"ft_velocity_mode\": ");      _f.print(ft_velocity_mode ? 1 : 0);       _f.println(",");
  _f.print("  \"ft_scale_quantization\": "); _f.print(ft_scale_quantization ? 1 : 0);  _f.println(",");
  _f.print("  \"ft_pattern_direction\": ");  _f.print(ft_pattern_direction ? 1 : 0);   _f.println(",");
  _f.print("  \"ft_variable_pat_length\": ");_f.print(ft_variable_pat_length ? 1 : 0); _f.println(",");
  _f.print("  \"ft_external_clock\": ");     _f.print(ft_external_clock ? 1 : 0);      _f.println(",");
  _f.print("  \"ft_octave_note_shift\": ");  _f.print(ft_octave_note_shift ? 1 : 0);   _f.println(",");
  _f.print("  \"ft_diagnostics\": ");        _f.print(ft_diagnostics ? 1 : 0);         _f.println(",");
  _f.print("  \"ft_voice_sliders\": ");      _f.print(ft_voice_sliders ? 1 : 0);       _f.println(",");

  // voice_cc_enabled is per-voice (not per-pattern) — save once at top level.
  _f.print("  \"voice_cc_enabled\": [");
  for (int v = 0; v < VOICE_COUNT; v++) {
    _f.print(voice_cc_enabled[v]);
    if (v < VOICE_COUNT - 1) _f.print(",");
  }
  _f.println("],");

  _f.println("  \"patterns\": [");

  for (int p = 0; p < 16; p++) {
    _f.println("    {");
    _f.print("      \"cc_number\": "); _f.print(cc_number[p]); _f.println(",");

    _f.print("      \"pitches\": [");
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print(voice_pitch[p][v]);
      if (v < VOICE_COUNT - 1) _f.print(",");
    }
    _f.println("],");

    _f.print("      \"velocities\": [");
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print(voice_velocity[p][v]);
      if (v < VOICE_COUNT - 1) _f.print(",");
    }
    _f.println("],");

    _f.print("      \"gates\": [");
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print(voice_gate[p][v]);
      if (v < VOICE_COUNT - 1) _f.print(",");
    }
    _f.println("],");

    _f.print("      \"cc_values\": [");
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print(voice_cc_value[p][v]);
      if (v < VOICE_COUNT - 1) _f.print(",");
    }
    _f.println("],");

    _f.print("      \"probabilities\": [");
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print(voice_probability[p][v]);
      if (v < VOICE_COUNT - 1) _f.print(",");
    }
    _f.println("],");

    // Per-voice step arrays — step_data[pattern][voice][step].
    for (int v = 0; v < VOICE_COUNT; v++) {
      _f.print("      \"steps_v");
      _f.print(v);
      _f.print("\": [");
      for (int s = 0; s < 16; s++) {
        _f.print(step_data[p][v][s]);
        if (s < 15) _f.print(",");
      }
      if (v < VOICE_COUNT - 1) _f.println("],");
      else                       _f.println("]");
    }

    _f.print("    }");
    if (p < 15) _f.print(",");
    _f.println();
  }

  _f.println("  ]");
  _f.println("}");
  _f.close();

  Serial.println("saved to SD");
  return true;
}

// ---------------------------------------------------------------------------
// Load from SD
// ---------------------------------------------------------------------------

bool load_from_sd() {
  if (!sd_available) return false;
  if (!_sd.exists(SD_AUTOSAVE_PATH)) {
    Serial.println("SD: no autosave found");
    return false;
  }

  _f = _sd.open(SD_AUTOSAVE_PATH, O_RDONLY);
  if (!_f) { Serial.println("SD: failed to open for read"); return false; }

  // Scalar fields — seek back to start for each one.
  _f.seekSet(0);
  if (sd_find("\"tempo\":")) {
    float t = sd_parse_number();
    if (t >= 30.0f && t <= 250.0f) TEMPO = t;
  }

  _f.seekSet(0);
  if (sd_find("\"swing\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 5) SWING = v;
  }

  _f.seekSet(0);
  if (sd_find("\"midi_channel\":")) {
    int v = (int)sd_parse_number();
    if (v >= 1 && v <= 16) MIDICHANNEL = v;
  }

  _f.seekSet(0);
  if (sd_find("\"octave_shift\":")) {
    int v = (int)sd_parse_number();
    if (v >= -5 && v <= 5) octave_shift = (int8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"note_shift\":")) {
    int v = (int)sd_parse_number();
    if (v >= -12 && v <= 12) note_shift = (int8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"slider_takeover\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 2) slider_takeover = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"swing_knob_function\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < SWING_KNOB_FN_COUNT) swing_knob_function = (uint8_t)v;
  }

  // Feature flags — optional; absent keys keep the compiled-in defaults (ON).
  _f.seekSet(0); if (sd_find("\"ft_advanced_mode\":"))      ft_advanced_mode       = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_cc_mode\":"))            ft_cc_mode             = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_probability\":"))        ft_probability         = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_gate_mode\":"))          ft_gate_mode           = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_velocity_mode\":"))      ft_velocity_mode       = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_scale_quantization\":")) ft_scale_quantization  = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_pattern_direction\":"))  ft_pattern_direction   = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_variable_pat_length\":"))ft_variable_pat_length = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_external_clock\":"))     ft_external_clock      = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_octave_note_shift\":"))  ft_octave_note_shift   = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_diagnostics\":"))        ft_diagnostics         = ((int)sd_parse_number() != 0);
  _f.seekSet(0); if (sd_find("\"ft_voice_sliders\":"))      ft_voice_sliders       = ((int)sd_parse_number() != 0);

  {
    uint8_t lo = slider_map_low_value;
    uint8_t hi = slider_map_high_value;
    _f.seekSet(0);
    if (sd_find("\"note_range_low\":")) {
      int v = (int)sd_parse_number();
      if (v >= 0 && v <= 126) lo = (uint8_t)v;
    }
    _f.seekSet(0);
    if (sd_find("\"note_range_high\":")) {
      int v = (int)sd_parse_number();
      if (v >= 1 && v <= 127) hi = (uint8_t)v;
    }
    if (lo < hi) {
      slider_map_low_value  = lo;
      slider_map_high_value = hi;
    }
  }

  _f.seekSet(0);
  if (sd_find("\"chain_active\":")) {
    extended_step_length_mode = (uint8_t)sd_parse_number() ? 1 : 0;
  }

  _f.seekSet(0);
  if (sd_find("\"chain_start\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 15) chain_start = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"chain_end\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 15) chain_end = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"advanced_mode\":")) {
    int v = (int)sd_parse_number();
    if (v == 0 || v == 1) advanced_mode = (bool)v;
  }

  _f.seekSet(0);
  if (sd_find("\"pattern_length\":")) {
    int v = (int)sd_parse_number();
    if (v >= 1 && v <= 16) pattern_length = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"pattern_direction\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < PATTERN_DIRECTION_COUNT) pattern_direction = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"scale_root\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < 12) scale_root = (uint8_t)v;
  }

  _f.seekSet(0);
  if (sd_find("\"scale_type\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < SCALE_COUNT) scale_type = (uint8_t)v;
  }

  // voice_cc_enabled — top-level array.
  _f.seekSet(0);
  if (sd_find("\"voice_cc_enabled\":")) {
    if (sd_read_until('[')) {
      sd_read_int8_array(voice_cc_enabled, VOICE_COUNT, 0, 1);
    }
  }

  // Patterns array — seek once to "patterns": then read sequentially.
  _f.seekSet(0);
  if (!sd_find("\"patterns\":")) {
    _f.close();
    Serial.println("SD: no patterns key");
    return false;
  }
  if (!sd_read_until('[')) { _f.close(); return false; }

  for (int p = 0; p < 16; p++) {
    if (!sd_read_until('{')) break;

    // cc_number — optional, use bounded search.
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"cc_number\":", 200)) {
        int v = (int)sd_parse_number();
        if (v >= 1 && v <= 119 && v != 32 && !(v >= 96 && v <= 101))
          cc_number[p] = (uint8_t)v;
      } else { _f.seekSet(pos); }
    }

    // Per-voice scalar arrays: pitches, velocities, gates, cc_values, probabilities.
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"pitches\":", 300)) {
        if (sd_read_until('['))
          sd_read_int8_array(voice_pitch[p], VOICE_COUNT, 0, 127);
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"velocities\":", 300)) {
        if (sd_read_until('['))
          sd_read_int8_array(voice_velocity[p], VOICE_COUNT, 0, 127);
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"gates\":", 300)) {
        if (sd_read_until('['))
          sd_read_int8_array(voice_gate[p], VOICE_COUNT, 1, 8);
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"cc_values\":", 300)) {
        if (sd_read_until('['))
          sd_read_int8_array(voice_cc_value[p], VOICE_COUNT, 0, 127);
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"probabilities\":", 300)) {
        if (sd_read_until('['))
          sd_read_int8_array(voice_probability[p], VOICE_COUNT, 0, 100);
      } else { _f.seekSet(pos); }
    }

    // Per-voice step arrays: steps_v0 through steps_v7.
    for (int v = 0; v < VOICE_COUNT; v++) {
      char key[12];
      snprintf(key, sizeof(key), "\"steps_v%d\":", v);
      uint32_t pos = _f.position();
      if (sd_find_bounded(key, 400)) {
        if (sd_read_until('[')) {
          for (int s = 0; s < 16; s++) {
            sd_skip_ws();
            int val = (int)sd_parse_number();
            step_data[p][v][s] = (val != 0) ? 1 : 0;
            sd_skip_ws();
            char c = (char)_f.peek();
            if (c == ',' || c == ']') _f.read();
          }
        }
      } else { _f.seekSet(pos); }
    }
  }

  _f.close();

  // Arm pickup guards for all voices so sliders don't overwrite loaded values.
  for (int v = 0; v < VOICE_COUNT; v++)
    slider_needs_pickup[v] = true;

  // Reset ping-pong so playback starts from the beginning.
  ping_pong_step = 0;
  ping_pong_going_forward = true;

  // Rebuild scale note pool from loaded settings.
  build_scale_notes();

  Serial.println("loaded from SD");
  return true;
}

// ---------------------------------------------------------------------------
// Boot load: try SD first, fall back to EEPROM.
// ---------------------------------------------------------------------------

void boot_load() {
  sd_init();
  if (load_from_sd()) return;
  load_from_eeprom();
}

// ---------------------------------------------------------------------------
// Save everywhere: SD primary + EEPROM backup.
// ---------------------------------------------------------------------------

void save_everywhere() {
  save_to_sd();
  save_to_eeprom();
}

// ---------------------------------------------------------------------------
// Diagnostics save-file viewer support.
//
// Populates l1[i] (field name) and l2[i] (value string) for the first
// SD_DIAG_FIELD_COUNT top-level scalar fields of autosave.json. Each field is
// found by rewinding and scanning for its quoted key, so order in the file
// doesn't matter. Missing keys show "(absent)"; no card/file shows "(no SD)".
// ---------------------------------------------------------------------------

void sd_diag_load_fields(char l1[][17], char l2[][17]) {
  static const char* keys[SD_DIAG_FIELD_COUNT] = {
    "version", "tempo", "swing", "midi_channel",
    "octave_shift", "note_shift", "note_range_low", "note_range_high",
    "scale_root", "scale_type", "advanced_mode", "pattern_length",
    "pattern_direction", "chain_active", "chain_start", "chain_end",
    "slider_takeover", "swing_knob_function", "ft_cc_mode"
  };

  for (int i = 0; i < SD_DIAG_FIELD_COUNT; i++) {
    snprintf(l1[i], 17, "%s", keys[i]);
    snprintf(l2[i], 17, "(no SD)");
  }

  if (!sd_available) return;
  _f = _sd.open(SD_AUTOSAVE_PATH, O_RDONLY);
  if (!_f) return;

  for (int i = 0; i < SD_DIAG_FIELD_COUNT; i++) {
    _f.seekSet(0);
    char qkey[24];
    snprintf(qkey, sizeof(qkey), "\"%s\"", keys[i]);
    if (sd_find(qkey) && sd_read_until(':')) {
      float v = sd_parse_number();
      if (strcmp(keys[i], "tempo") == 0) snprintf(l2[i], 17, "%.1f", (double)v);
      else                               snprintf(l2[i], 17, "%d", (int)v);
    } else {
      snprintf(l2[i], 17, "(absent)");
    }
  }
  _f.close();
}
