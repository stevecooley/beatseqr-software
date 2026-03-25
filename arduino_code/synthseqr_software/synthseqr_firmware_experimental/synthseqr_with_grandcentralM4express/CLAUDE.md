# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Synthseqr is Arduino firmware for a hardware MIDI step sequencer running on the **Adafruit Grand Central M4 Express** (ATSAMD51J19A ARM Cortex-M4). It's a 16-step, 4-pattern sequencer with physical step buttons, voice sliders, D-pad navigation, and an LCD display. Version 2.0x is experimental/in-progress.

## Build and Upload

This is an Arduino sketch. Open `synthseqr_with_grandcentralM4express.ino` in the Arduino IDE or use the Arduino CLI:

```bash
# Compile
arduino-cli compile --fqbn adafruit:samd:adafruit_grandcentral_m4 synthseqr_with_grandcentralM4express

# Upload (replace /dev/ttyACM0 with actual port)
arduino-cli upload -p /dev/ttyACM0 --fqbn adafruit:samd:adafruit_grandcentral_m4 synthseqr_with_grandcentralM4express
```

**Required library**: `MIDIUSB` (install via Library Manager). The `FifteenStep` library is custom and lives at `../../../libraries/FifteenStep/` relative to this sketch.

Serial monitor: 57600 baud for diagnostics output.

## Architecture

All `.ino` files are compiled as a single translation unit by Arduino. They share the globals declared in `config.h`.

**File responsibilities:**

| File | Role |
|------|------|
| `synthseqr_with_grandcentralM4express.ino` | `setup()` and `loop()` — polls all subsystems in order |
| `config.h` | All global state: pin definitions, arrays, library includes |
| `transport.ino` | Play/stop button, MIDI clock output |
| `midi_processor.ino` | MIDI input: clock sync (0xF8), start (0xFA), stop (0xFC) |
| `navigation.ino` | D-pad + enter button: adjusts tempo and swing |
| `step_button_routine.ino` | Step button presses, LED toggling, pattern clear |
| `voice_slider_routine.ino` | Reads 16 analog sliders → MIDI note numbers |
| `pattern_select_routine.ino` | Pattern switching, copy, and 4-pattern chain mode |
| `LCD.ino` | LCD initialization and display updates |
| `midi_note_sending.ino` | `stepsend()` callback: fires note on/off per step |
| `diagnostics.ino` | Hardware test mode (hold Play + Enter 2s to activate) |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers used throughout.

## Core State

```cpp
// In config.h:
bool step_data[4][1][16]           // [pattern][voice][step] — on/off for each step
uint8_t voice_slider_midinotenum[16] // MIDI note per slider (default 36–51)
uint8_t current_pattern            // Active pattern 0–3
bool playstatus                    // Is sequencer playing?
float TEMPO                        // BPM (10–250)
uint8_t SWING                      // 0–6
uint8_t lcdflag                    // LCD display mode selector
```

## Sequencer Flow

1. `seq.run()` (FifteenStep) ticks the sequencer on each `loop()` call
2. On each step change, `stepsend(current_step, last_step)` fires as a callback
3. `stepsend()` sends note-off for the previous step, note-on for the current step (if `step_data[pattern][0][step] == 1`), and updates chase lights
4. MIDI external sync: incoming 0xF8 clock pulses are counted; every 6 pulses advances one step

## Key Hardware Details

- **Step LEDs/buttons**: Paired on adjacent even/odd pins (LEDs 22–52 even, buttons 23–53 odd)
- **Voice sliders**: Analog pins A15 (slider 0) down to A0 (slider 15)
- **LCD**: `Serial1` at 9850 baud using a custom command protocol (`?f` = clear, `?x??y?` = cursor, `?B??` = backlight)
- **Pattern chain mode**: Patterns auto-advance when step 15 is reached (toggled by pressing pattern buttons 0+3 simultaneously)

## Navigation / Timing Modes

D-pad left/right cycles through 5 `timing_mode` values controlling what up/down adjusts:
1. ±10 BPM
2. ±1 BPM
3. ±0.1 BPM
4. ±0.01 BPM
5. Swing (0–6)

## Pattern Operations

- **Clear current pattern**: Hold step button 0 + step button 15
- **Clear all patterns**: Hold step button 0 + step button 11
- **Copy pattern**: Hold a pattern select button for 2s, then press destination pattern button
- **Chain 4 patterns**: Press pattern buttons 0 + 3 simultaneously to toggle
