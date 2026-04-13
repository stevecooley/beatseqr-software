# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Synthseqr is Arduino firmware for a hardware MIDI step sequencer running on the **Adafruit Grand Central M4 Express** (ATSAMD51J19A ARM Cortex-M4). It's a 16-step, 16-pattern sequencer with physical step buttons, voice sliders, D-pad navigation, and an LCD display. Version 2.3 is experimental/in-progress.

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
| `sd_storage.ino` | SD card save/load: `save_to_sd()`, `load_from_sd()`, `boot_load()`, `save_everywhere()` |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers used throughout.

**Potentiometer / SAMD51 ADC resolution**: `Potentiometer.cpp` was originally written for AVR (10-bit ADC, 0–1023). The SAMD51 uses a 12-bit ADC (0–4095). `getSector()` divides `analogRead()` by `4096/sectors` — not `1024/sectors`. Using 1024 causes the sector value to wrap at ~25% of slider travel, producing a peak-then-drop artifact. Do not revert to 1024.

**Button debouncing**: `Button.h`/`Button.cpp` are sketch-local copies with a 50 ms hardware debounce in `isPressed()`. A `_debounce_until` timestamp gates all state transitions — during the window `CHANGED` is suppressed and the cached state is returned. This prevents electrical bounce from firing `uniquePress()` multiple times per physical press. Call `setDebounceDelay(ms)` to override the default per-instance.

**Button pull-up on SAMD51**: `Button::pullup()` uses `pinMode(pin, INPUT_PULLUP)` — NOT the legacy AVR `digitalWrite(pin, HIGH)` trick. The AVR trick does not reliably enable the internal pull-up on SAMD51 (Cortex-M4). Without a valid pull-up the input pin floats, causing random noise reads and unreliable button detection. Do not revert to `digitalWrite(HIGH)` here.

**Step button / LED decoupling**: `detect_step_button_presses()` toggles `step_data[pattern_value][0][i]` directly and then calls `step_leds[i].on()` / `step_leds[i].off()` to match. It does NOT read `step_leds[i].getState()` as the source of truth. The chase light in `run_chase_lights()` inverts the current step's LED, so reading LED state would cancel out a button press on the currently-playing step. Always keep step toggle logic coupled to `step_data`, not LED state.

**`isPressed()` vs `wasPressed()` for combo checks**: Each call to `isPressed()` updates internal state (PREVIOUS, CHANGED, debounce window). Calling it a second time on the same button within the same loop iteration — after `uniquePress()` already ran — can steal a CHANGED flag and cause the next loop's `uniquePress()` to miss a press. For combo checks (step-clear, chain toggle) that run after `uniquePress()` has already been called for those buttons, always use `wasPressed()` instead. `wasPressed()` reads the cached CURRENT bit with no side effects.

## Core State

