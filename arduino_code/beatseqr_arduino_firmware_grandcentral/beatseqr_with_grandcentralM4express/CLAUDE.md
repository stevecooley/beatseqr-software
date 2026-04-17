# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Beatseqr is Arduino firmware for a hardware MIDI drum step sequencer running on the **Adafruit Grand Central M4 Express** (ATSAMD51J19A ARM Cortex-M4). It is a 16-step, up to 16-pattern sequencer with 8 drum voices, physical step buttons, voice sliders, voice-select buttons (resistor ladder), two hardware knobs (tempo + swing), and an LCD display. This firmware is a port of the Synthseqr firmware adapted for Beatseqr's different hardware layout.

## Build and Upload

This is an Arduino sketch. Open `beatseqr_with_grandcentralM4express.ino` in the Arduino IDE or use the Arduino CLI:

```bash
# Compile
arduino-cli compile --fqbn adafruit:samd:adafruit_grandcentral_m4 beatseqr_with_grandcentralM4express

# Upload (replace /dev/ttyACM0 with actual port)
arduino-cli upload -p /dev/ttyACM0 --fqbn adafruit:samd:adafruit_grandcentral_m4 beatseqr_with_grandcentralM4express
```

**Required libraries**: `MIDIUSB`, `FlashStorage_SAMD` (install via Library Manager). The `FifteenStep` library is custom and lives at `../../../libraries/FifteenStep/` relative to this sketch.

Serial monitor: 57600 baud for diagnostics output.

## Architecture

All `.ino` files are compiled as a single translation unit by Arduino. They share the globals declared in `config.h`.

**File responsibilities:**

| File | Role |
|------|------|
| `beatseqr_with_grandcentralM4express.ino` | `setup()` and `loop()` — polls all subsystems in order |
| `config.h` | All global state: pin definitions, arrays, library includes |
| `transport.ino` | Play/stop button ISR, transport events, `stepsend()` callback, `allNotesOff()`, swing |
| `midi_processor.ino` | MIDI input: clock sync (0xF8), start (0xFA), stop (0xFC) — `read_midi()` only |
| `midi_note_sending.ino` | Low-level MIDI output helpers: `noteOn`, `noteOff`, `controlChange`, `clockStart`, `clockStop`, `clockPulse`, `midi()` FifteenStep callback |
| `navigation.ino` | `listen_for_navigation_events()` (adv copy cancel), `setExternalClockMode()` |
| `sequencer_timer.ino` | TC4 hardware timer driver: setup, period update, stop/start, ISR |
| `step_button_routine.ino` | Step button presses, LED toggling, pattern clear, `read_step_memory()`, `init_blank_patterns_to_range()` |
| `voice_slider_routine.ino` | Reads 8 voice sliders in NN/VL/GT/CC mode → `voice_pitch/velocity/gate/cc_value` |
| `voice_select_routine.ino` | Resistor-ladder voice select (A10), `set_current_voice()` |
| `knob_routine.ino` | Tempo knob (A8) → TEMPO, swing knob (A9) → SWING; jog-wheel for config menu |
| `pattern_select_routine.ino` | Pattern switching, copy, chain mode, `go_to_pattern()` |
| `LCD.ino` | LCD initialization and display updates |
| `scales.ino` | Musical scale tables, `build_scale_notes()`, `quantize_to_scale()`, `apply_scale_to_all_patterns()` |
| `config_menu.ino` | Modal config menu: knob-jog navigation, all settings |
| `storage.ino` | EEPROM save/load (2605-byte layout, magic `0xBE`) |
| `sd_storage.ino` | SD card save/load: `/beatseqr/autosave.json`, `boot_load()`, `save_everywhere()` |
| `diagnostics.ino` | Hardware test mode stub (Phase 5 — not yet implemented) |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers.

**Potentiometer / SAMD51 ADC resolution**: `Potentiometer.cpp` uses `analogRead() / (4096/sectors)` — do NOT use 1024 as the divisor. The `getSector()` method returns a 0–(sectors-1) value. Voice sliders use 255 sectors for full resolution. Swing knob uses 8 sectors.

**Voice select ADC**: `analogReadResolution(10)` is called in `setup()` to keep the resistor-ladder thresholds (calibrated for 10-bit ADC) valid. Do not change to 12-bit for the resistor ladder — the thresholds in `vselectval_lowerranges/upperranges` would need recalibration.

## Key Hardware Differences from Synthseqr

| Feature | Synthseqr | Beatseqr |
|---------|-----------|----------|
| Voices | 16 sliders (one per step) | 8 sliders (one per voice) |
| Voice select | No | Resistor ladder on A10 (8 buttons) |
| Tempo control | D-pad | Knob on A8 |
| Swing control | Config menu | Knob on A9 |
| Config nav | D-pad + Enter button | Knobs (jog) + param_rec (Enter) |
| Play button pin | 21 | A11 / pin 65 |
| Play LED pin | 21 (same pin?) | pin 10 |
| Step LEDs | pins 22–52 even | pins 22–52 even (same) |
| Step buttons | pins 23–53 odd | pins 23–53 odd (same) |
| Pattern buttons | pins 6/8/14/15 | pins 18–21 |
| Pattern LEDs | corresponding | pins 14–17 |
| Voice LEDs | none | pins 2–9 |
| MIDI channel | 2 (melodic) | 10 (GM drums) |

