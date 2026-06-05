// SAMD51 has no native EEPROM; use FlashStorage_SAMD for a compatible API.
#include <FlashAsEEPROM_SAMD.h>

// EEPROM layout — 2605 bytes total.
// If you change the layout, increment EEPROM_MAGIC_VALUE so old saves are
// ignored rather than misread as valid data.
//
//  Addr  Bytes  Content
//  0     1      magic sentinel (0xBE — different from Synthseqr 0xC6)
//  1     1      MIDICHANNEL
//  2     1      SWING
//  3     4      TEMPO (float)
//  7     1      current_pattern
//  8     1      extended_step_length_mode (chain mode)
//  9     1      external_clock_mode
//  10    1      octave_shift (int8_t)
//  11    1      advanced_mode (bool)
//  12    1      note_shift (int8_t)
//  13    1      slider_map_low_value (uint8_t)
//  14    1      slider_map_high_value (uint8_t)
//  15    1      pattern_length (uint8_t, 1–16)
//  16    1      pattern_direction (uint8_t, 0–7)
//  17    1      scale_root (uint8_t, 0–11)
//  18    1      scale_type (uint8_t, 0–8)
//  19    1      chain_start (uint8_t)
//  20    1      chain_end (uint8_t)
//  21    2048   step_data[16][8][16]   — [p][v][s], 16×8×16 bytes
//  2069  128    voice_pitch[16][8]
//  2197  128    voice_velocity[16][8]
//  2325  128    voice_gate[16][8]
//  2453  128    voice_cc_value[16][8]
//  2581  8      voice_cc_enabled[8]
//  2589  16     cc_number[16]
//  2605  128    voice_probability[16][8]
//  2733  1      slider_takeover (0=Catch 1=Jump 2=Relative)
//  2734  11     ft_* feature flags (one byte each)
//  2745  1      swing_knob_function (0=Swing 1=Tempo 2=Pattern 3=Voice)
//  ---- 2746 bytes total ----

#define EEPROM_MAGIC_ADDR             0
#define EEPROM_MIDICHANNEL_ADDR       1
#define EEPROM_SWING_ADDR             2
#define EEPROM_TEMPO_ADDR             3
#define EEPROM_PATTERN_ADDR           7
#define EEPROM_CHAIN_MODE_ADDR        8
#define EEPROM_EXT_CLOCK_ADDR         9
#define EEPROM_OCTAVE_SHIFT_ADDR      10
#define EEPROM_ADVANCED_MODE_ADDR     11
#define EEPROM_NOTE_SHIFT_ADDR        12
#define EEPROM_NOTE_RANGE_LOW_ADDR    13
#define EEPROM_NOTE_RANGE_HIGH_ADDR   14
#define EEPROM_PAT_LENGTH_ADDR        15
#define EEPROM_PAT_DIR_ADDR           16
#define EEPROM_SCALE_ROOT_ADDR        17
#define EEPROM_SCALE_TYPE_ADDR        18
#define EEPROM_CHAIN_START_ADDR       19
#define EEPROM_CHAIN_END_ADDR         20
#define EEPROM_STEP_DATA_ADDR         21     // 2048 bytes: step_data[16][8][16]
#define EEPROM_VOICE_PITCH_ADDR       2069   // 128 bytes: voice_pitch[16][8]
#define EEPROM_VOICE_VEL_ADDR         2197   // 128 bytes: voice_velocity[16][8]
#define EEPROM_VOICE_GATE_ADDR        2325   // 128 bytes: voice_gate[16][8]
#define EEPROM_VOICE_CC_VAL_ADDR      2453   // 128 bytes: voice_cc_value[16][8]
#define EEPROM_VOICE_CC_EN_ADDR       2581   // 8 bytes:   voice_cc_enabled[8]
#define EEPROM_CC_NUMBERS_ADDR        2589   // 16 bytes:  cc_number[16]
#define EEPROM_VOICE_PROB_ADDR        2605   // 128 bytes: voice_probability[16][8]
#define EEPROM_TAKEOVER_ADDR          2733   // 1 byte:   slider_takeover (0/1/2)
#define EEPROM_FT_FLAGS_ADDR          2734   // 11 bytes: ft_* feature flags
#define EEPROM_SWING_KNOB_FN_ADDR     2745   // 1 byte:   swing_knob_function (0–3)

#define EEPROM_MAGIC_VALUE  0xC1  // bumped: added swing_knob_function