```cpp
// In config.h:
int step_data[16][1][16]              // [pattern][voice][step] — on/off for each step
uint8_t pattern_step_pitches[16][16]  // saved pitch per pattern per step
uint8_t voice_slider_midinotenum[16]  // MIDI note per slider (default 36–51)
uint8_t current_pattern               // Active pattern 0–15
bool playstatus                       // Is sequencer playing?
float TEMPO                           // BPM (10–250)
uint8_t SWING                         // 0–5
uint8_t lcdflag                       // LCD display mode selector
bool external_clock_mode              // false = internal TC4, true = follow USB-MIDI clock
int8_t sounding_notes[16]            // pitch currently sounding per step (-1 = silent)
bool advanced_mode                    // false = Simple (4 patterns, buttons select), true = Advanced (16 patterns, buttons are function keys)
bool adv_pat_nav_active               // true while pattern-nav mode is active; step buttons select/chain patterns instead of editing steps
bool adv_copy_waiting_source          // true while copy phase 1: waiting for step tap to pick the source pattern
bool adv_copy_armed                   // true while copy phase 2: source = current_pattern; waiting for step tap = destination
int8_t adv_chain_hold_step            // step button held in nav mode for chain definition (-1 = none)
unsigned long adv_blink_last_ms       // last blink toggle timestamp in nav mode
bool adv_blink_state                  // current blink state for the playing pattern's LED in nav mode
int8_t octave_shift                   // semitone offset applied at MIDI send time; range -5 to +5 octaves
int8_t note_shift                     // additional semitone offset applied at MIDI send time; range -12 to +12
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

## Physical Layout

The enclosure is 3D-printed with a honeycomb texture. Controls from left to right / top to bottom:

- **Top-left**: Play button (single yellow/green LED, pin 21) + LCD (16×2, Serial1)
- **Top-center**: D-pad (up/down/left/right, pins 16–19) + Enter button (pin 20) with LED
- **Top-right**: 4 pattern select buttons (pins 6/8/14/15) with yellow/green LEDs
- **Middle**: 16 faders (analog pins A0–A15), one per step
- **Bottom row**: 16 step buttons (pins 23–53 odd) paired with red LEDs (pins 22–52 even)

## Key Hardware Details

- **Step LEDs/buttons**: Paired on adjacent even/odd pins (LEDs 22–52 even, buttons 23–53 odd)
- **Voice sliders**: Analog pins A15 (slider 0) down to A0 (slider 15)
- **LCD**: `Serial1` at 9850 baud using a custom command protocol (`?f` = clear, `?x??y?` = cursor, `?B??` = backlight). `run_LCD_update()` is rate-limited to 66 ms (≈15 fps) to prevent buffer overflow at 9850 baud. All LCD writes go through this function — never call `lcd.print()` directly from other routines.
- **Pattern chain mode**: Patterns auto-advance when step 15 is reached (toggled by pressing pattern buttons 0+3 simultaneously). The toggle is non-blocking — no `delay()` calls; LED state changes and `go_to_pattern()` execute immediately.

## Play Button Hardware Interrupt

The play button (pin 21) is attached to a SAMD51 EIC external interrupt (FALLING edge) so it responds immediately regardless of main loop timing. Key details:

- `attachInterrupt(digitalPinToInterrupt(21), playButtonISR, FALLING)` called in `setup()`
- The ISR sets a `volatile bool play_button_isr_fired` flag — no USB or MIDI in ISR context
- A 100 ms software debounce (`play_button_last_isr_ms`) in the ISR suppresses contact bounce
- `listen_for_transport_events()` checks the flag each loop and processes play/stop logic there
- `playbutton.isPressed()` is still called each loop to keep `heldFor()` state current for diagnostics combo detection
- Do NOT increase debounce beyond ~150 ms or quick tap-to-stop will feel sluggish

## Navigation / Timing Modes

D-pad left/right cycles through 8 `timing_mode` values controlling what up/down adjusts, in visual left-to-right order matching the LCD layout:

| Mode | Up/Down adjusts | LCD location |
|------|----------------|--------------|
| 1 | Pattern (1–4 simple / 1–16 advanced, wraps) | Line 1, column 2 |
| 2 | Tempo ±10 BPM | Line 1, column 6 |
| 3 | Tempo ±1 BPM | Line 1, column 7 |
| 4 | Tempo ±0.1 BPM | Line 1, column 9 |
| 5 | Tempo ±0.01 BPM | Line 1, column 10 |
| 6 | Swing (0–5) | Line 2, column 1 |
| 7 | Clock source (up=EXT, down=INT) | Line 2, column 7 |
| 8 | MIDI channel (1–16) | Line 2, column 12 |

Default `timing_mode = 2` (±10 BPM).

**LCD line 1 format** (case 255): `[icon] P%02u T%6.2f   ` where `[icon]` is custom char `?0` (play) or `?7` (stop) printed first (1 char), then the 15-char sprintf result. Example: `▶ P01 T120.00  `. Pattern is always 2 digits (P01–P16). Do not change `%6.2f` to `%.2f` — variable width breaks cursor alignment. `go_to_pattern()` sets `update_line1 = true` so the pattern number refreshes on every pattern switch.

**LCD line 2 format** (case 255): `s%d clk:%s Ch%02d ` (exactly 16 chars). Example: `s0 clk:int Ch02 `. MIDI channel is on line 2; `midi_channel_events()` sets `update_line2 = true`.

**LCD cursor positions** are defined as named constants in config.h — use these instead of hardcoded numbers:
- `LCD_L1_X_PATTERN = 3` — first digit of pattern in "P%02u" (after icon+space+'P')
- `LCD_L1_X_TEMPO_10 = 8` — hundreds/tens digit of tempo (shifted right 1 vs old format due to 2-digit pattern)
- `LCD_L1_X_TEMPO_1 = 9` — units digit
- `LCD_L1_X_TEMPO_01 = 11` — tenths digit (after decimal at col 10)
- `LCD_L1_X_TEMPO_001 = 12` — hundredths digit
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
- **Copy pattern (simple mode)**: Hold a pattern select button for 2s, then press destination pattern button
- **Copy pattern (advanced mode)**: Double-click pattern button 0 (within 400 ms) → phase 1: tap step = source pattern → phase 2: tap step = destination pattern
- **Cancel copy (advanced mode)**: D-pad left while copy is armed
- **Chain 4 patterns (simple mode)**: Press pattern buttons 0 + 3 simultaneously to toggle
- **Select pattern 0–15 (advanced mode)**: Hold pattern button 0, tap a step button

**`go_to_pattern(pattern, silent)`**: Turns all 4 pattern LEDs off, except LED 0 stays on when `adv_pat_nav_active` is true. In simple mode, lights the LED for `pattern % 4`. In advanced mode outside nav mode, no LED is lit — buttons are function keys. `toggle()` was previously used but is state-dependent; always use `on()`. The `silent` parameter is accepted but currently unused.

**Pattern LEDs in advanced mode**: While `adv_pat_nav_active` is true, LED 0 stays lit (managed by `run_pattern_select_routine()`). Outside nav mode, no LEDs are lit — buttons are function keys. Do not call `pattern_select_leds[x].on()` from `go_to_pattern()` in advanced mode.

**Advanced mode pattern-nav mode**: Single-click of pattern button 0 (400 ms timeout with no second press) toggles `adv_pat_nav_active`. While active, step buttons select/chain patterns instead of editing steps. The nav loop in `run_pattern_select_routine()` blinking the current pattern's LED at 200 ms period; all chain patterns are solid. Tap one step = single pattern select (clears chain). Hold one step (`adv_chain_hold_step`) and tap a second = define `chain_start..chain_end` range and set `extended_step_length_mode = 1`. Wrap-around chains (start > end) are fully supported. On exit from nav mode, `read_step_memory()` restores the step LEDs.

**Advanced mode 2-phase copy**: Double-click pattern button 0 (two `pattern_select_button_flags[0]` within 400 ms) enters copy mode. Phase 1 (`adv_copy_waiting_source`): LCD shows `copy which pat?` (flag 103); tap a step button to navigate to that pattern and arm phase 2. Phase 2 (`adv_copy_armed`): LCD shows `Copy N->where?` (flag 102); tap a step button to copy to that pattern; LCD shows `Copied X to Y` (flag 101) for 500 ms then returns to main display. D-pad left in `listen_for_navigation_events()` cancels both phases at any point.

**Chain toggle one-shot guard**: The `isPressed()` check for pattern buttons 0+3 fires every loop iteration while both are held. A `static bool chain_toggle_handled` in `run_pattern_select_routine()` ensures the mode flip and `go_to_pattern()` call happen only once per press. It resets when the buttons are released. Do not remove this guard — without it the mode flips back and forth on every loop frame.

**`clear_pattern_memory()` clears all 16 patterns**: The step 0+11 combo calls `clear_pattern_memory()`, which loops over all 16 patterns (`p = 0..15`) and zeros every step. After clearing, it calls `read_step_memory(0, pattern_value)` to refresh the LEDs for the active pattern.

**Pattern chain auto-advance**: When `extended_step_length_mode == 1`, `stepsend()` calls `run_auto_pattern_select_routine()` at `current_step == 15`. This advances `current_pattern` within `chain_start..chain_end`. **Wrap-around chains** (where `chain_start > chain_end`, e.g. start=7, end=2 → plays 7,8,…,15,0,1,2) are fully supported. The advance happens at the start of step 15 so step 15 plays from the current pattern; the new pattern takes over from step 0.

## Simple vs Advanced Mode

**Simple mode** (`advanced_mode == false`, default):
- Pattern buttons 0–3 select patterns 0–3 directly
- Pattern buttons 0+3 simultaneously toggle chain mode (4 patterns)
- Hold any pattern button 2s → pattern copy (press destination pattern button)
- Pattern LEDs show the active pattern (0–3)
- D-pad mode 1 navigates patterns 1–4

**Advanced mode** (`advanced_mode == true`):
- Pattern buttons are function keys — do NOT select patterns directly
- Pattern button 0 **single-click** (400 ms timeout) → toggle `adv_pat_nav_active` (pattern-nav mode)
  - In nav mode, step buttons select/chain patterns; LED 0 stays lit; step LEDs show chain state with blink
  - Tap one step → single pattern select (clears chain)
  - Hold one step, tap another → define `chain_start..chain_end` (wrap-around supported)
  - Single-click button 0 again → exit nav mode, restore step LEDs
- Pattern button 0 **double-click** (two taps ≤400 ms) → 2-phase pattern copy
  - Phase 1: LCD `copy which pat?` → tap step = source; Phase 2: LCD `Copy N->where?` → tap step = destination
  - D-pad left cancels either phase
- Pattern LEDs: LED 0 lit while `adv_pat_nav_active`; otherwise all off
- D-pad mode 1 navigates patterns 1–16
- Hold-for-2s pattern copy is disabled in advanced mode

## Config Menu

Entered by **double-tapping Enter** (two presses within 400 ms). The menu is modal — all d-pad and enter flags are consumed by `run_config_menu()` and `listen_for_navigation_events()` is suppressed while active. The sequencer continues playing normally in the background.

**Navigation**: d-pad up/down scrolls. Line 1 shows `> {current item}`, line 2 shows the next item (no cursor). D-pad left exits from anywhere (also cancels a pending confirmation). Enter selects.

**Menu items (in order)**:
1. **Exit** — Enter, left, or right all exit
2. **Save** — saves to SD (primary) + EEPROM (backup); blocked while playing (shows `Stop first!` on line 2); exits menu and shows `saved!` on success
3. **Clear pattern** — confirmation required (line 2: `Entr=ok  Lft=no`)
4. **Clear all pats** — confirmation required
5. **Reset sliders** — confirmation required
6. **Mode: Simple/Advanced** — toggles immediately, value shown inline on line 1
7. **Octave shift** — enter editing sub-state; up/down adjust ±1 octave (range -5 to +5); Enter or Left exits editing; label shows `Octave shift *` when non-zero
8. **Note shift** — enter editing sub-state; up/down adjust ±1 semitone (range -12 to +12); Enter or Left exits editing; label shows `Note shift   *` when non-zero
9. **Note range** — two-phase editor: Enter starts editing low value (`Edit Lo: N`), Enter again switches to high value (`Edit Hi: N`), Enter again exits; Left exits either phase; low range 0–(high-1), high range (low+1)–127; defaults 36/52; label shows `Note range   *` when non-default
10. **Note scales** — placeholder, shows `Coming soon...` (not yet implemented)

**Double-tap detection**: implemented in the main `loop()` with a static `last_enter_ms` timestamp. Two `uniquePress()` events within 400 ms trigger `enter_config_menu()` instead of setting `enterbutton_flag`. Single tap still behaves as before (LCD refresh, enter LED toggle).

**Save timing**: `EEPROM.commit()` can stall the CPU 10–50 ms during flash programming. Saving while playing would cause a timing hiccup. The Save item checks `playstatus` and refuses with `Stop first!` if the sequencer is running.

## SD Card Storage

**Primary storage**: `/synthseqr/autosave.json` on the onboard SD card slot (uses `SDCARD_SS_PIN`, not GPIO 10). The folder is created automatically on first init.

**Boot sequence**: `boot_load()` in `sd_storage.ino` calls `sd_init()` then `load_from_sd()`. On failure (no card, no file), falls back to `load_from_eeprom()`. Called from `setup()` instead of the old direct `load_from_eeprom()`.

**Save**: `save_everywhere()` writes to SD first, then EEPROM. Called from the config menu Save item.

**JSON format**: hand-rolled minimal parser — no ArduinoJson dependency. Scans for known keys by name, ignores unknown keys (forward-compatible with extra fields added by external tools). All 16 patterns are saved. Users can hand-edit or generate JSON externally and load it on the device.

```json
{
  "version": 1,
  "tempo": 120.00,
  "swing": 0,
  "midi_channel": 2,
  "octave_shift": 0,
  "note_shift": 0,
  "note_range_low": 36,
  "note_range_high": 52,
  "chain_active": 0,
  "chain_start": 0,
  "chain_end": 3,
  "advanced_mode": 0,
  "patterns": [
    {"steps":[1,0,...16 values],"pitches":[36,37,...16 values]},
    ...16 patterns
  ]
}
```

**Arduino prototype issue**: The Arduino build tool auto-generates function prototypes before `#include`s are processed. Functions with `File&` parameters fail with "File not declared in this scope". All SD helper functions use a module-level `static File _f` handle instead — no `File` type appears in any function signature. Do not add `File&` parameters to helpers in `sd_storage.ino`.

