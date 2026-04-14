# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Synthseqr is Arduino firmware for a hardware MIDI step sequencer running on the **Adafruit Grand Central M4 Express** (ATSAMD51J19A ARM Cortex-M4). It's a 16-step, 16-pattern sequencer with physical step buttons, voice sliders, D-pad navigation, and an LCD display. Version 2.5 is experimental/in-progress.

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
| `navigation.ino` | D-pad + enter button: adjusts tempo; enter cycles slider mode (NN/VL/GT) |
| `sequencer_timer.ino` | TC4 hardware timer driver: setup, period update, stop/start, ISR |
| `step_button_routine.ino` | Step button presses, LED toggling, pattern clear |
| `voice_slider_routine.ino` | Reads 16 analog sliders in NN/VL/GT mode → note, velocity, or gate |
| `pattern_select_routine.ino` | Pattern switching, copy, and 4-pattern chain mode |
| `LCD.ino` | LCD initialization and display updates |
| `midi_note_sending.ino` | `step()` blink callback and `midi()` MIDI clock output callback |
| `diagnostics.ino` | Hardware test mode (hold D-pad left + right 1s to enter/exit) |
| `storage.ino` | EEPROM save/load: `save_to_eeprom()`, `load_from_eeprom()` |
| `sd_storage.ino` | SD card save/load: `save_to_sd()`, `load_from_sd()`, `boot_load()`, `save_everywhere()` |
| `config_menu.ino` | Modal config menu: save, clear, mode toggle, octave/note shift, note range, swing, clock, channel |

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
uint8_t pattern_step_velocities[16][16] // saved velocity per pattern per step (default 127)
uint8_t step_gate[16][16]             // gate length per pattern per step (1–8 steps, default 1)
uint8_t voice_slider_midinotenum[16]  // MIDI note per slider (default 36–51)
uint8_t voice_slider_midivelocity[16] // MIDI velocity per slider (default 127)
int8_t sounding_notes[16]            // pitch currently sounding per step (-1 = silent)
int8_t sounding_note_end_step[16]    // which step triggers note-off for each step (-1 = silent)
int8_t last_triggered_step           // most-recently-fired step for LCD line 2 feedback (-1 = none)
uint8_t current_pattern               // Active pattern 0–15
bool playstatus                       // Is sequencer playing?
float TEMPO                           // BPM (10–250)
uint8_t SWING                         // 0–5
uint8_t cc_step_values[16][16]        // CC value per pattern per step (0–127); default 0
uint8_t cc_step_enabled[16][16]       // CC step on/off per pattern per step; default 0 (off)
uint8_t cc_number[16]                 // CC controller number per pattern (1–119, skipping 32 and 96–101); default 1 (Mod Wheel)
uint8_t slider_mode                   // 1=NN (note number), 2=VL (velocity), 3=GT (gate), 4=CC (control change)
uint8_t slider_mode_total             // 4 (NN, VL, GT, CC)
uint8_t lcdflag                       // LCD display mode selector
bool external_clock_mode              // false = internal TC4, true = follow USB-MIDI clock
bool advanced_mode                    // false = Simple (4 patterns, buttons select), true = Advanced (16 patterns, buttons are function keys)
bool adv_pat_nav_active               // true while pattern-nav mode is active; step buttons select/chain patterns instead of editing steps
bool adv_copy_waiting_source          // true while copy phase 1: waiting for step tap to pick the source pattern
bool adv_copy_armed                   // true while copy phase 2: source = current_pattern; waiting for step tap = destination
int8_t adv_chain_hold_step            // step button held in nav mode for chain definition (-1 = none)
unsigned long adv_blink_last_ms       // last blink toggle timestamp in nav mode
bool adv_blink_state                  // current blink state for the playing pattern's LED in nav mode
int8_t octave_shift                   // semitone offset applied at MIDI send time; range -5 to +5 octaves
int8_t note_shift                     // additional semitone offset applied at MIDI send time; range -12 to +12
uint8_t scale_type                    // 0=Chromatic 1=Major 2=NatMinor 3=PentMaj 4=PentMin 5=Dorian 6=Mixolydian 7=HarmMinor 8=Blues; default 0
uint8_t scale_root                    // 0=C … 11=B; default 0 (C)
uint8_t scale_note_pool[128]          // in-scale MIDI notes within the note range; rebuilt by build_scale_notes()
uint8_t scale_note_count              // number of valid entries in scale_note_pool[]
// Pattern playback settings (global — apply to all patterns):
uint8_t pattern_length                // 1–16 steps before looping; default 16; FifteenStep step count kept in sync via seq.setSteps()
uint8_t pattern_direction             // 0=Fwd 1=Rev 2=Pong 3=Rand 4=Shuf 5=E/O 6=In 7=Quad
bool ping_pong_going_forward          // ping-pong advance direction; flips at each endpoint
uint8_t ping_pong_step                // ping-pong virtual play position (0..pattern_length-1)
uint8_t shuffle_order[16]             // current Fisher-Yates permutation; regenerated by init_shuffle() at cycle end
uint8_t shuffle_pos                   // next index into shuffle_order (0..pattern_length-1)
// External clock swing state:
uint8_t ext_clk_pulse_count           // 0xF8 pulses accumulated since last step (0–5)
unsigned long ext_clk_last_pulse_us   // timestamp of previous 0xF8 pulse
unsigned long ext_clk_avg_interval_us // IIR-averaged 0xF8 pulse interval (~20833 µs at 120 BPM)
bool ext_swing_pulse_pending          // true when a deferred external-clock step pulse is waiting
unsigned long ext_swing_pulse_fire_us  // micros() value to fire it at
bool          ext_clock_start_pending  // play pressed while ext clock running; waiting for next beat boundary to call seq.start()
```

## Sequencer Flow

1. `seq.run()` (FifteenStep) ticks the sequencer on each `loop()` call
2. Timing is driven by the **TC4 hardware timer** (internal mode) or **incoming USB-MIDI 0xF8** (external mode) — both call `seq.hardwareClockPulse()` which sets volatile flags; `seq.run()` processes those flags in main-loop context
3. On each step change, `stepsend(current_step, last_step)` fires as the step callback
4. `stepsend()` first computes `play_step` from `current_step` via the `pattern_direction` switch (Fwd=identity; Rev=mirror; Pong=ping_pong_step counter; Rand=random(); Shuf=shuffle_order[shuffle_pos]; E/O=even-then-odd interleave; In=outside-in; Quad=Q1,Q3,Q2,Q4 reordering). Then it scans all 16 `sounding_note_end_step[]` entries — any slot whose end-step equals `current_step` gets a note-off (gate timing is always in hardware clock steps). If `step_data[pattern][0][play_step]` is on, note-on is sent using `pattern_step_pitches[pattern][play_step]` and `voice_slider_midivelocity[play_step]`; `sounding_note_end_step[play_step]` is set to `(current_step + step_gate[pattern][play_step]) % pattern_length`. `last_triggered_step` is updated to `play_step` and `update_line2` is set for the LCD. If `cc_step_enabled[pattern][play_step]` is set, a CC message is also sent using `cc_number[pattern]` and `cc_step_values[pattern][play_step]`; CC steps fire independently of note steps.
5. On stop (play button or MIDI stop), `allNotesOff()` sends note-off for every entry in `sounding_notes[]` and clears both `sounding_notes[]` and `sounding_note_end_step[]`

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

D-pad left/right cycles through 4 `timing_mode` values controlling what up/down adjusts, in visual left-to-right order matching the LCD layout:

| Mode | Up/Down adjusts | LCD location |
|------|----------------|--------------|
| 1 | Pattern (1–4 simple / 1–16 advanced, wraps) | Line 1, column 1 |
| 2 | Tempo ±10 BPM | Line 1, column 9 |
| 3 | Tempo ±1 BPM | Line 1, column 10 |
| 4 | Tempo ±0.1 BPM | Line 1, column 12 |

Default `timing_mode = 2` (±10 BPM). Timing mode 5 (±0.01 BPM) was removed. Swing, clock source, and MIDI channel have moved to the config menu (double-tap Enter).

**Enter button** (single tap, simple mode only) cycles the slider mode: NN → VL → GT → CC → NN via `set_slider_mode()`. In advanced mode the enter button has no slider-mode function (pattern buttons 1/2/3 handle it). The enter LED is no longer toggled by the enter button.

**LCD line 1 format** (case 255): `P{pat:02u} >{step:02d} {tempo:05.1f} [?5][mode]` — 16 chars total. No play/stop icon. Pattern is 2 digits `P01`–`P16` (cols 0–2); step counter `>{01–16}` at cols 4–6, or `>--` when stopped/no step fired; tempo is `%05.1f` (zero-padded, 1 decimal) at cols 8–12; cols 14–15 show slider mode indicator: custom char `?5` + mode char (`?4`=NN, `?2`=VL, `G`=GT, `?3`=CC). Example: `P01 >03 120.0 ♪N`. `go_to_pattern()` sets `update_line1 = true` so the pattern number refreshes on every pattern switch.

**LCD line 2 format** (case 255): Real-time step-trigger feedback. Format: `[note_icon]PPP [vel_icon]VVV G{gate}[cc_icon]{ccc}` (16 chars). Fields: note icon (`?4`) + 3-digit pitch + space (5 chars), velocity icon (`?2`) + 3-digit velocity + space (5 chars), `G` + gate digit (2 chars), CC icon (`?3`) + 3-digit CC value or `---` if cc_step_enabled is 0 (4 chars). Example: `♪045 ♩127 G4♩064`. Updates on every step trigger (`last_triggered_step` changes) or slider mode cycle. When no step has fired yet (`last_triggered_step == -1`), line 2 shows blanks or a default state.

**LCD cursor positions** are defined as named constants in config.h — use these instead of hardcoded numbers:
- `LCD_L1_X_PATTERN = 1` — first digit of pattern in "P%02u"
- `LCD_L1_X_STEP = 5` — first digit of step counter in ">%02d"
- `LCD_L1_X_TEMPO_10 = 9` — hundreds/tens digit of tempo
- `LCD_L1_X_TEMPO_1 = 10` — units digit
- `LCD_L1_X_TEMPO_01 = 12` — tenths digit (after decimal at col 11)
- `LCD_L1_X_SLIDERMODE = 14` — slider mode indicator (cols 14–15)
- `LCD_L1_X_TEMPO_001` was removed (timing mode 5 removed)

Switching clock source calls `setExternalClockMode()` which stops or starts TC4 as needed.

**Cursor restore after redraw**: `run_LCD_update()` case 255 tracks a local `did_redraw` flag. If either `update_line1` or `update_line2` was processed that frame, the cursor is repositioned to `cursor_x`/`cursor_y` afterward — even if `cursor_flag` is false. This ensures that events like play/stop toggling line 1 or pattern switches don't leave the cursor stranded after the redraw. All transient message cases (93, 101, 200, 201, 202) set `update_line1 = true` and `update_line2 = true` before returning to `next_lcdflag = 255` for the same reason.

**`lcdflag` / `next_lcdflag` rule**: `run_LCD_update()` starts every frame with `if (next_lcdflag != lcdflag) lcdflag = next_lcdflag`. Any code that sets `lcdflag` externally (outside `run_LCD_update`) **must also set `next_lcdflag` to the same value** — otherwise `next_lcdflag` (still 255 from the previous frame) immediately overwrites `lcdflag` before the switch runs and the message is never displayed. This applies to every transient message trigger: save (202), chain single/4 (200/201), pattern copy (100/101), slider reset (93).

## Slider Modes

The 16 voice sliders operate in one of four modes:

| Mode | Display | Slider controls | Storage |
|------|---------|----------------|---------|
| 1 — NN | `?5?4` (note icon) | MIDI note number (mapped to `slider_map_low_value`–`slider_map_high_value`) | `pattern_step_pitches[p][s]`, `voice_slider_midinotenum[s]` |
| 2 — VL | `?5?2` (velocity icon) | MIDI velocity (1–127) | `pattern_step_velocities[p][s]`, `voice_slider_midivelocity[s]` |
| 3 — GT | `?5G` | Gate length (1–8 steps) | `step_gate[p][s]` |
| 4 — CC | `?5?3` (CC icon) | CC value (0–127) per step; step buttons toggle `cc_step_enabled` on/off | `cc_step_values[p][s]`, `cc_step_enabled[p][s]` |

**Switching modes:**
- **Simple mode**: Enter single-tap cycles NN → VL → GT → CC → NN
- **Advanced mode**: Pattern button 1 = NN, button 2 = GT, button 3 = VL. CC mode not directly mapped to a button in advanced mode; use Enter to reach it.

**`set_slider_mode(mode)`**: Central entry point for all slider mode changes. Sets `slider_mode`, arms `slider_needs_pickup[i] = true` for all 16 sliders, sets `update_line1 = update_line2 = true`, and switches step LED display: entering CC mode calls `read_cc_step_memory()` (LEDs show `cc_step_enabled`); leaving CC mode calls `read_step_memory()` (LEDs show `step_data`). Always call this function — never set `slider_mode` directly — so pickup guards and LED display are always updated on transition.

**Pickup guard (all modes)**: `slider_needs_pickup[s]` prevents a slider from overwriting stored data until the physical slider crosses through the stored value for the current mode. This prevents mode switches from silently corrupting pattern data when sliders are at different physical positions for each mode. Tolerance: ±1 note (NN), ±1 velocity unit (VL), exact match (GT, range 1–8), ±1 CC unit (CC).

**CC mode step buttons**: In CC mode, step buttons toggle `cc_step_enabled[p][s]` on/off (independent of `step_data`). A step can have CC enabled without having a note, and vice versa. The step LED reflects `cc_step_enabled` state while in CC mode.

**Always boots to NN mode** (`slider_mode = 1`). Mode is not saved to SD/EEPROM — it resets to NN on power-up.

**`resetSliders()`**: Resets all 16 steps' pitches to `slider_map_low_value`, velocities to 127, and gates to 1, for the current pattern.

## Multi-Step Gate Lengths

Each step has a gate length (1–8) stored in `step_gate[pattern][step]`. A gate of 1 means the note-off fires at the next step (classic behavior). A gate of N means the note rings for N steps.

**Implementation**: `stepsend()` sets `sounding_note_end_step[current_step] = (current_step + gate) % 16` when a note fires. On every step advance, `stepsend()` scans all 16 `sounding_note_end_step[]` entries and sends note-off for any that match `current_step`. This allows multiple overlapping notes to ring simultaneously and expire independently.

**Note**: gate lengths wrap around step 16→0, so a gate of 8 on step 15 will hold until step 7 of the next loop (or the next pattern if chained).

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
- The play button works as a local arm/start. **Beat-sync start**: if the external clock is already running when play is pressed, `ext_clk_pulse_count` is reset to 0 and `ext_clock_start_pending` is set instead of calling `seq.start()` immediately. `read_midi()` calls `seq.start()` on the very next step-boundary pulse (6th 0xF8), so the sequencer starts in phase with the incoming clock. If a MIDI Stop arrives while the start is pending, the flag is cleared.
- **Swing is supported**: 0xF8 pulses are counted per step (0–5; 6th pulse = step advance). An IIR-averaged interval (`ext_clk_avg_interval_us`, 7/8 old + 1/8 new) estimates pulse spacing. When transitioning to an odd step with SWING > 0, the step pulse is deferred by `avg_interval × SWING` µs using `ext_swing_pulse_pending` / `ext_swing_pulse_fire_us`. The main loop checks `(long)(micros() - ext_swing_pulse_fire_us) >= 0` and fires the deferred `hardwareClockPulse()` before `seq.run()`. Only the "late odd step" side is implemented (no compensating early even step), making external swing slightly milder than internal at the same SWING value.

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

**Pattern copy includes velocities, gates, and CC data**: Both `listen_for_copy_command()` (simple mode) and the advanced mode 2-phase copy loop copy `pattern_step_velocities`, `step_gate`, `cc_step_enabled`, `cc_step_values`, and `cc_number` along with `step_data` and `pattern_step_pitches`.

**`go_to_pattern(pattern, silent)`**: Turns all 4 pattern LEDs off, except LED 0 stays on when `adv_pat_nav_active` is true. In simple mode, lights the LED for `pattern % 4`. In advanced mode outside nav mode, no LED is lit — buttons are function keys. `toggle()` was previously used but is state-dependent; always use `on()`. The `silent` parameter is accepted but currently unused.

**Pattern LEDs in advanced mode**: While `adv_pat_nav_active` is true, LED 0 stays lit (managed by `run_pattern_select_routine()`). Outside nav mode, no LEDs are lit — buttons are function keys. Do not call `pattern_select_leds[x].on()` from `go_to_pattern()` in advanced mode.

**Advanced mode pattern-nav mode**: Single-click of pattern button 0 (400 ms timeout with no second press) toggles `adv_pat_nav_active`. While active, step buttons select/chain patterns instead of editing steps. The nav loop in `run_pattern_select_routine()` blinking the current pattern's LED at 200 ms period; all chain patterns are solid. Tap one step = single pattern select (clears chain). Hold one step (`adv_chain_hold_step`) and tap a second = define `chain_start..chain_end` range and set `extended_step_length_mode = 1`. Wrap-around chains (start > end) are fully supported. On exit from nav mode, `read_step_memory()` restores the step LEDs.

**Advanced mode 2-phase copy**: Double-click pattern button 0 (two `pattern_select_button_flags[0]` within 400 ms) enters copy mode. Phase 1 (`adv_copy_waiting_source`): LCD shows `copy which pat?` (flag 103); tap a step button to navigate to that pattern and arm phase 2. Phase 2 (`adv_copy_armed`): LCD shows `Copy N->where?` (flag 102); tap a step button to copy to that pattern; LCD shows `Copied X to Y` (flag 101) for 500 ms then returns to main display. D-pad left in `listen_for_navigation_events()` cancels both phases at any point.

**Chain toggle one-shot guard**: The `isPressed()` check for pattern buttons 0+3 fires every loop iteration while both are held. A `static bool chain_toggle_handled` in `run_pattern_select_routine()` ensures the mode flip and `go_to_pattern()` call happen only once per press. It resets when the buttons are released. Do not remove this guard — without it the mode flips back and forth on every loop frame.

**`clear_pattern_memory()` clears all 16 patterns**: The step 0+11 combo calls `clear_pattern_memory()`, which loops over all 16 patterns (`p = 0..15`) and zeros every step, resets velocities to 127, and resets gates to 1. After clearing, it calls `read_step_memory(0, pattern_value)` to refresh the LEDs for the active pattern.

**Pattern chain auto-advance**: When `extended_step_length_mode == 1`, `stepsend()` calls `run_auto_pattern_select_routine()` at `current_step == pattern_length - 1`. This advances `current_pattern` within `chain_start..chain_end`. **Wrap-around chains** (where `chain_start > chain_end`, e.g. start=7, end=2 → plays 7,8,…,15,0,1,2) are fully supported. The advance happens at the start of step 15 so step 15 plays from the current pattern; the new pattern takes over from step 0.

## Simple vs Advanced Mode

**Simple mode** (`advanced_mode == false`, default):
- Pattern buttons 0–3 select patterns 0–3 directly
- Pattern buttons 0+3 simultaneously toggle chain mode (4 patterns)
- Hold any pattern button 2s → pattern copy (press destination pattern button)
- Pattern LEDs show the active pattern (0–3)
- D-pad mode 1 navigates patterns 1–4
- Enter single-tap cycles slider mode: NN → VL → GT → NN

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
- **Pattern button 1** → slider mode NN (note number); LED 1 stays lit
- **Pattern button 2** → slider mode GT (gate); LED 2 stays lit
- **Pattern button 3** → slider mode VL (velocity); LED 3 stays lit
- Pattern LEDs: LED 0 = nav mode active; LEDs 1/2/3 = active slider mode indicator
- D-pad mode 1 navigates patterns 1–16
- Hold-for-2s pattern copy is disabled in advanced mode
- Enter single-tap has no function in advanced mode (no LED toggle, no mode cycle)

## Config Menu

Entered by **double-tapping Enter** (two presses within 400 ms). The menu is modal — all d-pad and enter flags are consumed by `run_config_menu()` and `listen_for_navigation_events()` is suppressed while active. The sequencer continues playing normally in the background.

**Navigation**: d-pad up/down scrolls. Line 1 shows `> {current item}`, line 2 shows the next item (no cursor). D-pad left exits from anywhere (also cancels a pending confirmation). Enter selects.

**Menu items (in order)**:
1. **Exit** — Enter, left, or right all exit
2. **Save** — saves to SD (primary) + EEPROM (backup); blocked while playing (shows `Stop first!` on line 2); exits menu and shows `saved!` on success
3. **Clear pattern** — confirmation required (line 2: `Entr=ok  Lft=no`)
4. **Clear all pats** — confirmation required
5. **Reset sliders** — confirmation required
6. **Clock: int/ext** — toggles immediately via `setExternalClockMode(!external_clock_mode)`; value shown inline on line 1
7. **Channel** — enter editing sub-state; up/down adjust 1–16; Enter or Left exits editing
8. **Swing** — enter editing sub-state; up/down adjust 0–5; Enter or Left exits editing
9. **Mode: Simple/Advanced** — toggles immediately, value shown inline on line 1
10. **Octave shift** — enter editing sub-state; up/down adjust ±1 octave (range -5 to +5); Enter or Left exits editing; label shows `Octave shift *` when non-zero
11. **Note shift** — enter editing sub-state; up/down adjust ±1 semitone (range -12 to +12); Enter or Left exits editing; label shows `Note shift   *` when non-zero
12. **Note range** — two-phase editor: Enter starts editing low value (`Edit Lo: N`), Enter again switches to high value (`Edit Hi: N`), Enter again exits; Left exits either phase; low range 0–(high-1), high range (low+1)–127; defaults 36/52; label shows `Note range   *` when non-default
13. **Note scales** — two-phase editor: Enter starts editing scale type (`Sc: Major`), Enter again switches to root note (`Root: C#`), Enter exits; Left exits either phase; changing scale type or root immediately calls `apply_scale_to_all_patterns()` which retroactively quantizes all stored pitches across all 16 patterns to the nearest in-scale note and re-arms all pickup guards; scale pool is also rebuilt on note range changes; label shows `Note scales  *` when scale is non-default (not Chromatic/C). Scales: 0=Chromatic 1=Major 2=NatMinor 3=PentMaj 4=PentMin 5=Dorian 6=Mixolydian 7=HarmMinor 8=Blues. Scale tables and helpers live in `scales.ino`; `SCALE_COUNT` and `extern` declarations for `SCALE_NAMES`/`ROOT_NAMES` are in `config.h` so `config_menu.ino` (compiled before `scales.ino` alphabetically) can see them.
14. **Pat length** — enter editing sub-state; up/down adjust 1–16; tap any step button to set length to N+1 (step buttons are consumed before `detect_step_button_presses()` runs); step LEDs 0..length-1 light up while editing, restored via `read_step_memory()` on exit; `seq.setSteps(pattern_length)` called on every change; `init_shuffle()` called if direction is Shuf; label shows `Pat length   *` when not 16
15. **Pat dir** — enter editing sub-state; up/down cycles 0–7 (`PATTERN_DIRECTION_COUNT`); entering Pong resets `ping_pong_step=0`/`ping_pong_going_forward=true`; entering Shuf calls `init_shuffle()`; direction names: `Fwd`/`Rev`/`Pong`/`Rand`/`Shuf`/`E/O`/`In`/`Quad`
16. **CC number** — enter editing sub-state; up/down cycles through valid CC numbers (1–119, skipping 32 and 96–101) via `next_valid_cc(current, dir)`; Enter or Left exits editing; label shows `CC:{number} {name}` (7-char LCD name for well-known CCs); line 2 shows `CC: {number} {name}` while editing. One CC number per pattern, shared across all 16 steps of that pattern. Safe CC range: 1–119, forbidden: 32 (Bank LSB), 96–101 (RPN/NRPN data), 120–127 (Channel Mode Messages, not in menu range)