void save_to_eeprom() {
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  EEPROM.write(EEPROM_MIDICHANNEL_ADDR, MIDICHANNEL);
  EEPROM.write(EEPROM_SWING_ADDR, SWING);
  EEPROM.put(EEPROM_TEMPO_ADDR, TEMPO);
  EEPROM.write(EEPROM_PATTERN_ADDR, current_pattern);
  EEPROM.write(EEPROM_CHAIN_MODE_ADDR, extended_step_length_mode);
  EEPROM.write(EEPROM_EXT_CLOCK_ADDR, (uint8_t)external_clock_mode);
  EEPROM.write(EEPROM_OCTAVE_SHIFT_ADDR, (uint8_t)octave_shift);
  EEPROM.write(EEPROM_ADVANCED_MODE_ADDR, (uint8_t)advanced_mode);
  EEPROM.write(EEPROM_NOTE_SHIFT_ADDR, (uint8_t)note_shift);
  EEPROM.write(EEPROM_NOTE_RANGE_LOW_ADDR, slider_map_low_value);
  EEPROM.write(EEPROM_NOTE_RANGE_HIGH_ADDR, slider_map_high_value);
  EEPROM.write(EEPROM_PAT_LENGTH_ADDR, pattern_length);
  EEPROM.write(EEPROM_PAT_DIR_ADDR, pattern_direction);
  EEPROM.write(EEPROM_SCALE_ROOT_ADDR, scale_root);
  EEPROM.write(EEPROM_SCALE_TYPE_ADDR, scale_type);
  EEPROM.write(EEPROM_CHAIN_START_ADDR, chain_start);
  EEPROM.write(EEPROM_CHAIN_END_ADDR, chain_end);

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(addr++, (uint8_t)step_data[p][v][s]);

  addr = EEPROM_VOICE_PITCH_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      EEPROM.write(addr++, voice_pitch[p][v]);

  addr = EEPROM_VOICE_VEL_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      EEPROM.write(addr++, voice_velocity[p][v]);

  addr = EEPROM_VOICE_GATE_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      EEPROM.write(addr++, voice_gate[p][v]);

  addr = EEPROM_VOICE_CC_VAL_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      EEPROM.write(addr++, voice_cc_value[p][v]);

  for (int v = 0; v < VOICE_COUNT; v++)
    EEPROM.write(EEPROM_VOICE_CC_EN_ADDR + v, voice_cc_enabled[v]);

  for (int p = 0; p < 16; p++)
    EEPROM.write(EEPROM_CC_NUMBERS_ADDR + p, cc_number[p]);

  {
    int addr = EEPROM_VOICE_PROB_ADDR;
    for (int p = 0; p < 16; p++)
      for (int v = 0; v < VOICE_COUNT; v++)
        EEPROM.write(addr++, voice_probability[p][v]);
  }

  EEPROM.write(EEPROM_TAKEOVER_ADDR, slider_takeover);

  // Feature flags — order must match load_from_eeprom() below.
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 0,  (uint8_t)ft_advanced_mode);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 1,  (uint8_t)ft_cc_mode);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 2,  (uint8_t)ft_probability);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 3,  (uint8_t)ft_gate_mode);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 4,  (uint8_t)ft_velocity_mode);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 5,  (uint8_t)ft_scale_quantization);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 6,  (uint8_t)ft_pattern_direction);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 7,  (uint8_t)ft_variable_pat_length);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 8,  (uint8_t)ft_external_clock);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 9,  (uint8_t)ft_octave_note_shift);
  EEPROM.write(EEPROM_FT_FLAGS_ADDR + 10, (uint8_t)ft_diagnostics);

  EEPROM.write(EEPROM_SWING_KNOB_FN_ADDR, swing_knob_function);

  // FlashAsEEPROM_SAMD buffers all writes in RAM until commit() is called.
  EEPROM.commit();

  Serial.println("saved to EEPROM");
}