## EEPROM Save / Load

EEPROM is a **silent fallback** — used only when SD is unavailable on boot. Save still writes to both (`save_everywhere()`).

**On boot**: `load_from_eeprom()` is called only if `load_from_sd()` returns false. Checks for a magic sentinel byte at address 0. If missing (first boot or layout change), globals keep compiled-in defaults.

**EEPROM layout** (527 bytes, defined as `#define` constants in `storage.ino`):

| Address | Size | Content |
|---------|------|---------|
| 0 | 1 | Magic byte `0xC3` |
| 1 | 1 | `MIDICHANNEL` |
| 2 | 1 | `SWING` |
| 3 | 4 | `TEMPO` (float) |
| 7 | 1 | `current_pattern` |
| 8 | 1 | `extended_step_length_mode` |
| 9 | 1 | `external_clock_mode` |
| 10 | 256 | `step_data[16][16]` (1 byte per step) |
| 266 | 256 | `pattern_step_pitches[16][16]` |
| 522 | 1 | `octave_shift` (int8_t as raw byte) |
| 523 | 1 | `advanced_mode` (bool) |
| 524 | 1 | `note_shift` (int8_t as raw byte) |
| 525 | 1 | `slider_map_low_value` (uint8_t) |
| 526 | 1 | `slider_map_high_value` (uint8_t) |

**Validation**: All loaded values are range-checked so corrupted flash can't break the sequencer.

**`EEPROM.commit()` is required**: `storage.ino` uses `FlashAsEEPROM_SAMD`. All writes buffer in RAM until `commit()` burns to flash. Without it, saves vanish on power-off.

**Magic byte**: Increment `EEPROM_MAGIC_VALUE` in `storage.ino` whenever the layout changes. Current value: `0xC3`.

**LCD confirmation**: `lcdflag = 202` shows `saved!` for 2 seconds using a `static unsigned long msg_until` timer inside the LCD case, then returns to the main display.
