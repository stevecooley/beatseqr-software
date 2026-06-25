// SAMD51 has no native EEPROM; use FlashStorage_SAMD for a compatible API.
// The library's default EEPROM_EMULATION_SIZE is 1024 bytes — smaller than our
// 2121-byte layout. Writes past the buffer overflow into adjacent globals
// (seen symptom: `advanced_mode` and `adv_pat_nav_active` bools getting set
// to 100, the default value of step_probability bytes being written past the
// buffer end). Bumped to 4096 to give chord-mode fields (256 + 1 + 1 bytes)
// headroom plus room for future per-step arrays without another resize.
#define EEPROM_EMULATION_SIZE 4096
#include <FlashAsEEPROM_SAMD.h>

// EEPROM layout — 1862 bytes total.
// If you change the layout, increment EEPROM_MAGIC_VALUE so old saves are
// ignored rather than misread as valid data.
//
//  Addr    Bytes  Content
//  0       1      magic sentinel
//  1       1      MIDICHANNEL
//  2       1      SWING
//  3       4      TEMPO (float)
//  7       1      current_pattern
//  8       1      extended_step_length_mode (chain mode)
//  9       1      external_clock_mode
//  10      256    step_data[16][16]  (one byte per step, 0 or 1)
//  266     256    pattern_step_pitches[16][16]
//  522     1      octave_shift (int8_t stored as raw byte)
//  523     1      advanced_mode (bool)
//  524     1      note_shift (int8_t stored as raw byte)
//  525     1      slider_map_low_value (uint8_t)
//  526     1      slider_map_high_value (uint8_t)
//  527     1      pattern_length (uint8_t, 1–16)
//  528     1      pattern_direction (uint8_t, 0–7)
//  529     1      scale_root (uint8_t, 0–11)
//  530     1      scale_type (uint8_t, 0–9)
//  531     16     cc_number[16]
//  547     256    cc_step_enabled[16][16]
//  803     256    cc_step_values[16][16]
//  1059    1      pitch_drift
//  1060    256    step_probability[16][16]
//  1316    13     runtime feature flags (ft_*)
//  1329    1      slider_hi_trim
//  1330    1      ft_live_cc_mode (bool)
//  1331    1      live_cc_channel (1–16)
//  1332    16     live_cc_number[16] (CC# per lane)
//  1348    1      ft_drift_mode (bool)
//  1349    256    step_drift_enabled[16][16]
//  1605    256    step_drift_amount[16][16] (0–12)
//  1861    1      slider_takeover (0=Catch 1=Jump 2=Relative)
//  1862    1      dpad_main_mode (0..DPAD_MAIN_MODE_COUNT-1)
//  1863    256    step_chord_type[16][16]  (0=single note, 1..CHORD_COUNT-1)
//  2119    1      current_chord_type (paint-active chord type, 0..CHORD_COUNT-1)
//  2120    1      ft_chord_mode (bool)
//  2121    1      slider_noise_threshold (raw ADC; 12=Low 24=Med 48=High)
//  2122    1      ft_midi_program_mode (bool)
//  2123    1536   step_custom_chord[16][16][6] (-1 stored as 0xFF; pitch 0..127 raw)
//  3659    1      clock_div (index into CLOCK_DIV[])
//  3660    1      ft_clock_div (bool)

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
#define EEPROM_CC_NUMBERS_ADDR        531   // 16 bytes: cc_number[16]
#define EEPROM_CC_ENABLED_ADDR        547   // 256 bytes: cc_step_enabled[16][16]
#define EEPROM_CC_VALUES_ADDR         803   // 256 bytes: cc_step_values[16][16]
#define EEPROM_PITCH_DRIFT_ADDR      1059   // 1 byte: pitch_drift
#define EEPROM_STEP_PROB_ADDR        1060   // 256 bytes: step_probability[16][16]
#define EEPROM_FEATURE_FLAGS_ADDR    1316   // 13 bytes: runtime feature flags (ft_*)
#define EEPROM_HI_TRIM_ADDR          1329   // 1 byte: slider_hi_trim (0–4)
#define EEPROM_FT_LIVE_CC_ADDR       1330   // 1 byte: ft_live_cc_mode (bool)
#define EEPROM_LIVE_CC_CHAN_ADDR     1331   // 1 byte: live_cc_channel (1–16)
#define EEPROM_LIVE_CC_NUMBERS_ADDR  1332   // 16 bytes: live_cc_number[16]
#define EEPROM_FT_DRIFT_ADDR         1348   // 1 byte: ft_drift_mode (bool)
#define EEPROM_DRIFT_ENABLED_ADDR    1349   // 256 bytes: step_drift_enabled[16][16]
#define EEPROM_DRIFT_AMOUNT_ADDR     1605   // 256 bytes: step_drift_amount[16][16]
#define EEPROM_SLIDER_TAKEOVER_ADDR  1861   // 1 byte: slider_takeover (0–2)
#define EEPROM_DPAD_MAIN_MODE_ADDR   1862   // 1 byte: dpad_main_mode (0..N-1)
#define EEPROM_CHORD_TYPES_ADDR      1863   // 256 bytes: step_chord_type[16][16]
#define EEPROM_CURRENT_CHORD_ADDR    2119   // 1 byte: current_chord_type
#define EEPROM_FT_CHORD_ADDR         2120   // 1 byte: ft_chord_mode
#define EEPROM_SLIDER_NOISE_ADDR     2121   // 1 byte: slider_noise_threshold (12/24/48)
#define EEPROM_FT_MIDI_PROG_ADDR     2122   // 1 byte: ft_midi_program_mode (bool)
#define EEPROM_CUSTOM_CHORD_ADDR     2123   // 1536 bytes: step_custom_chord[16][16][6]
#define EEPROM_CLOCK_DIV_ADDR        3659   // 1 byte: clock_div (index into CLOCK_DIV[])
#define EEPROM_FT_CLOCK_DIV_ADDR     3660   // 1 byte: ft_clock_div (bool)