// Returns true if valid save data was found and loaded.
// Returns false on first boot (no magic byte) — globals keep their defaults.
bool load_from_eeprom() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VALUE) {
    Serial.println("no EEPROM save found, using defaults");
    return false;
  }

  MIDICHANNEL = EEPROM.read(EEPROM_MIDICHANNEL_ADDR);
  if (MIDICHANNEL < 1 || MIDICHANNEL > 16) MIDICHANNEL = 10;

  SWING = EEPROM.read(EEPROM_SWING_ADDR);
  if (SWING > 5) SWING = 0;

  EEPROM.get(EEPROM_TEMPO_ADDR, TEMPO);
  if (TEMPO < 30.0f || TEMPO > 250.0f) TEMPO = 120.0f;

  current_pattern = EEPROM.read(EEPROM_PATTERN_ADDR);
  if (current_pattern > 15) current_pattern = 0;

  extended_step_length_mode = EEPROM.read(EEPROM_CHAIN_MODE_ADDR);
  if (extended_step_length_mode > 1) extended_step_length_mode = 0;

  external_clock_mode = (bool)EEPROM.read(EEPROM_EXT_CLOCK_ADDR);

  octave_shift = (int8_t)EEPROM.read(EEPROM_OCTAVE_SHIFT_ADDR);
  if (octave_shift < -5 || octave_shift > 5) octave_shift = 0;

  advanced_mode = (bool)EEPROM.read(EEPROM_ADVANCED_MODE_ADDR);

  note_shift = (int8_t)EEPROM.read(EEPROM_NOTE_SHIFT_ADDR);
  if (note_shift < -12 || note_shift > 12) note_shift = 0;

  {
    uint8_t lo = EEPROM.read(EEPROM_NOTE_RANGE_LOW_ADDR);
    uint8_t hi = EEPROM.read(EEPROM_NOTE_RANGE_HIGH_ADDR);
    if (lo < hi && hi <= 127) {
      slider_map_low_value  = lo;
      slider_map_high_value = hi;
    }
  }

  {
    uint8_t pl = EEPROM.read(EEPROM_PAT_LENGTH_ADDR);
    if (pl >= 1 && pl <= 16) pattern_length = pl;
  }
  {
    uint8_t pd = EEPROM.read(EEPROM_PAT_DIR_ADDR);
    if (pd < PATTERN_DIRECTION_COUNT) pattern_direction = pd;
  }
  {
    uint8_t sr = EEPROM.read(EEPROM_SCALE_ROOT_ADDR);
    if (sr < 12) scale_root = sr;
  }
  {
    uint8_t st = EEPROM.read(EEPROM_SCALE_TYPE_ADDR);
    if (st < SCALE_COUNT) scale_type = st;
  }

  chain_start = EEPROM.read(EEPROM_CHAIN_START_ADDR);
  if (chain_start > 15) chain_start = 0;
  chain_end = EEPROM.read(EEPROM_CHAIN_END_ADDR);
  if (chain_end > 15) chain_end = 3;

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++)
      for (int s = 0; s < 16; s++)
        step_data[p][v][s] = EEPROM.read(addr++);

  addr = EEPROM_VOICE_PITCH_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++) {
      uint8_t val = EEPROM.read(addr++);
      if (val <= 127) voice_pitch[p][v] = val;
    }

  addr = EEPROM_VOICE_VEL_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++) {
      uint8_t val = EEPROM.read(addr++);
      if (val <= 127) voice_velocity[p][v] = val;
    }

  addr = EEPROM_VOICE_GATE_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++) {
      uint8_t val = EEPROM.read(addr++);
      if (val >= 1 && val <= 8) voice_gate[p][v] = val;
    }

  addr = EEPROM_VOICE_CC_VAL_ADDR;
  for (int p = 0; p < 16; p++)
    for (int v = 0; v < VOICE_COUNT; v++) {
      uint8_t val = EEPROM.read(addr++);
      if (val <= 127) voice_cc_value[p][v] = val;
    }

  for (int v = 0; v < VOICE_COUNT; v++)
    voice_cc_enabled[v] = EEPROM.read(EEPROM_VOICE_CC_EN_ADDR + v) ? 1 : 0;

  for (int p = 0; p < 16; p++) {
    uint8_t n = EEPROM.read(EEPROM_CC_NUMBERS_ADDR + p);
    if (n >= 1 && n <= 119 && n != 32 && !(n >= 96 && n <= 101))
      cc_number[p] = n;
  }

  {
    int addr = EEPROM_VOICE_PROB_ADDR;
    for (int p = 0; p < 16; p++)
      for (int v = 0; v < VOICE_COUNT; v++) {
        uint8_t val = EEPROM.read(addr++);
        if (val <= 100) voice_probability[p][v] = val;
      }
  }

  {
    uint8_t t = EEPROM.read(EEPROM_TAKEOVER_ADDR);
    if (t <= 2) slider_takeover = t;
  }

  // Feature flags — order must match save_to_eeprom() above.
  ft_advanced_mode       = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 0)  ? true : false;
  ft_cc_mode             = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 1)  ? true : false;
  ft_probability         = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 2)  ? true : false;
  ft_gate_mode           = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 3)  ? true : false;
  ft_velocity_mode       = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 4)  ? true : false;
  ft_scale_quantization  = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 5)  ? true : false;
  ft_pattern_direction   = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 6)  ? true : false;
  ft_variable_pat_length = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 7)  ? true : false;
  ft_external_clock      = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 8)  ? true : false;
  ft_octave_note_shift   = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 9)  ? true : false;
  ft_diagnostics         = EEPROM.read(EEPROM_FT_FLAGS_ADDR + 10) ? true : false;

  {
    uint8_t f = EEPROM.read(EEPROM_SWING_KNOB_FN_ADDR);
    if (f < SWING_KNOB_FN_COUNT) swing_knob_function = f;
  }

  // Arm pickup guards so sliders don't immediately overwrite loaded values
  // (Catch only; Jump/Relative don't use pickup). Seed last_raw for Relative.
  for (int v = 0; v < VOICE_COUNT; v++) {
    slider_needs_pickup[v] = (slider_takeover == 0);
    slider_last_raw[v]     = voice_sliders[v].getValue();
  }

  // Reset ping-pong state so playback starts from the beginning.
  ping_pong_step = 0;
  ping_pong_going_forward = true;

  // Rebuild scale note pool from loaded settings.
  build_scale_notes();

  Serial.println("loaded from EEPROM");
  return true;
}
