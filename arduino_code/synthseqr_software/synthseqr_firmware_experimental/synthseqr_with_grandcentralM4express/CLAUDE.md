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
| `transport.ino` | Play/stop button, MIDI clock output, `stepsend()` callback, `allNotesOff()` |
| `midi_processor.ino` | MIDI input: clock sync (0xF8), start (0xFA), stop (0xFC) |
| `navigation.ino` | D-pad + enter button: adjusts tempo, swing, and clock source |
| `sequencer_timer.ino` | TC4 hardware timer driver: setup, period update, stop/start, ISR |
| `step_button_routine.ino` | Step button presses, LED toggling, pattern clear |
| `voice_slider_routine.ino` | Reads 16 analog sliders → MIDI note numbers |
| `pattern_select_routine.ino` | Pattern switching, copy, and 4-pattern chain mode |
| `LCD.ino` | LCD initialization and display updates |
| `midi_note_sending.ino` | `step()` blink callback and `midi()` MIDI clock output callback |
| `diagnostics.ino` | Hardware test mode (hold Play + Enter 2s to activate) |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers used throughout.

**Button debouncing**: `Button.h`/`Button.cpp` are sketch-local copies with a 20 ms hardware debounce added to `isPressed()`. A `_debounce_until` timestamp gates all state transitions — during the window `CHANGED` is suppressed and the cached state is returned. This prevents electrical bounce from firing `uniquePress()` multiple times per physical press. Call `setDebounceDelay(ms)` to override the default per-instance.

## Core State

```cpp
// In config.h:
bool step_data[4][1][16]             // [pattern][voice][step] — on/off for each step
uint8_t voice_slider_midinotenum[16] // MIDI note per slider (default 36–51)
uint8_t current_pattern              // Active pattern 0–3
bool playstatus                      // Is sequencer playing?
float TEMPO                          // BPM (10–250)
uint8_t SWING                        // 0–6
uint8_t lcdflag                      // LCD display mode selector
bool external_clock_mode             // false = internal TC4, true = follow USB-MIDI clock
int8_t sounding_notes[16]           // pitch currently sounding per step (-1 = silent)
```

## Sequencer Flow

1. `seq.run()` (FifteenStep) ticks the sequencer on each `loop()` call
2. Timing is driven by the **TC4 hardware timer** (internal mode) or **incoming USB-MIDI 0xF8** (external mode) — both call `seq.hardwareClockPulse()` which sets volatile flags; `seq.run()` processes those flags in main-loop context
3. On each step change, `stepsend(current_step, last_step)` fires as the step callback
4. `stepsend()` sends note-off for the previous step using `sounding_notes[last_step]` (the exact pitch that was played), note-on for the current step, records the new pitch in `sounding_notes[current_step]`, and updates chase lights
5. On stop (play button or MIDI stop), `allNotesOff()` sends note-off for every entry in `sounding_notes[]` and clears the array

## Hardware Timer (TC4)

The sequencer uses the SAMD51's TC4 peripheral in 16-bit MFRQ mode for drift-free timing. Key details:

- Clock: GCLK0 (120 MHz) / prescaler 1024 → 8.533 µs/tick
- The ISR (`TC4_Handler`) only sets two `volatile bool` flags — no USB or MIDI in ISR context
- `setupSequencerTimer(us)` — call once from `setup()`
- `setSequencerTimerPeriod(us)` — call after every `seq.setTempo()` to update without stopping
- `resetSequencerTimerSync()` — resets TC4 counter to zero on play-start for phase alignment
- `stopSequencerTimer()` / `startSequencerTimer()` — used when switching to/from external clock mode
- CMSIS-Atmel 1.2.2 (Adafruit SAMD 1.7.17) is missing `MCLK_APBDMASK_TC4`; the raw bit `(1ul << 5)` is used instead (SAMD51P20A datasheet Table 14-8)

## Key Hardware Details

- **Step LEDs/buttons**: Paired on adjacent even/odd pins (LEDs 22–52 even, buttons 23–53 odd)
- **Voice sliders**: Analog pins A15 (slider 0) down to A0 (slider 15)
- **LCD**: `Serial1` at 9850 baud using a custom command protocol (`?f` = clear, `?x??y?` = cursor, `?B??` = backlight). `run_LCD_update()` is rate-limited to 66 ms (≈15 fps) to prevent buffer overflow at 9850 baud. All LCD writes go through this function — never call `lcd.print()` directly from other routines.
- **Pattern chain mode**: Patterns auto-advance when step 15 is reached (toggled by pressing pattern buttons 0+3 simultaneously). The toggle is non-blocking — no `delay()` calls; LED state changes and `go_to_pattern()` execute immediately.

## Navigation / Timing Modes

D-pad left/right cycles through 6 `timing_mode` values controlling what up/down adjusts:
1. ±10 BPM
2. ±1 BPM
3. ±0.1 BPM
4. ±0.01 BPM
5. Swing (0–6)
6. Clock source (up = EXT, down = INT)

Modes 5 and 6 are both shown on LCD line 2 as `s0 clk:int` / `s0 clk:ext`. The cursor sits on the swing digit in mode 5, and on the `int`/`ext` value in mode 6. Switching clock source via mode 6 calls `setExternalClockMode()` which stops or starts TC4 as needed.

**LCD line 1 format** (case 255): `[icon] C%02d [vmicon]%u T%6.2f` where `[icon]` is custom char `?0` (play) or `?7` (stop) and `[vmicon]` is custom char `?6`. The tempo uses `%6.2f` (right-aligned, 6 chars wide) so cursor positions are constant across all BPM values — `cursor_x` 11/12/14/15 for modes 1–4 are correct for any tempo from 10–250 BPM. Do not change to `%.2f` (variable width breaks cursor alignment for sub-100 BPM).

**LCD cursor positions on line 0 (y=0):**
- Mode 1 (±10): `cursor_x = 11` — tens digit of tempo
- Mode 2 (±1): `cursor_x = 12` — units digit
- Mode 3 (±0.1): `cursor_x = 14` — tenths digit
- Mode 4 (±0.01): `cursor_x = 15` — hundredths digit

**LCD cursor positions on line 1 (y=1):**
- Mode 5 (swing): `cursor_x = 1` — SWING digit after `s`
- Mode 6 (clock): `cursor_x = 7` — first char of `int`/`ext`

**Enter button** clears the LCD and sets both `update_line1 = true` and `update_line2 = true` so both lines redraw. Missing `update_line2` here would leave line 2 blank after clear.

## External MIDI Clock Mode

When `external_clock_mode == true`:
- TC4 is stopped (`stopSequencerTimer()`)
- Incoming USB-MIDI 0xF8 bytes in `read_midi()` call `seq.hardwareClockPulse()` directly
- Transport messages (0xFA start, 0xFC stop) still start/stop the sequencer
- The sequencer does NOT output MIDI clock or transport messages (it is a follower)
- The play button still works as a local arm/start

When `external_clock_mode == false` (default):
- TC4 drives `hardwareClockPulse()` from its ISR at the rate set by `TEMPO`
- Sequencer outputs MIDI clock (0xF8), start (0xFA), and stop (0xFC)

## Pattern Operations

- **Clear current pattern**: Hold step button 0 + step button 15
- **Clear all patterns**: Hold step button 0 + step button 11
- **Copy pattern**: Hold a pattern select button for 2s, then press destination pattern button
- **Chain 4 patterns**: Press pattern buttons 0 + 3 simultaneously to toggle