## Data Model

Beatseqr uses a **per-voice** data model, not the per-step model of Synthseqr:

```cpp
// Core sequencer data:
int step_data[16][VOICE_COUNT][16]     // [pattern][voice][step] — on/off
uint8_t voice_pitch[16][VOICE_COUNT]   // [pattern][voice] — MIDI note for each voice
uint8_t voice_velocity[16][VOICE_COUNT]// [pattern][voice] — velocity (0=muted)
uint8_t voice_gate[16][VOICE_COUNT]    // [pattern][voice] — gate length 1–8 steps
uint8_t voice_cc_value[16][VOICE_COUNT]// [pattern][voice] — CC value 0–127
uint8_t voice_cc_enabled[VOICE_COUNT]  // [voice] — per-voice CC enable (not per-step)
uint8_t cc_number[16]                  // [pattern] — CC controller number per pattern

// Voice sounding state (indexed by voice, not step):
int8_t sounding_notes[VOICE_COUNT]         // MIDI pitch currently sounding (-1=silent)
int8_t sounding_note_end_step[VOICE_COUNT] // step to fire note-off (-1=silent)
```

The key distinction from Synthseqr: each drum voice has ONE pitch/velocity/gate that applies to every active step for that voice. The slider for voice N always controls voice N regardless of which voice is "selected". Selecting a voice only determines which voice's step data is shown on the step LEDs and which voice is edited by step button presses.

## Sequencer Flow

1. `seq.run()` (FifteenStep) ticks the sequencer each `loop()`
2. TC4 hardware timer (internal) or USB-MIDI 0xF8 (external) calls `seq.hardwareClockPulse()`
3. On each step, `stepsend(current_step, last_step)` fires
4. `stepsend()` computes `play_step` from `pattern_direction`, then for all 8 voices:
   - Sends note-off for any voice whose `sounding_note_end_step[v] == current_step`
   - If `step_data[pat][v][play_step]` is on AND velocity > 0: sends note-on, schedules note-off
   - If `voice_cc_enabled[v]` AND step active: sends CC message
5. On stop: `allNotesOff()` sends note-off for all 8 `sounding_notes[]` entries

## Hardware Pin Map

```
Play button:       A11 / pin 78   (hardware interrupt, FALLING edge)
Play LED:          pin 10
param_rec (Enter): D11 / pin 11
Voice LEDs:        pins 2–9       (one per voice, active voice lit)
Voice select:      A10 / pin 77   (resistor ladder, 8 voices)
Voice sliders:     A7–A0          (slider[0]=A7, slider[7]=A0)
Tempo knob:        A8 / pin 75
Swing knob:        A9 / pin 76
slider_mode_select: A13 / pin 80
voice_mode_select:  A12 / pin 79
knob_mode_select:   A14 / pin 81
NOTE: On Grand Central M4, A0=67, A1=68...A11=78, A12=79, A13=80, A14=81.
      Always use Ax macros in code, never hardcoded numbers for analog pins.
Step LEDs:         pins 22–52 even
Step buttons:      pins 23–53 odd
Pattern LEDs:      pins 14–17     (LED[0]=pin17, LED[1]=pin16, LED[2]=pin15, LED[3]=pin14)
Pattern buttons:   pins 18–21     (btn[0]=pin21, btn[1]=pin20, btn[2]=pin19, btn[3]=pin18)
LCD:               Serial1, TX=pin1, 9600 baud
NOTE: LCD wire must connect to pin 1 (Serial1 TX). A15/pin61 cannot be UART TX
      on SAMD51 (PAD[3] is RX-only). SoftwareSerial is not available on SAMD51.
```

## Config Menu Navigation

The config menu is entered by **double-tapping `knob_mode_select`** (two presses within 400 ms). Navigation uses the hardware knobs as jog wheels:

| Control | Action |
|---------|--------|
| Tempo knob CW | Next menu item / increment value |
| Tempo knob CCW | Previous menu item / decrement value |
| Swing knob CCW | Cancel editing → cancel confirmation → exit menu |
| Swing knob CW | Exit menu (only when cursor is on "Exit" item) |
| param_rec | Select item / confirm |

Jog threshold: 20 ADC counts. `enter_knob_jog_mode()` anchors the baseline when the menu opens so the current knob position is not misread as movement.

**Menu items (in order):**
0. Exit
1. Save (blocked while playing — shows "Stop first!")
2. Clear pattern (confirmation required)
3. Clear all pats (confirmation required)
4. Reset sliders (confirmation required)
5. Mode: Simple / Advanced
6. Clock: int / ext
7. Channel (1–16)
8. Diagnostics (enters hardware test mode — stub in Phase 5)
9. Octave shift (±5 octaves)
10. Note shift (±12 semitones)
11. Note range (low / high MIDI note, two-phase editor)
12. Note scales (scale type + root, retroactive quantize)
13. Pat length (1–16, step buttons tap-to-set)
14. Pat dir (Fwd/Rev/Pong/Rand/Shuf/E/O/In/Quad)
15. CC number (per-pattern, valid range 1–119 skipping 32, 96–101)

