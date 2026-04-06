// SAMD51 has no native EEPROM; use FlashStorage_SAMD for a compatible API.
#include <FlashAsEEPROM_SAMD.h>

// EEPROM layout — 138 bytes total.
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
//  10    64     step_data[4][16]  (one byte per step, 0 or 1)
//  74    64     pattern_step_pitches[4][16]

#define EEPROM_MAGIC_ADDR       0
#define EEPROM_MIDICHANNEL_ADDR 1
#define EEPROM_SWING_ADDR       2
#define EEPROM_TEMPO_ADDR       3
#define EEPROM_PATTERN_ADDR     7
#define EEPROM_CHAIN_MODE_ADDR  8
#define EEPROM_EXT_CLOCK_ADDR   9
#define EEPROM_STEP_DATA_ADDR   10
#define EEPROM_PITCHES_ADDR     74

#define EEPROM_MAGIC_VALUE  0xBE  // bump this if the layout changes

void save_to_eeprom() {
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  EEPROM.write(EEPROM_MIDICHANNEL_ADDR, MIDICHANNEL);
  EEPROM.write(EEPROM_SWING_ADDR, SWING);
  EEPROM.put(EEPROM_TEMPO_ADDR, TEMPO);
  EEPROM.write(EEPROM_PATTERN_ADDR, current_pattern);
  EEPROM.write(EEPROM_CHAIN_MODE_ADDR, extended_step_length_mode);
  EEPROM.write(EEPROM_EXT_CLOCK_ADDR, (uint8_t)external_clock_mode);

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 4; p++) {
    for (int s = 0; s < 16; s++) {
      EEPROM.write(addr++, (uint8_t)step_data[p][0][s]);
    }
  }

  addr = EEPROM_PITCHES_ADDR;
  for (int p = 0; p < 4; p++) {
    for (int s = 0; s < 16; s++) {
      EEPROM.write(addr++, pattern_step_pitches[p][s]);
    }
  }

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
  if (SWING > 6) SWING = 0;

  EEPROM.get(EEPROM_TEMPO_ADDR, TEMPO);
  if (TEMPO < 10.0f || TEMPO > 250.0f) TEMPO = 120.0f;

  current_pattern = EEPROM.read(EEPROM_PATTERN_ADDR);
  if (current_pattern > 3) current_pattern = 0;

  extended_step_length_mode = EEPROM.read(EEPROM_CHAIN_MODE_ADDR);
  if (extended_step_length_mode > 1) extended_step_length_mode = 0;

  external_clock_mode = (bool)EEPROM.read(EEPROM_EXT_CLOCK_ADDR);

  int addr = EEPROM_STEP_DATA_ADDR;
  for (int p = 0; p < 4; p++) {
    for (int s = 0; s < 16; s++) {
      step_data[p][0][s] = EEPROM.read(addr++);
    }
  }

  addr = EEPROM_PITCHES_ADDR;
  for (int p = 0; p < 4; p++) {
    for (int s = 0; s < 16; s++) {
      pattern_step_pitches[p][s] = EEPROM.read(addr++);
    }
  }

  // Sync the active voice array to the loaded pattern's pitches, and arm
  // pickup so sliders don't immediately overwrite them.
  for (int s = 0; s < 16; s++) {
    voice_slider_midinotenum[s] = pattern_step_pitches[current_pattern][s];
    slider_needs_pickup[s] = true;
  }

  Serial.println("loaded from EEPROM");
  return true;
}
