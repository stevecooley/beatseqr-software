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

#define SD_AUTOSAVE_PATH "/autosave.json"

static bool sd_available = false;
static SdFat32 _sd;   // SdFat Adafruit Fork — replaces the standard SD global
static File32  _f;    // module-level handle used by all parser helpers

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

bool sd_init() {
  sd_available = _sd.begin(SdSpiConfig(SDCARD_SS_PIN, DEDICATED_SPI, SD_SCK_MHZ(4), &SDCARD_SPI));
  if (sd_available) {
    Serial.println("SD card init OK");
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

// Scan forward at most max_bytes for needle. Used for optional per-pattern keys
// so a missing key doesn't consume the rest of the file.
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

// ---------------------------------------------------------------------------
// Save to SD
// ---------------------------------------------------------------------------

bool save_to_sd() {
  if (!sd_available) {
    Serial.println("SD: not available");
    return false;
  }

  if (_f) _f.close();
  if (_sd.exists(SD_AUTOSAVE_PATH)) _sd.remove(SD_AUTOSAVE_PATH);
  _f = _sd.open(SD_AUTOSAVE_PATH, O_WRONLY | O_CREAT | O_TRUNC);
  if (!_f) {
    Serial.println("SD: open for write failed");
    return false;
  }

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
  // advanced_mode is intentionally not saved — always boots Simple.
  _f.print("  \"pattern_length\": ");   _f.print(pattern_length);        _f.println(",");
  _f.print("  \"pattern_direction\": "); _f.print(pattern_direction);    _f.println(",");
  _f.print("  \"scale_root\": ");        _f.print(scale_root);           _f.println(",");
  _f.print("  \"scale_type\": ");        _f.print(scale_type);           _f.println(",");
  _f.print("  \"pitch_drift\": ");       _f.print(pitch_drift);          _f.println(",");
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
    _f.print("],\"cc_number\":");
    _f.print(cc_number[p]);
    _f.print(",\"cc_enabled\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(cc_step_enabled[p][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("],\"cc_values\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(cc_step_values[p][s]);
      if (s < 15) _f.print(",");
    }
    _f.print("],\"probabilities\":[");
    for (int s = 0; s < 16; s++) {
      _f.print(step_probability[p][s]);
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
  if (!_sd.exists(SD_AUTOSAVE_PATH)) {
    Serial.println("SD: no autosave found");
    return false;
  }

  _f = _sd.open(SD_AUTOSAVE_PATH, O_RDONLY);
  if (!_f) { Serial.println("SD: failed to open for read"); return false; }

  // Scan for each scalar key independently (seek back to start each time).
  _f.seekSet(0);
  if (sd_find("\"tempo\":")) {
    float t = sd_parse_number();
    if (t >= 10.0f && t <= 250.0f) TEMPO = t;
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
      slider_map_low_value = lo;
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

  // advanced_mode is intentionally not loaded — always boots Simple.

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

  _f.seekSet(0);
  if (sd_find("\"pitch_drift\":")) {
    int v = (int)sd_parse_number();
    if (v >= 0 && v <= 7) pitch_drift = (uint8_t)v;
  }

  // Parse patterns array — seek once to "patterns": then read sequentially.
  _f.seekSet(0);
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

    // CC fields — optional (new files only). Use bounded search so missing
    // keys in old files don't consume content from subsequent patterns.
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"cc_number\":", 400)) {
        int v = (int)sd_parse_number();
        if (v >= 1 && v <= 119 && v != 32 && !(v >= 96 && v <= 101))
          cc_number[p] = (uint8_t)v;
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"cc_enabled\":", 400)) {
        if (sd_read_until('[')) {
          for (int s = 0; s < 16; s++) {
            sd_skip_ws();
            int v = (int)sd_parse_number();
            cc_step_enabled[p][s] = (v != 0) ? 1 : 0;
            sd_skip_ws();
            char c = (char)_f.peek();
            if (c == ',' || c == ']') _f.read();
          }
        }
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"cc_values\":", 400)) {
        if (sd_read_until('[')) {
          for (int s = 0; s < 16; s++) {
            sd_skip_ws();
            int v = (int)sd_parse_number();
            if (v >= 0 && v <= 127) cc_step_values[p][s] = (uint8_t)v;
            sd_skip_ws();
            char c = (char)_f.peek();
            if (c == ',' || c == ']') _f.read();
          }
        }
      } else { _f.seekSet(pos); }
    }
    {
      uint32_t pos = _f.position();
      if (sd_find_bounded("\"probabilities\":", 400)) {
        if (sd_read_until('[')) {
          for (int s = 0; s < 16; s++) {
            sd_skip_ws();
            int v = (int)sd_parse_number();
            if (v >= 0 && v <= 100) step_probability[p][s] = (uint8_t)v;
            sd_skip_ws();
            char c = (char)_f.peek();
            if (c == ',' || c == ']') _f.read();
          }
        }
      } else { _f.seekSet(pos); }
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

// ---------------------------------------------------------------------------
// Diagnostics: read all scalar fields from the save file into display strings.
// Caller passes two 16-element arrays of 17-byte buffers.
// Each pair (l1[i], l2[i]) is a 16-char null-terminated label / value string.
// ---------------------------------------------------------------------------

static void _diag_pad(char* dst, const char* src) {
  snprintf(dst, 17, "%-16s", src);
}

void sd_diag_load_fields(char (*l1)[17], char (*l2)[17]) {
  char tmp[24];

  // Field 0: SD card availability
  _diag_pad(l1[0], "SD card");
  _diag_pad(l2[0], sd_available ? "available" : "NOT AVAILABLE");

  if (!sd_available) {
    for (int i = 1; i < SD_DIAG_FIELD_COUNT; i++) {
      _diag_pad(l1[i], "---");
      _diag_pad(l2[i], "(no SD card)");
    }
    return;
  }

  // Field 1: file presence
  _diag_pad(l1[1], "save file");
  bool exists = _sd.exists(SD_AUTOSAVE_PATH);
  _diag_pad(l2[1], exists ? "found" : "NOT FOUND");

  if (!exists) {
    for (int i = 2; i < SD_DIAG_FIELD_COUNT; i++) {
      _diag_pad(l1[i], "---");
      _diag_pad(l2[i], "(no save file)");
    }
    return;
  }

  if (_f) _f.close();
  _f = _sd.open(SD_AUTOSAVE_PATH, O_RDONLY);
  if (!_f) {
    for (int i = 2; i < SD_DIAG_FIELD_COUNT; i++) {
      _diag_pad(l1[i], "---");
      _diag_pad(l2[i], "open failed");
    }
    return;
  }

  int v;

  // Field 2: advanced_mode
  _diag_pad(l1[2], "advanced_mode");
  _f.seekSet(0); v = sd_find("\"advanced_mode\":") ? (int)sd_parse_number() : -1;
  if      (v == 0) _diag_pad(l2[2], "0 (Simple)");
  else if (v == 1) _diag_pad(l2[2], "1 (Advanced)");
  else             _diag_pad(l2[2], "? not found");

  // Field 3: chain_active
  _diag_pad(l1[3], "chain_active");
  _f.seekSet(0); v = sd_find("\"chain_active\":") ? (int)sd_parse_number() : -1;
  if      (v == 0) _diag_pad(l2[3], "0 (off)");
  else if (v == 1) _diag_pad(l2[3], "1 (on)");
  else             _diag_pad(l2[3], "? not found");

  // Field 4: chain_start
  _diag_pad(l1[4], "chain_start");
  _f.seekSet(0); v = sd_find("\"chain_start\":") ? (int)sd_parse_number() : -1;
  if (v >= 0 && v <= 15) snprintf(tmp, sizeof(tmp), "P%02d", v + 1);
  else                   snprintf(tmp, sizeof(tmp), "? (%d)", v);
  _diag_pad(l2[4], tmp);

  // Field 5: chain_end
  _diag_pad(l1[5], "chain_end");
  _f.seekSet(0); v = sd_find("\"chain_end\":") ? (int)sd_parse_number() : -1;
  if (v >= 0 && v <= 15) snprintf(tmp, sizeof(tmp), "P%02d", v + 1);
  else                   snprintf(tmp, sizeof(tmp), "? (%d)", v);
  _diag_pad(l2[5], tmp);

  // Field 6: tempo
  _diag_pad(l1[6], "tempo");
  _f.seekSet(0);
  if (sd_find("\"tempo\":")) { float fv = sd_parse_number(); snprintf(tmp, sizeof(tmp), "%.2f", fv); }
  else                       snprintf(tmp, sizeof(tmp), "? not found");
  _diag_pad(l2[6], tmp);

  // Field 7: swing
  _diag_pad(l1[7], "swing");
  _f.seekSet(0); v = sd_find("\"swing\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[7], tmp);

  // Field 8: midi_channel
  _diag_pad(l1[8], "midi_channel");
  _f.seekSet(0); v = sd_find("\"midi_channel\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[8], tmp);

  // Field 9: pattern_length
  _diag_pad(l1[9], "pattern_length");
  _f.seekSet(0); v = sd_find("\"pattern_length\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[9], tmp);

  // Field 10: pattern_direction
  _diag_pad(l1[10], "pattern_dir");
  _f.seekSet(0); v = sd_find("\"pattern_direction\":") ? (int)sd_parse_number() : -1;
  {
    const char* dn;
    switch (v) {
      case 1: dn="1 (Rev)";  break; case 2: dn="2 (Pong)"; break;
      case 3: dn="3 (Rand)"; break; case 4: dn="4 (Shuf)"; break;
      case 5: dn="5 (E/O)";  break; case 6: dn="6 (In)";   break;
      case 7: dn="7 (Quad)"; break; case 0: dn="0 (Fwd)";  break;
      default: dn="? not found"; break;
    }
    _diag_pad(l2[10], dn);
  }

  // Field 11: scale_type
  _diag_pad(l1[11], "scale_type");
  _f.seekSet(0); v = sd_find("\"scale_type\":") ? (int)sd_parse_number() : -1;
  {
    const char* sn;
    switch (v) {
      case 0: sn="0 Chromatic";  break; case 1: sn="1 Major";    break;
      case 2: sn="2 NatMinor";   break; case 3: sn="3 PentMaj";  break;
      case 4: sn="4 PentMin";    break; case 5: sn="5 Dorian";   break;
      case 6: sn="6 Mixolydian"; break; case 7: sn="7 HarmMinor";break;
      case 8: sn="8 Blues";      break; default: sn="? not found"; break;
    }
    _diag_pad(l2[11], sn);
  }

  // Field 12: scale_root
  _diag_pad(l1[12], "scale_root");
  _f.seekSet(0); v = sd_find("\"scale_root\":") ? (int)sd_parse_number() : -1;
  {
    static const char* roots[] = {
      "0 C","1 C#","2 D","3 D#","4 E","5 F",
      "6 F#","7 G","8 G#","9 A","10 A#","11 B"
    };
    _diag_pad(l2[12], (v >= 0 && v <= 11) ? roots[v] : "? not found");
  }

  // Field 13: pitch_drift
  _diag_pad(l1[13], "pitch_drift");
  _f.seekSet(0); v = sd_find("\"pitch_drift\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[13], tmp);

  // Field 14: note_range_low
  _diag_pad(l1[14], "note_range_low");
  _f.seekSet(0); v = sd_find("\"note_range_low\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[14], tmp);

  // Field 15: note_range_high
  _diag_pad(l1[15], "note_range_hi");
  _f.seekSet(0); v = sd_find("\"note_range_high\":") ? (int)sd_parse_number() : -1;
  snprintf(tmp, sizeof(tmp), v >= 0 ? "%d" : "? not found", v);
  _diag_pad(l2[15], tmp);

  _f.close();
}
