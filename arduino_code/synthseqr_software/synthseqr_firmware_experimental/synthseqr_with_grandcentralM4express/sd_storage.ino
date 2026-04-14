// sd_storage.ino
//
// SD card save/load for synthseqr.
//
// Primary storage: /synthseqr/autosave.json on the SD card.
// Fallback:        EEPROM (via storage.ino) if no SD card or file missing.
//
// Song file format (JSON, hand-parsed — no library dependency):
//
//   {
//     "version": 1,
//     "tempo": 120.00,
//     "swing": 0,
//     "midi_channel": 2,
//     "octave_shift": 0,
//     "note_shift": 0,
//     "note_range_low": 36,
//     "note_range_high": 52,
//     "chain_active": 0,
//     "chain_start": 0,
//     "chain_end": 3,
//     "advanced_mode": 0,
//     "patterns": [
//       {"steps":[1,0,...16 values],"pitches":[36,37,...16 values]},
//       ...up to 16 patterns
//     ]
//   }
//
// The parser is deliberately minimal: it scans for known keys and reads the
// value that follows, ignoring anything it doesn't recognise. This makes it
// forward-compatible with files that contain extra keys.
//
// Implementation note: helper functions use a module-level File handle (_f)
// rather than File& parameters. This avoids the Arduino build tool trying to
// auto-generate prototypes for functions with the File type before SD.h has
// been included, which causes "File was not declared in this scope" errors.

#define SD_AUTOSAVE_PATH "/synthseqr/autosave.json"
#define SD_FOLDER        "/synthseqr"

static bool sd_available = false;
static File _f;  // module-level handle used by all parser helpers

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