Note: Swing is NOT a menu item — it is set by the hardware knob.

## Slider Modes

Cycled by single press of `slider_mode_select`:

| Mode | Slider controls |
|------|----------------|
| 1 — NN | MIDI note number → `voice_pitch[pattern][voice]` |
| 2 — VL | Velocity 0–127 → `voice_velocity[pattern][voice]` (0 = voice muted) |
| 3 — GT | Gate length 1–8 → `voice_gate[pattern][voice]` |
| 4 — CC | CC value 0–127 → `voice_cc_value[pattern][voice]` |

**Important**: Slider `j` always controls voice `j` — the mapping is fixed regardless of `current_voice`. All 8 voices are simultaneously adjustable. The **pickup guard** (`slider_needs_pickup[j]`) prevents a slider from overwriting stored data after a voice switch, mode switch, or pattern switch until the physical slider reaches the stored value (±1 tolerance for NN/VL/CC, exact for GT).

## LCD Display

Line 1 format: `P01 >03 V3 N 120` (16 chars)
- `P01` = pattern number
- `>03` = last triggered step (or `>--` if none)
- `V3` = current voice number
- `N` = slider mode (N/V/G/C)
- `120` = integer BPM (detected BPM in external clock mode)

Line 2 format: `?4036 ?2100 G1?3---` (16 chars) — always shows current voice's data:
- `?4` + 3-digit pitch
- `?2` + 3-digit velocity
- `G` + gate digit
- `?3` + 3-digit CC value or `---` if `voice_cc_enabled[current_voice]` is 0

LCD rate-limited to ~15 fps (66 ms) to prevent overflow at 9850 baud.

## Pattern Operations

- **Clear voice pattern**: Hold step 0 + step 15
- **Clear all patterns**: Hold step 0 + step 11
- **Copy pattern (simple mode)**: Hold a pattern select button for 2s, then press destination button
- **Copy pattern (advanced mode)**: Double-click pattern button 0 → phase 1: tap step = source → phase 2: tap step = destination
- **Cancel copy (advanced mode)**: Swing knob CCW (jog_h < 0) via `listen_for_navigation_events()`
- **Chain 4 patterns (simple mode)**: Press pattern buttons 0 + 3 simultaneously to toggle
- **Select/chain patterns (advanced mode)**: Pattern button 0 single-click → nav mode → tap steps

## Storage

**EEPROM** (magic `0xBE`, ~2605 bytes): stores all 16 patterns × 8 voices of step data, pitch, velocity, gate, CC value; also voice_cc_enabled, cc_number, and all config scalars. Velocities, gates, and CC data are all stored (unlike old Beatseqr firmware). Increment `EEPROM_MAGIC_VALUE` in `storage.ino` if you change the layout.

**SD card** (`/beatseqr/autosave.json`): primary storage. Per-pattern JSON with per-voice arrays (`pitches[8]`, `velocities[8]`, `gates[8]`, `cc_values[8]`, `steps_v0`…`steps_v7`). `voice_cc_enabled` stored once at top level. Hand-rolled parser, no ArduinoJson dependency.

**Boot**: `boot_load()` tries SD first, falls back to EEPROM. `save_everywhere()` writes SD + EEPROM.

## HAL Notes

**Button debouncing**: 50 ms hardware debounce in `isPressed()`. Use `wasPressed()` (no side effects) for combo checks after `uniquePress()` has already been called for those buttons in the same loop iteration.

**Potentiometer SAMD51 fix**: `getSector()` divides by `4096/sectors` (not `1024/sectors`). Do not revert to 1024 — it causes wrap-around artifacts at ~25% of slider travel.

**Button PULLUP on SAMD51**: Uses `pinMode(pin, INPUT_PULLUP)` — not the AVR `digitalWrite(HIGH)` trick.

**`isPressed()` vs `wasPressed()`**: Calling `isPressed()` twice in the same loop iteration can steal a CHANGED flag. Use `wasPressed()` for combo checks (chain toggle, clear combos) that run after `uniquePress()` was already called for those buttons.

## Pending Work

- **Phase 5**: `diagnostics.ino` — full hardware self-test mode for Beatseqr (resistor ladder display, slider readings, knob readings, all buttons)
- **ADC recalibration**: Voice select thresholds (`vselectval_lowerranges/upperranges`) should eventually be recalibrated at 12-bit native resolution (multiply current values by 4) if `analogReadResolution(12)` is ever desired
- **TRS MIDI output**: Hardware jack exists on board; implementation deferred
- **voice_mode_select button**: Currently reserved; no function assigned
- **knob_mode_select single tap**: Currently unused after double-tap detection window expires