**Double-tap detection**: implemented in the main `loop()` with `last_enter_ms` and `enter_tap_pending` statics. The first tap starts a 400 ms window without immediately setting `enterbutton_flag` — this prevents the first press of a double-tap from accidentally triggering a slider mode change. If a second `uniquePress()` arrives within the window, `enter_config_menu()` fires. If the window expires without a second tap, `enterbutton_flag` is set as a normal single-tap. Single-tap actions are therefore delayed by up to 400 ms, which is imperceptible for mode-cycling use.

**Save timing**: `EEPROM.commit()` can stall the CPU 10–50 ms during flash programming. Saving while playing would cause a timing hiccup. The Save item checks `playstatus` and refuses with `Stop first!` if the sequencer is running.

## SD Card Storage

**Primary storage**: `/synthseqr/autosave.json` on the onboard SD card slot (uses `SDCARD_SS_PIN`, not GPIO 10). The folder is created automatically on first init.

**Boot sequence**: `boot_load()` in `sd_storage.ino` calls `sd_init()` then `load_from_sd()`. On failure (no card, no file), falls back to `load_from_eeprom()`. Called from `setup()` instead of the old direct `load_from_eeprom()`.

**Save**: `save_everywhere()` writes to SD first, then EEPROM. Called from the config menu Save item.