bool sd_init() {
  sd_available = SD.begin(SDCARD_SS_PIN);
  if (sd_available) {
    Serial.println("SD card init OK");
    if (!SD.exists(SD_FOLDER)) {
      SD.mkdir(SD_FOLDER);
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
// Returns true if found (position is just past last char of needle).
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

// ---------------------------------------------------------------------------
// Save to SD
// ---------------------------------------------------------------------------

bool save_to_sd() {
  if (!sd_available) return false;

  if (SD.exists(SD_AUTOSAVE_PATH)) SD.remove(SD_AUTOSAVE_PATH);
  _f = SD.open(SD_AUTOSAVE_PATH, FILE_WRITE);
  if (!_f) { Serial.println("SD: failed to open for write"); return false; }

  _f.println("{");
  _f.println("  \"version\": 1,");
  _f.print("  \"tempo\": ");         _f.print(TEMPO, 2);              _f.println(",");
  _f.print("  \"swing\": ");         _f.print(SWING);                 _f.println(",");
  _f.print("  \"midi_channel\": ");  _f.print(MIDICHANNEL);           _f.println(",");
  _f.print("  \"octave_shift\": ");  _f.print(octave_shift);          _f.println(",");
  _f.print("  \"note_shift\": ");      _f.print(note_shift);              _f.println(",");
  _f.print("  \"note_range_low\": ");  _f.print(slider_map_low_value);    _f.println(",");
  _f.print("  \"note_range_high\": "); _f.print(slider_map_high_value);   _f.println(",");
  _f.print("  \"chain_active\": ");  _f.print(extended_step_length_mode); _f.println(",");
  _f.print("  \"chain_start\": ");   _f.print(chain_start);           _f.println(",");
  _f.print("  \"chain_end\": ");     _f.print(chain_end);             _f.println(",");
  _f.print("  \"advanced_mode\": ");    _f.print(advanced_mode ? 1 : 0); _f.println(",");
  _f.print("  \"pattern_length\": ");   _f.print(pattern_length);        _f.println(",");
  _f.print("  \"pattern_direction\": "); _f.print(pattern_direction);    _f.println(",");
  _f.print("  \"scale_root\": ");        _f.print(scale_root);           _f.println(",");
  _f.print("  \"scale_type\": ");        _f.print(scale_type);           _f.println(",");
  _f.println("  \"patterns\": [");

  for (int p = 0; p < 16; p++) {
    _f.print("    {\"steps\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(step_data[p][0][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("],\"pitches\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(pattern_step_pitches[p][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("],\"velocities\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(pattern_step_velocities[p][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("],\"gates\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(step_gate[p][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("]}");
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
  if (!SD.exists(SD_AUTOSAVE_PATH)) {
    Serial.println("SD: no autosave found");
    return false;
  }

  _f = SD.open(SD_AUTOSAVE_PATH, FILE_READ);
  if (!_f) { Serial.println("SD: failed to open for read"); return false; }

  // Scan for each scalar key independently (seek back to start each time).
  _f.seek(0);
  if (sd_find("\"tempo\":")) {
    float t = sd_parse_number();
    if (t >= 10.0f && t <= 250.0f) TEMPO = t;
  }

  _f.seek(0);
  if (sd_find("\"swing\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 5) SWING = v;
  }

  _f.seek(0);
  if (sd_find("\"midi_channel\":")) {
    int v = (int)sd_parse_number();
    if (v >= 1 && v <= 16) MIDICHANNEL = v;
  }

  _f.seek(0);
  if (sd_find("\"octave_shift\":")) {
    int v = (int)sd_parse_number();
    if (v >= -5 && v <= 5) octave_shift = (int8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"note_shift\":")) {
    int v = (int)sd_parse_number();
    if (v >= -12 && v <= 12) note_shift = (int8_t)v;
  }

  {
    uint8_t lo = slider_map_low_value;
    uint8_t hi = slider_map_high_value;
    _f.seek(0);
    if (sd_find("\"note_range_low\":")) {
      int v = (int)sd_parse_number();
      if (v >= 0 && v <= 126) lo = (uint8_t)v;
    }
    _f.seek(0);
    if (sd_find("\"note_range_high\":")) {
      int v = (int)sd_parse_number();
      if (v >= 1 && v <= 127) hi = (uint8_t)v;
    }
    if (lo < hi) {
      slider_map_low_value = lo;
      slider_map_high_value = hi;
    }
  }

  _f.seek(0);
  if (sd_find("\"chain_active\":")) {
    extended_step_length_mode = (uint8_t)sd_parse_number() ? 1 : 0;
  }

  _f.seek(0);
  if (sd_find("\"chain_start\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 15) chain_start = (uint8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"chain_end\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 15) chain_end = (uint8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"advanced_mode\":")) {
    advanced_mode = (bool)sd_parse_number();
  }

  _f.seek(0);
  if (sd_find("\"pattern_length\":")) {
    int v = (int)sd_parse_number();
    if (v >= 1 && v <= 16) pattern_length = (uint8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"pattern_direction\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < PATTERN_DIRECTION_COUNT) pattern_direction = (uint8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"scale_root\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < 12) scale_root = (uint8_t)v;
  }

  _f.seek(0);
  if (sd_find("\"scale_type\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v < SCALE_COUNT) scale_type = (uint8_t)v;
  }

  // Parse patterns array — seek once to "patterns": then read sequentially.
  _f.seek(0);
  if (!sd_find("\"patterns\":")) {
    _f.close();
    Serial.println("SD: no patterns key");
    return false;
  }
  if (!sd_read_until('[')) { _f.close(); return false; }

  for (int p = 0; p < 16; p++) {
    if (!sd_read_until('{')) break;

    if (sd_find("\"steps\":")) {
      if (!sd_read_until('[')) break;
      for (int s = 0; s < 16; s++) {
        sd_skip_ws();
        int v = (int)sd_parse_number();
        step_data[p][0][s] = (v != 0) ? 1 : 0;
        sd_skip_ws();
        char c = (char)_f.peek();
        if (c == ',' || c == ']') _f.read();
      }
    }

    if (sd_find("\"pitches\":")) {
      if (!sd_read_until('[')) break;
      for (int s = 0; s < 16; s++) {
        sd_skip_ws();
        int v = (int)sd_parse_number();
        if (v >= 0 && v <= 127) pattern_step_pitches[p][s] = (uint8_t)v;
        sd_skip_ws();
        char c = (char)_f.peek();
        if (c == ',' || c == ']') _f.read();
      }
    }

    if (sd_find("\"velocities\":")) {
      if (!sd_read_until('[')) break;
      for (int s = 0; s < 16; s++) {
        sd_skip_ws();
        int v = (int)sd_parse_number();
        if (v >= 1 && v <= 127) pattern_step_velocities[p][s] = (uint8_t)v;
        sd_skip_ws();
        char c = (char)_f.peek();
        if (c == ',' || c == ']') _f.read();
      }
    }

    if (sd_find("\"gates\":")) {
      if (!sd_read_until('[')) break;
      for (int s = 0; s < 16; s++) {
        sd_skip_ws();
        int v = (int)sd_parse_number();
        if (v >= 1 && v <= 8) step_gate[p][s] = (uint8_t)v;
        sd_skip_ws();
        char c = (char)_f.peek();
        if (c == ',' || c == ']') _f.read();
      }
    }
  }

  _f.close();

  // Sync active voice arrays to loaded pattern's pitches and velocities.
  // Arm pickup so NN-mode sliders don't immediately overwrite stored pitches.
  for (int s = 0; s < 16; s++) {
    voice_slider_midinotenum[s] = pattern_step_pitches[current_pattern][s];
    voice_slider_midivelocity[s] = pattern_step_velocities[current_pattern][s];
    slider_needs_pickup[s] = true;
  }

  // Reset ping-pong state so playback always starts from the beginning.
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
