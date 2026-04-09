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
| `diagnostics.ino` | Hardware test mode (hold D-pad left + right 1s to enter/exit) |
| `storage.ino` | EEPROM save/load: `save_to_eeprom()`, `load_from_eeprom()` |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers used throughout.

**Button debouncing**: `Button.h`/`Button.cpp` are sketch-local copies with a 50 ms hardware debounce in `isPressed()`. A `_debounce_until` timestamp gates all state transitions — during the window `CHANGED` is suppressed and the cached state is returned. This prevents electrical bounce from firing `uniquePress()` multiple times per physical press. Call `setDebounceDelay(ms)` to override the default per-instance.

**Button pull-up on SAMD51**: `Button::pullup()` uses `pinMode(pin, INPUT_PULLUP)` — NOT the legacy AVR `digitalWrite(pin, HIGH)` trick. The AVR trick does not reliably enable the internal pull-up on SAMD51 (Cortex-M4). Without a valid pull-up the input pin floats, causing random noise reads and unreliable button detection. Do not revert to `digitalWrite(HIGH)` here.

**Step button / LED decoupling**: `detect_step_button_presses()` toggles `step_data[pattern_value][0][i]` directly and then calls `step_leds[i].on()` / `step_leds[i].off()` to match. It does NOT read `step_leds[i].getState()` as the source of truth. The chase light in `run_chase_lights()` inverts the current step's LED, so reading LED state would cancel out a button press on the currently-playing step. Always keep step toggle logic coupled to `step_data`, not LED state.

**`isPressed()` vs `wasPressed()` for combo checks**: Each call to `isPressed()` updates internal state (PREVIOUS, CHANGED, debounce window). Calling it a second time on the same button within the same loop iteration — after `uniquePress()` already ran — can steal a CHANGED flag and cause the next loop's `uniquePress()` to miss a press. For combo checks (step-clear, chain toggle) that run after `uniquePress()` has already been called for those buttons, always use `wasPressed()` instead. `wasPressed()` reads the cached CURRENT bit with no side effects.

## Core State