**JSON format**: hand-rolled minimal parser — no ArduinoJson dependency. Scans for known keys by name, ignores unknown keys (forward-compatible with extra fields added by external tools). All 16 patterns are saved including velocities and gates. Users can hand-edit or generate JSON externally and load it on the device.

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
  "scale_root": 0,
  "scale_type": 0,
  "chain_active": 0,
  "chain_start": 0,
  "chain_end": 3,
  "advanced_mode": 0,
  "pattern_length": 16,
  "pattern_direction": 0,
  "patterns": [
    {
      "steps":[1,0,...16 values],
      "pitches":[36,37,...16 values],
      "velocities":[127,127,...16 values],
      "gates":[1,1,...16 values],
      "cc_number": 1,
      "cc_enabled":[0,0,...16 values],
      "cc_values":[0,0,...16 values]
    },
    ...16 patterns
  ]
}
```

**Arduino prototype issue**: The Arduino build tool auto-generates function prototypes before `#include`s are processed. Functions with `File&` parameters fail with "File not declared in this scope". All SD helper functions use a module-level `static File _f` handle instead — no `File` type appears in any function signature. Do not add `File&` parameters to helpers in `sd_storage.ino`.

## EEPROM Save / Load

EEPROM is a **silent fallback** — used only when SD is unavailable on boot. Save still writes to both (`save_everywhere()`). Velocities and gates are **not stored in EEPROM** (SD only) — on EEPROM-only boot they default to 127 and 1 respectively. CC data (cc_number, cc_step_enabled, cc_step_values) **is** stored in EEPROM.