#define EEPROM_MAGIC_VALUE  0xD5  // bumped: clock_div + ft_clock_div added

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

  {
    int cc_addr = EEPROM_CC_NUMBERS_ADDR;
    for (int p = 0; p < 16; p++) EEPROM.write(cc_addr++, cc_number[p]);
    cc_addr = EEPROM_CC_ENABLED_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(cc_addr++, cc_step_enabled[p][s]);
    cc_addr = EEPROM_CC_VALUES_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(cc_addr++, cc_step_values[p][s]);
  }

  EEPROM.write(EEPROM_PITCH_DRIFT_ADDR, pitch_drift);

  {
    int addr = EEPROM_STEP_PROB_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(addr++, step_probability[p][s]);
  }

  {
    int fa = EEPROM_FEATURE_FLAGS_ADDR;
    EEPROM.write(fa++, (uint8_t)ft_advanced_mode);
    EEPROM.write(fa++, (uint8_t)ft_cc_mode);
    EEPROM.write(fa++, (uint8_t)ft_probability);
    EEPROM.write(fa++, (uint8_t)ft_gate_mode);
    EEPROM.write(fa++, (uint8_t)ft_scale_quantization);
    EEPROM.write(fa++, (uint8_t)ft_pitch_drift);
    EEPROM.write(fa++, (uint8_t)ft_pattern_direction);
    EEPROM.write(fa++, (uint8_t)ft_variable_pat_length);
    EEPROM.write(fa++, (uint8_t)ft_swing);
    EEPROM.write(fa++, (uint8_t)ft_external_clock);
    EEPROM.write(fa++, (uint8_t)ft_octave_note_shift);
    EEPROM.write(fa++, (uint8_t)ft_diagnostics);
    EEPROM.write(fa++, (uint8_t)ft_velocity_mode);
  }

  EEPROM.write(EEPROM_HI_TRIM_ADDR, slider_hi_trim);

  EEPROM.write(EEPROM_FT_LIVE_CC_ADDR, (uint8_t)ft_live_cc_mode);
  EEPROM.write(EEPROM_LIVE_CC_CHAN_ADDR, live_cc_channel);
  {
    int addr = EEPROM_LIVE_CC_NUMBERS_ADDR;
    for (int i = 0; i < 16; i++) EEPROM.write(addr++, live_cc_number[i]);
  }

  EEPROM.write(EEPROM_FT_DRIFT_ADDR, (uint8_t)ft_drift_mode);
  {
    int addr = EEPROM_DRIFT_ENABLED_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(addr++, step_drift_enabled[p][s]);
    addr = EEPROM_DRIFT_AMOUNT_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(addr++, step_drift_amount[p][s]);
  }

  EEPROM.write(EEPROM_SLIDER_TAKEOVER_ADDR, slider_takeover);
  EEPROM.write(EEPROM_DPAD_MAIN_MODE_ADDR, dpad_main_mode);

  {
    int addr = EEPROM_CHORD_TYPES_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        EEPROM.write(addr++, step_chord_type[p][s]);
  }
  EEPROM.write(EEPROM_CURRENT_CHORD_ADDR, current_chord_type);
  EEPROM.write(EEPROM_FT_CHORD_ADDR, (uint8_t)ft_chord_mode);

  EEPROM.write(EEPROM_SLIDER_NOISE_ADDR, slider_noise_threshold);

  EEPROM.write(EEPROM_FT_MIDI_PROG_ADDR, (uint8_t)ft_midi_program_mode);
  {
    int addr = EEPROM_CUSTOM_CHORD_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        for (int n = 0; n < MAX_CHORD_NOTES; n++)
          EEPROM.write(addr++, (uint8_t)step_custom_chord[p][s][n]);
  }

  EEPROM.write(EEPROM_CLOCK_DIV_ADDR, clock_div);
  EEPROM.write(EEPROM_FT_CLOCK_DIV_ADDR, (uint8_t)ft_clock_div);

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

  {
    uint8_t raw = EEPROM.read(EEPROM_ADVANCED_MODE_ADDR);
    advanced_mode = (bool)raw;
    if (advanced_mode > 1) advanced_mode = false;
  }

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

  {
    int cc_addr = EEPROM_CC_NUMBERS_ADDR;
    for (int p = 0; p < 16; p++) {
      uint8_t v = EEPROM.read(cc_addr++);
      if (v >= 1 && v <= 119 && v != 32 && !(v >= 96 && v <= 101))
        cc_number[p] = v;
    }
  }
  {
    int cc_addr = EEPROM_CC_ENABLED_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        cc_step_enabled[p][s] = EEPROM.read(cc_addr++) ? 1 : 0;
  }
  {
    int cc_addr = EEPROM_CC_VALUES_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++) {
        uint8_t v = EEPROM.read(cc_addr++);
        if (v <= 127) cc_step_values[p][s] = v;
      }
  }

  {
    uint8_t v = EEPROM.read(EEPROM_PITCH_DRIFT_ADDR);
    if (v <= 7) pitch_drift = v;
  }

  {
    int addr = EEPROM_STEP_PROB_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++) {
        uint8_t v = EEPROM.read(addr++);
        if (v <= 100) step_probability[p][s] = v;
      }
  }

  {
    int fa = EEPROM_FEATURE_FLAGS_ADDR;
    uint8_t v;
    v = EEPROM.read(fa++); if (v <= 1) ft_advanced_mode       = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_cc_mode             = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_probability         = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_gate_mode           = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_scale_quantization  = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_pitch_drift         = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_pattern_direction   = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_variable_pat_length = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_swing               = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_external_clock      = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_octave_note_shift   = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_diagnostics         = (bool)v;
    v = EEPROM.read(fa++); if (v <= 1) ft_velocity_mode       = (bool)v;
  }

  {
    uint8_t v = EEPROM.read(EEPROM_HI_TRIM_ADDR);
    if (v <= 4) slider_hi_trim = v;
  }

  {
    uint8_t v = EEPROM.read(EEPROM_FT_LIVE_CC_ADDR);
    if (v <= 1) ft_live_cc_mode = (bool)v;
  }
  {
    uint8_t v = EEPROM.read(EEPROM_LIVE_CC_CHAN_ADDR);
    if (v >= 1 && v <= 16) live_cc_channel = v;
  }
  {
    int addr = EEPROM_LIVE_CC_NUMBERS_ADDR;
    for (int i = 0; i < 16; i++) {
      uint8_t v = EEPROM.read(addr++);
      if (v >= 1 && v <= 119 && v != 32 && !(v >= 96 && v <= 101))
        live_cc_number[i] = v;
    }
  }

  {
    uint8_t v = EEPROM.read(EEPROM_FT_DRIFT_ADDR);
    if (v <= 1) ft_drift_mode = (bool)v;
  }
  {
    int addr = EEPROM_DRIFT_ENABLED_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        step_drift_enabled[p][s] = EEPROM.read(addr++) ? 1 : 0;
    addr = EEPROM_DRIFT_AMOUNT_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++) {
        uint8_t v = EEPROM.read(addr++);
        step_drift_amount[p][s] = (v <= 12) ? v : 0;
      }
  }

  {
    uint8_t v = EEPROM.read(EEPROM_SLIDER_TAKEOVER_ADDR);
    if (v <= 2) slider_takeover = v;
  }

  {
    uint8_t v = EEPROM.read(EEPROM_DPAD_MAIN_MODE_ADDR);
    if (v < DPAD_MAIN_MODE_COUNT) dpad_main_mode = v;
  }

  {
    int addr = EEPROM_CHORD_TYPES_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++) {
        uint8_t v = EEPROM.read(addr++);
        step_chord_type[p][s] = (v < CHORD_COUNT) ? v : 0;
      }
  }
  {
    uint8_t v = EEPROM.read(EEPROM_CURRENT_CHORD_ADDR);
    if (v < CHORD_COUNT) current_chord_type = v;
  }
  {
    uint8_t v = EEPROM.read(EEPROM_FT_CHORD_ADDR);
    if (v <= 1) ft_chord_mode = (bool)v;
  }
  {
    uint8_t v = EEPROM.read(EEPROM_SLIDER_NOISE_ADDR);
    if (v == 12 || v == 24 || v == 48) slider_noise_threshold = v;
  }

  {
    uint8_t v = EEPROM.read(EEPROM_FT_MIDI_PROG_ADDR);
    if (v <= 1) ft_midi_program_mode = (bool)v;
  }
  {
    // Stored as raw bytes: 0xFF = -1 (empty slot), 0..127 = pitch, anything
    // else is corrupt and falls back to -1.
    int addr = EEPROM_CUSTOM_CHORD_ADDR;
    for (int p = 0; p < 16; p++)
      for (int s = 0; s < 16; s++)
        for (int n = 0; n < MAX_CHORD_NOTES; n++) {
          uint8_t v = EEPROM.read(addr++);
          step_custom_chord[p][s][n] = (v <= 127) ? (int8_t)v : (int8_t)-1;
        }
  }

  {
    uint8_t v = EEPROM.read(EEPROM_CLOCK_DIV_ADDR);
    if (v < CLOCK_DIV_COUNT) clock_div = v;
  }
  {
    uint8_t v = EEPROM.read(EEPROM_FT_CLOCK_DIV_ADDR);
    if (v <= 1) ft_clock_div = (bool)v;
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
