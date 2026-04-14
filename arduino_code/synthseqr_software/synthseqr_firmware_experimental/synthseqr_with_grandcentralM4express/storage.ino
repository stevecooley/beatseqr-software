// SAMD51 has no native EEPROM; use FlashStorage_SAMD for a compatible API.
#include <FlashAsEEPROM_SAMD.h>

// EEPROM layout — 529 bytes total.
// If you change the layout, increment EEPROM_MAGIC_VALUE so old saves are
// ignored rather than misread as valid data.
//
//  Addr  Bytes  Content
//  0     1      magic sentinel
//  1     1      MIDICHANNEL
//  2     1      SWING
//  3     4      TEMPO (float)
//  7     1      current_pattern
//  8     1      extended_step_length_mode (chain mode)
//  9     1      external_clock_mode
//  10    256    step_data[16][16]  (one byte per step, 0 or 1)
//  266   256    pattern_step_pitches[16][16]
//  522   1      octave_shift (int8_t stored as raw byte)
//  523   1      advanced_mode (bool)
//  524   1      note_shift (int8_t stored as raw byte)
//  525   1      slider_map_low_value (uint8_t)
//  526   1      slider_map_high_value (uint8_t)
//  527   1      pattern_length (uint8_t, 1–16)
//  528   1      pattern_direction (uint8_t, 0–3)

#define EEPROM_MAGIC_ADDR             0
#define EEPROM_MIDICHANNEL_ADDR       1
#define EEPROM_SWING_ADDR             2
#define EEPROM_TEMPO_ADDR             3
#define EEPROM_PATTERN_ADDR           7
#define EEPROM_CHAIN_MODE_ADDR        8
#define EEPROM_EXT_CLOCK_ADDR         9
#define EEPROM_STEP_DATA_ADDR         10
#define EEPROM_PITCHES_ADDR           266
#define EEPROM_OCTAVE_SHIFT_ADDR      522
#define EEPROM_ADVANCED_MODE_ADDR     523
#define EEPROM_NOTE_SHIFT_ADDR        524
#define EEPROM_NOTE_RANGE_LOW_ADDR    525
#define EEPROM_NOTE_RANGE_HIGH_ADDR   526
#define EEPROM_PAT_LENGTH_ADDR        527
#define EEPROM_PAT_DIR_ADDR           528
#define EEPROM_SCALE_ROOT_ADDR        529
#define EEPROM_SCALE_TYPE_ADDR        530

#define EEPROM_MAGIC_VALUE  0xC5  // bumped: added scale_root/scale_type

void save_to_eeprom() {
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  EEPROM.write(EEPROM_MIDICHANNEL_ADDR, MIDICHANNEL);
  EEPROM.write(EEPROM_SWING_ADDR, SWING);
  EEPROM.put(EEPROM_TEMPO_ADDR, TEMPO);
  EEPROM.write(EEPROM_PATTERN_ADDR, current_pattern);
  EEPROM.write(EEPROM_CHAIN_MODE_ADDR, extended_step_length_mode);
  EEPROM.write(EEPROM_EXT_CLOCK_ADDR, (uint8_t)external_clock_mode);

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 16; p++) {
    for (int s = 0; s < 16; s++) {
      EEPROM.write(addr++, (uint8_t)step_data[p][0][s]);
    }
  }

  addr = EEPROM_PITCHES_ADDR;
  for (int p = 0; p < 16; p++) {
    for (int s = 0; s < 16; s++) {
      EEPROM.write(addr++, pattern_step_pitches[p][s]);
    }
  }

  EEPROM.write(EEPROM_OCTAVE_SHIFT_ADDR, (uint8_t)octave_shift);
  EEPROM.write(EEPROM_ADVANCED_MODE_ADDR, (uint8_t)advanced_mode);
  EEPROM.write(EEPROM_NOTE_SHIFT_ADDR, (uint8_t)note_shift);
  EEPROM.write(EEPROM_NOTE_RANGE_LOW_ADDR, slider_map_low_value);
  EEPROM.write(EEPROM_NOTE_RANGE_HIGH_ADDR, slider_map_high_value);
  EEPROM.write(EEPROM_PAT_LENGTH_ADDR, pattern_length);
  EEPROM.write(EEPROM_PAT_DIR_ADDR, pattern_direction);
  EEPROM.write(EEPROM_SCALE_ROOT_ADDR, scale_root);
  EEPROM.write(EEPROM_SCALE_TYPE_ADDR, scale_type);

  // FlashAsEEPROM_SAMD buffers all writes in RAM until commit() is called.
  // Without this, nothing actually persists to flash across a power cycle.
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
  if (MIDICHANNEL < 1 || MIDICHANNEL > 16) MIDICHANNEL = 2;

  SWING = EEPROM.read(EEPROM_SWING_ADDR);
  if (SWING > 5) SWING = 0;

  EEPROM.get(EEPROM_TEMPO_ADDR, TEMPO);
  if (TEMPO < 10.0f || TEMPO > 250.0f) TEMPO = 120.0f;

  current_pattern = EEPROM.read(EEPROM_PATTERN_ADDR);
  if (current_pattern > 15) current_pattern = 0;

  extended_step_length_mode = EEPROM.read(EEPROM_CHAIN_MODE_ADDR);
  if (extended_step_length_mode > 1) extended_step_length_mode = 0;

  external_clock_mode = (bool)EEPROM.read(EEPROM_EXT_CLOCK_ADDR);

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 16; p++) {
    for (int s = 0; s < 16; s++) {
      step_data[p][0][s] = EEPROM.read(addr++);
    }
  }

  addr = EEPROM_PITCHES_ADDR;
  for (int p = 0; p < 16; p++) {
    for (int s = 0; s < 16; s++) {
      pattern_step_pitches[p][s] = EEPROM.read(addr++);
    }
  }

  octave_shift = (int8_t)EEPROM.read(EEPROM_OCTAVE_SHIFT_ADDR);
  if (octave_shift < -5 || octave_shift > 5) octave_shift = 0;

  advanced_mode = (bool)EEPROM.read(EEPROM_ADVANCED_MODE_ADDR);

  note_shift = (int8_t)EEPROM.read(EEPROM_NOTE_SHIFT_ADDR);
  if (note_shift < -12 || note_shift > 12) note_shift = 0;

  {
    uint8_t lo = EEPROM.read(EEPROM_NOTE_RANGE_LOW_ADDR);
    uint8_t hi = EEPROM.read(EEPROM_NOTE_RANGE_HIGH_ADDR);
    if (lo < hi && hi <= 127) {
      slider_map_low_value = lo;
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

  // Sync the active voice array to the loaded pattern's pitches, and arm
  // pickup so sliders don't immediately overwrite them.
  for (int s = 0; s < 16; s++) {
    voice_slider_midinotenum[s] = pattern_step_pitches[current_pattern][s];
    slider_needs_pickup[s] = true;
  }

  // Rebuild scale note pool from loaded settings.
  build_scale_notes();

  Serial.println("loaded from EEPROM");
  return true;
}