**On boot**: `load_from_eeprom()` is called only if `load_from_sd()` returns false. Checks for a magic sentinel byte at address 0. If missing (first boot or layout change), globals keep compiled-in defaults.

**EEPROM layout** (1059 bytes, defined as `#define` constants in `storage.ino`):

| Address | Size | Content |
|---------|------|---------|
| 0 | 1 | Magic byte `0xC6` |
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
| 527 | 1 | `pattern_length` (uint8_t, 1–16) |
| 528 | 1 | `pattern_direction` (uint8_t, 0–7) |
| 529 | 1 | `scale_root` (uint8_t, 0–11) |
| 530 | 1 | `scale_type` (uint8_t, 0–8) |
| 531 | 16 | `cc_number[16]` (one byte per pattern) |
| 547 | 256 | `cc_step_enabled[16][16]` (one byte per step) |
| 803 | 256 | `cc_step_values[16][16]` (one byte per step) |

**Validation**: All loaded values are range-checked so corrupted flash can't break the sequencer. CC numbers validated to be in safe range (1–119, not 32, not 96–101).

**`EEPROM.commit()` is required**: `storage.ino` uses `FlashAsEEPROM_SAMD`. All writes buffer in RAM until `commit()` burns to flash. Without it, saves vanish on power-off.

**Magic byte**: Increment `EEPROM_MAGIC_VALUE` in `storage.ino` whenever the layout changes. Current value: `0xC6`.

**LCD confirmation**: `lcdflag = 202` shows `saved!` for 2 seconds using a `static unsigned long msg_until` timer inside the LCD case, then returns to the main display.