```cpp
// In config.h:
bool step_data[4][1][16]             // [pattern][voice][step] — on/off for each step
uint8_t voice_slider_midinotenum[16] // MIDI note per slider (default 36–51)
uint8_t current_pattern              // Active pattern 0–3
bool playstatus                      // Is sequencer playing?
float TEMPO                          // BPM (10–250)
uint8_t SWING                        // 0–5
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

D-pad left/right cycles through 8 `timing_mode` values controlling what up/down adjusts, in visual left-to-right order matching the LCD layout:

| Mode | Up/Down adjusts | LCD location |
|------|----------------|--------------|
| 1 | Pattern (1–4, wraps) | Line 1, column 2 |
| 2 | Tempo ±10 BPM | Line 1, column 6 |
| 3 | Tempo ±1 BPM | Line 1, column 7 |
| 4 | Tempo ±0.1 BPM | Line 1, column 9 |
| 5 | Tempo ±0.01 BPM | Line 1, column 10 |
| 6 | Swing (0–5) | Line 2, column 1 |
| 7 | Clock source (up=EXT, down=INT) | Line 2, column 7 |
| 8 | MIDI channel (1–16) | Line 2, column 12 |

Default `timing_mode = 2` (±10 BPM).

**LCD line 1 format** (case 255): `[icon] P%u T%6.2f    ` where `[icon]` is custom char `?0` (play) or `?7` (stop) printed first (1 char), then the 15-char sprintf result. Example: `▶ P1 T120.00    `. Do not change `%6.2f` to `%.2f` — variable width breaks cursor alignment. `go_to_pattern()` sets `update_line1 = true` so the pattern number refreshes on every pattern switch.

**LCD line 2 format** (case 255): `s%d clk:%s Ch%02d ` (exactly 16 chars). Example: `s0 clk:int Ch02 `. MIDI channel is on line 2; `midi_channel_events()` sets `update_line2 = true`.

**LCD cursor positions** are defined as named constants in config.h — use these instead of hardcoded numbers:
- `LCD_L1_X_PATTERN = 3` — pattern digit (after icon+space+'P')
- `LCD_L1_X_TEMPO_10 = 7` — hundreds/tens digit of tempo
- `LCD_L1_X_TEMPO_1 = 8` — units digit
- `LCD_L1_X_TEMPO_01 = 10` — tenths digit (after decimal at col 9)
- `LCD_L1_X_TEMPO_001 = 11` — hundredths digit
- `LCD_L2_X_SWING = 1` — swing digit after 's'
- `LCD_L2_X_CLOCK = 7` — first char of int/ext in "clk:%s"
- `LCD_L2_X_MIDICHAN = 13` — first channel digit in "Ch%02d"

Switching clock source calls `setExternalClockMode()` which stops or starts TC4 as needed.

**Enter button** clears the LCD and sets both `update_line1 = true` and `update_line2 = true` so both lines redraw. Missing `update_line2` here would leave line 2 blank after clear.

**Cursor restore after redraw**: `run_LCD_update()` case 255 tracks a local `did_redraw` flag. If either `update_line1` or `update_line2` was processed that frame, the cursor is repositioned to `cursor_x`/`cursor_y` afterward — even if `cursor_flag` is false. This ensures that events like play/stop toggling line 1 or pattern switches don't leave the cursor stranded after the redraw. All transient message cases (93, 101, 200, 201, 202) set `update_line1 = true` and `update_line2 = true` before returning to `next_lcdflag = 255` for the same reason.

**`lcdflag` / `next_lcdflag` rule**: `run_LCD_update()` starts every frame with `if (next_lcdflag != lcdflag) lcdflag = next_lcdflag`. Any code that sets `lcdflag` externally (outside `run_LCD_update`) **must also set `next_lcdflag` to the same value** — otherwise `next_lcdflag` (still 255 from the previous frame) immediately overwrites `lcdflag` before the switch runs and the message is never displayed. This applies to every transient message trigger: save (202), chain single/4 (200/201), pattern copy (100/101), slider reset (93).

## Swing Implementation

`seq.setShuffle()` is a no-op in hardware timer mode — FifteenStep only applies shuffle in its software polling path. Swing is instead implemented directly in `stepsend()` by adjusting the TC4 clock period after each step:

- **Even step just fired**: set TC4 period to `base_us * (6 + SWING) / 6` — the next (odd) step arrives late
- **Odd step just fired**: set TC4 period to `base_us * (6 - SWING) / 6` — the next (even) step arrives early

The total time per pair of steps stays constant (`(6+SWING) + (6-SWING) = 12` base periods = 2 × 16th notes). When `SWING == 0` both paths produce `base_us * 6/6 = base_us`, giving straight timing identical to the previous behaviour. Only applied in internal clock mode (`!external_clock_mode`).

Because TC4's period changes, the MIDI clock output (0xF8) also swings. Avoid using MIDI clock output to sync external devices when SWING > 0.

`SWING` range 0–5: SWING=1 is mild, SWING=2 is 2:1 (classic triplet feel), SWING=3 is 3:1 (heavy). SWING=5 is maximum (SWING=6 produced unusable timing and was removed).

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

**`go_to_pattern(pattern, silent)`**: Turns all 4 pattern LEDs off then calls `on()` (not `toggle()`) for the active pattern. `toggle()` was previously used but is state-dependent and misfires if the LED state is out of sync. Always use `on()` here. The `silent` parameter is accepted but currently unused.

**Chain toggle one-shot guard**: The `isPressed()` check for pattern buttons 0+3 fires every loop iteration while both are held. A `static bool chain_toggle_handled` in `run_pattern_select_routine()` ensures the mode flip and `go_to_pattern()` call happen only once per press. It resets when the buttons are released. Do not remove this guard — without it the mode flips back and forth on every loop frame.

**`clear_pattern_memory()` clears all 4 patterns**: The step 0+11 combo calls `clear_pattern_memory()`, which loops over all 4 patterns (`p = 0..3`) and zeros every step. After clearing, it calls `read_step_memory(0, pattern_value)` to refresh the LEDs for the active pattern. Do not scope this loop to only `pattern_value` — that was a prior bug where only the active pattern was cleared.

**Pattern chain auto-advance**: When `extended_step_length_mode == 1`, `stepsend()` calls `run_auto_pattern_select_routine()` at `current_step == 15`. This advances `current_pattern` via `(current_pattern + 1) % 4`, cycling all four patterns. The advance happens at the start of step 15 so step 15 plays from the current pattern; the new pattern takes over from step 0.

## EEPROM Save / Load

**Trigger**: Hold Enter for 2 seconds (without Play held — Play+Enter was the old diagnostics combo; the new combo is D-pad left+right). The LCD shows `saved!` on line 1 for 2 seconds as confirmation. Implemented in `navigation.ino` with a `static bool save_handled` one-shot guard, same pattern as chain toggle.

**On boot**: `load_from_eeprom()` is called in `setup()` after `seq.begin()`. It checks for a magic sentinel byte (`0xBE` at address 0). If missing (first boot), globals keep their compiled-in defaults. If present, all state is restored before `go_to_pattern()` and `setupSequencerTimer()` run, so the timer uses the saved TEMPO.

**EEPROM layout** (138 bytes, defined as `#define` constants in `storage.ino`):

| Address | Size | Content |
|---------|------|---------|
| 0 | 1 | Magic byte `0xBE` |
| 1 | 1 | `MIDICHANNEL` |
| 2 | 1 | `SWING` |
| 3 | 4 | `TEMPO` (float) |
| 7 | 1 | `current_pattern` |
| 8 | 1 | `extended_step_length_mode` |
| 9 | 1 | `external_clock_mode` |
| 10 | 64 | `step_data[4][16]` (1 byte per step) |
| 74 | 64 | `pattern_step_pitches[4][16]` |

**Validation**: All loaded values are range-checked (`TEMPO` 10–250, `MIDICHANNEL` 1–16, etc.) so corrupted flash can't put the sequencer in a broken state.

**`EEPROM.commit()` is required**: `storage.ino` uses `FlashAsEEPROM_SAMD`. All `EEPROM.write()` / `EEPROM.put()` calls only update a RAM buffer. `EEPROM.commit()` at the end of `save_to_eeprom()` is what actually burns the buffer to flash. Without it, saves appear to work within a session (reads hit the buffer) but vanish on power-off.

**Flash endurance**: SAMD51 NVM is rated 25,000 write cycles per page minimum. The Arduino EEPROM emulation uses wear leveling across multiple pages, giving an effective endurance far beyond typical use. Save-on-demand (not continuous) makes exhaustion essentially impossible in practice.

**Magic byte**: If the EEPROM layout changes (new fields, reordered addresses), increment `EEPROM_MAGIC_VALUE` in `storage.ino` so old saves are ignored on first boot rather than misread.

**LCD confirmation**: `lcdflag = 202` shows `saved!` for 2 seconds using a `static unsigned long msg_until` timer inside the LCD case, then returns to the main display. This is the same pattern used for all timed LCD messages.
