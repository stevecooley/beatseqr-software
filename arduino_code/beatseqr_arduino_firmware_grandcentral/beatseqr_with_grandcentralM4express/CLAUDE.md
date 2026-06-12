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
| `midi_processor.ino` | MIDI input: clock sync (0xF8), start (0xFA), stop (0xFC), and **MIDI Learn** (0x0B CC capture → CC#) — `read_midi()` |
| `midi_note_sending.ino` | Low-level MIDI output helpers: `noteOn`, `noteOff`, `controlChange`, `clockStart`, `clockStop`, `clockPulse`, `midi()` FifteenStep callback |
| `navigation.ino` | `listen_for_navigation_events()` (adv copy cancel), `setExternalClockMode()` |
| `sequencer_timer.ino` | TC4 hardware timer driver: setup, period update, stop/start, ISR |
| `step_button_routine.ino` | Step button presses, LED toggling, pattern clear, `read_step_memory()`, `init_blank_patterns_to_range()` |
| `voice_slider_routine.ino` | Reads 8 voice sliders in NN/VL/GT/CC mode → `voice_pitch/velocity/gate/cc_value` |
| `voice_select_routine.ino` | Resistor-ladder voice select (A10), `set_current_voice()` |
| `knob_routine.ino` | Tempo knob (A8) jog-wheel for config menu; swing knob (A9) = configurable play control (`swing_knob_function`) + tempo-knob menu jog |
| `pattern_select_routine.ino` | Pattern switching, copy, chain mode, `go_to_pattern()` |
| `LCD.ino` | LCD initialization and display updates |
| `scales.ino` | Musical scale tables, `build_scale_notes()`, `quantize_to_scale()`, `apply_scale_to_all_patterns()` |
| `config_menu.ino` | Modal config menu: knob-jog navigation, all settings |
| `storage.ino` | EEPROM save/load (2733-byte layout, magic `0xBF`) |
| `sd_storage.ino` | SD card save/load: `/beatseqr/autosave.json`, `boot_load()`, `save_everywhere()` |
| `diagnostics.ino` | Hardware self-test: button test (buttons, knobs, voice-select ladder), slider test (one slider at a time), LED chase, SD save-file viewer |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers.

**Potentiometer / SAMD51 ADC resolution**: `Potentiometer.cpp` uses `analogRead() / (4096/sectors)` — do NOT use 1024 as the divisor. The `getSector()` method returns a 0–(sectors-1) value. Voice sliders use 255 sectors for full resolution. Swing knob uses 8 sectors.

**Voice select ADC**: `analogReadResolution(12)` globally. Thresholds in `vselectval_lowerranges/upperranges` are 12-bit (0–4095). Idle reads ~10. All 8 voices fully calibrated. Ladder: 2.2k+2.2k+3k+3.9k+4.7k+5.6k+6.8k+15k series resistors, 15k pull-down to GND.

Detection uses **unanimous consensus**: all VOICE_SELECT_SAMPLES reads must independently classify as the same voice before committing. Do NOT revert to averaging — averaging sweeps through lower voice ranges during button release and falsely triggers voice 8.

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
int step_data[16][VOICE_COUNT][16]          // [pattern][voice][step] — on/off
uint8_t voice_pitch[16][VOICE_COUNT]        // [pattern][voice] — MIDI note for each voice
uint8_t voice_velocity[16][VOICE_COUNT]     // [pattern][voice] — velocity (0=muted)
uint8_t voice_gate[16][VOICE_COUNT]         // [pattern][voice] — gate length 1–8 steps
uint8_t voice_cc_value[16][VOICE_COUNT]     // [pattern][voice] — CC value 0–127
uint8_t voice_cc_enabled[VOICE_COUNT]       // [voice] — per-voice CC enable (not per-step)
uint8_t cc_number[16]                       // [pattern] — CC controller number per pattern
uint8_t voice_probability[16][VOICE_COUNT]  // [pattern][voice] — fire probability 0–100% (default 100)

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
   - If `step_data[pat][v][play_step]` is on: checks `voice_probability[pat][v]` — fires only if `prob >= 100` or `random(100) < prob`
   - If the probability check passes AND velocity > 0: sends note-on, schedules note-off
   - If `voice_cc_enabled[v]` AND step active: sends CC message (CC fires regardless of probability)
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
| `knob_mode_select` single-tap | Back one level (cancel editing → cancel confirmation → close submenu → exit menu) |
| `knob_mode_select` double-tap | Exit menu entirely |
| param_rec | Select item / confirm |

The **swing knob does not navigate the menu** — it is a configurable play control (see Swing knob item / `swing_knob_function`). The single-tap back gesture fires after the 400 ms double-tap detection window via `knob_mode_back_flag`.

Jog threshold: `KNOB_JOG_THRESHOLD` = 80 ADC counts (12-bit). `enter_knob_jog_mode()` anchors the baseline when the menu opens so the current knob position is not misread as movement.

**Menu items (in order):**
0. Exit
1. Save (blocked while playing — shows "Stop first!")
2. Clear pattern (confirmation required)
3. Clear all pats (confirmation required)
4. Reset sliders (confirmation required)
5. Mode: Simple / Advanced
6. Clock: int / ext
7. Channel (1–16)
8. Diagnostics (opens the diag submenu: Button test / Slider test / LED test)
9. Octave shift (±5 octaves)
10. Note shift (±12 semitones)
11. Note range (low / high MIDI note, two-phase editor)
12. Note scales (scale type + root, retroactive quantize)
13. Pat length (1–16, step buttons tap-to-set)
14. Pat dir (Fwd/Rev/Pong/Rand/Shuf/E/O/In/Quad)
15. CC number (per-pattern, valid range 1–119 skipping 32, 96–101)
16. Tempo (integer BPM, editing sub-state)
17. Swing (0–5, editing sub-state)
18. Voice prob — exits menu immediately and activates slider mode 5 (PR); label shows `*` if any voice probability < 100 for the current pattern
19. Takeover (Catch / Jump / Relative — slider pickup behaviour; editing sub-state)
20. Swing knob (Swing / Tempo / Patt / Voice / Note — what the swing knob does during play; editing sub-state). **Note** mode is a workaround for failing hardware: the swing knob itself is NOT used — instead **voice 1's slider (slider 0)** sets the *selected* voice's `voice_pitch` across the configurable **Note range** (`slider_map_low_value`–`slider_map_high_value`, the same range NN slider mode uses; set via config menu → Note range), the slider's full travel (raw 0–4095) spanning that range. Implemented in `run_note_slider_override()` (`voice_slider_routine.ino`), called from `loop()`; it runs even when `ft_voice_sliders` is off (only slider 0 is read). A jump guard re-arms on each selected-voice change so switching voices doesn't clobber their notes until slider 0 physically moves. While Note mode is active, slider 0 is skipped by the normal slider routine.
21. Features (opens the Features submenu of `ft_*` flags)

Items whose feature flag is disabled are skipped during knob-jog scrolling (`config_menu_step()` / `config_item_enabled()` in `config_menu.ino`).

Note: Swing is also editable via the swing knob when **Swing knob** = Swing — the menu item and knob edit the same value.

## Features Submenu

Opened from config menu → **Features**. Knob-jog navigation (tempo knob scrolls, `param_rec` toggles, swing knob CCW exits to the main menu). Each `ft_*` flag enables an optional capability; when off, its config item(s) are hidden and, for slider modes, the slider-mode cycle (`next_slider_mode()`) and advanced-mode pattern shortcuts skip it. Toggling a flag off applies side-effects via `_apply_feature_disable()` (e.g. dropping an active disabled slider mode back to NN, forcing Simple mode). Flags are NOT auto-saved — use Save after changing them.

| Feature        | Flag                     | Gates |
|----------------|--------------------------|-------|
| Advanced mode  | `ft_advanced_mode`       | Mode item + advanced pattern layout |
| CC mode        | `ft_cc_mode`             | slider mode 4 (CC) + CC num item |
| Probability    | `ft_probability`         | slider mode 5 (PR) + Voice prob item |
| Gate sliders   | `ft_gate_mode`           | slider mode 3 (GT) |
| Velocity       | `ft_velocity_mode`       | slider mode 2 (VL) |
| Note scales    | `ft_scale_quantization`  | Note scales item |
| Pat direction  | `ft_pattern_direction`   | Pat dir item |
| Pat length     | `ft_variable_pat_length` | Pat length item |
| Ext clock      | `ft_external_clock`      | Clock item |
| Oct/note shift | `ft_octave_note_shift`   | Octave shift + Note shift items |
| Diagnostics    | `ft_diagnostics`         | Diagnostics item |
| Voice sliders  | `ft_voice_sliders`       | When off, `run_voice_slider_routine()` skips all reads (faulty/noisy sliders can't overwrite data); set notes via swing-knob Note mode |

NN (slider mode 1) is always enabled. Note range is always visible (not gated).

## MIDI Learn

While editing the per-pattern CC# (config menu → CC num, in editing sub-state), an incoming USB-MIDI Control Change message sets `cc_number[pattern_value]` to that controller number. Channel is ignored; filtered CCs (32, 96–101) are skipped and the edit stays armed. Implemented as the `0x0B` branch in `read_midi()` (`midi_processor.ino`). No explicit arm gesture; no MIDI thru is added.

## Slider Modes

Cycled by single press of `slider_mode_select`:

| Mode | Slider controls |
|------|----------------|
| 1 — NN | MIDI note number → `voice_pitch[pattern][voice]` |
| 2 — VL | Velocity 0–127 → `voice_velocity[pattern][voice]` (0 = voice muted) |
| 3 — GT | Gate length 1–8 → `voice_gate[pattern][voice]` |
| 4 — CC | CC value 0–127 → `voice_cc_value[pattern][voice]` |
| 5 — PR | Fire probability 0–100% → `voice_probability[pattern][voice]` |

PR mode is accessed via config menu → **Voice prob** (exits the menu and sets `slider_mode = 5`). Slider j controls voice j's probability for the current pattern. Default 100 = always fires. At 0 the voice is silenced; at 50 it fires roughly half the time.

**Important**: Slider `j` always controls voice `j` — the mapping is fixed regardless of `current_voice`. All 8 voices are simultaneously adjustable.

**Slider takeover** (`slider_takeover`, set via config menu → Takeover): governs how a slider re-engages after a mode/voice/pattern switch. Logic lives in `slider_take()` in `voice_slider_routine.ino`.
- **0 = Catch** (default): the **pickup guard** (`slider_needs_pickup[j]`) blocks writes until the physical slider reaches the stored value (±1 tolerance for NN/VL/CC/PR, exact for GT).
- **1 = Jump**: pickup is armed but unlocks on any movement past `JUMP_RAW_THRESHOLD` (32 ADC counts) from the seed position, then the value jumps to the slider.
- **2 = Relative**: pickup is bypassed; each read adds the scaled ADC delta (`raw_delta / adc_per_unit`) to the stored value, carrying sub-unit motion in `slider_last_raw[j]`. NN with an active scale moves by indices through `scale_note_pool`.

`slider_last_raw[VOICE_COUNT]` is seeded from the physical positions on every mode/voice/pattern switch (and EEPROM load) so Jump/Relative don't see a phantom delta.

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
- **Cancel copy (advanced mode)**: Press param_rec (Enter) while a copy phase is active, via `listen_for_navigation_events()`
- **Chain 4 patterns (simple mode)**: Press pattern buttons 0 + 3 simultaneously to toggle
- **Select/chain patterns (advanced mode)**: Pattern button 0 single-click → nav mode → tap steps

## Storage

**EEPROM** (magic `0xC2`, ~2747 bytes): stores all 16 patterns × 8 voices of step data, pitch, velocity, gate, CC value, and probability; also voice_cc_enabled, cc_number, `slider_takeover` (addr 2733), the 12 `ft_*` feature flags (addr 2734), `swing_knob_function` (addr 2746), and all config scalars. Increment `EEPROM_MAGIC_VALUE` in `storage.ino` if you change the layout. Old saves with a prior magic are automatically rejected and replaced with defaults (was `0xC1` before `ft_voice_sliders`, `0xC0` before swing_knob_function, `0xBF` before takeover + feature flags).

**SD card** (`/beatseqr/autosave.json`): primary storage. Per-pattern JSON with per-voice arrays (`pitches[8]`, `velocities[8]`, `gates[8]`, `cc_values[8]`, `probabilities[8]`, `steps_v0`…`steps_v7`). `voice_cc_enabled` stored once at top level, along with `slider_takeover`, `swing_knob_function`, and the `ft_*` flags. Hand-rolled parser, no ArduinoJson dependency. `probabilities`, `slider_takeover`, `swing_knob_function`, and all `ft_*` keys (including `ft_voice_sliders`) are optional in the JSON — older saves without them load cleanly and default to 100 / Catch / Swing / ON respectively.

**Boot**: `boot_load()` tries SD first, falls back to EEPROM. `save_everywhere()` writes SD + EEPROM.

## HAL Notes

**Button debouncing**: 50 ms hardware debounce in `isPressed()`. Use `wasPressed()` (no side effects) for combo checks after `uniquePress()` has already been called for those buttons in the same loop iteration.

**Potentiometer SAMD51 fix**: `getSector()` divides by `4096/sectors` (not `1024/sectors`). Do not revert to 1024 — it causes wrap-around artifacts at ~25% of slider travel.

**Button PULLUP on SAMD51**: Uses `pinMode(pin, INPUT_PULLUP)` — not the AVR `digitalWrite(HIGH)` trick.

**`isPressed()` vs `wasPressed()`**: Calling `isPressed()` twice in the same loop iteration can steal a CHANGED flag. Use `wasPressed()` for combo checks (chain toggle, clear combos) that run after `uniquePress()` was already called for those buttons.

## Diagnostics Mode

Entered from config menu → **Diagnostics**, which opens a knob-jog submenu (Button test / Slider test / LED test). `diag_mode` gates the main loop: `loop()` runs `run_diagnostics()` then `if (diag_mode) return;`, and `run_LCD_update()` early-returns, so diagnostics owns the display. `diag_submode` selects button test (0), slider test (1), or LED test (2).

- **Button test** (`run_diag_button_test()`): reports each button (step, pattern, play, param_rec, slider/voice/knob-mode selects), the two tempo/swing knobs (raw ADC, ±16 threshold, 100 ms rate-limit), and the voice-select resistor ladder (raw A10 + detected voice via a local copy of the classifier). Auto-clears to idle after 2 s. **Pattern button 0 (PAT1)** opens the save-file viewer (scroll `autosave.json` top-level fields with the tempo knob; `sd_diag_load_fields()` in `sd_storage.ino`).
- **Slider test** (`run_diag_slider_test()`): watches a single slider at a time so a noisy slider can't drown out the others. Press a **voice-select button (1–8)** to choose which slider to display; the focused voice LED lights and line 1 shows `SLIDER V<n>  <pin>`, line 2 shows the live raw value (100 ms rate-limit). Focus stays on the last-picked slider until another voice-select button is pressed (starts on `current_voice`). Knobs and the voice-select ladder readout are not in this test — they live in the button test, since voice-select is repurposed here for slider focus.
- **LED test** (`run_diag_led_test()`): non-blocking chase across 16 step + 4 pattern + 8 voice + play LEDs (29 total), 80 ms/LED.
- **Exit** (all tests): double-tap `param_rec` → back to the diag submenu. (No D-pad on Beatseqr, so the synthseqr "Left exits" gesture is replaced by the param_rec double-tap.)

## Pending Work

- **ADC recalibration**: Voice select thresholds (`vselectval_lowerranges/upperranges`) should eventually be recalibrated at 12-bit native resolution (multiply current values by 4) if `analogReadResolution(12)` is ever desired
- **TRS MIDI output**: Hardware jack exists on board; implementation deferred
- **voice_mode_select button**: Currently reserved; no function assigned
- **knob_mode_select single tap**: Inside the config menu = back one level (`knob_mode_back_flag`); outside the menu it is still unused
