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

| File                                       | Role                                                                                                                                                                                                              |
| ------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `synthseqr_with_grandcentralM4express.ino` | `setup()` and `loop()` — polls all subsystems in order                                                                                                                                                            |
| `config.h`                                 | All global state: pin definitions, arrays, library includes                                                                                                                                                       |
| `transport.ino`                            | Play/stop button, MIDI clock output, `stepsend()` callback, `allNotesOff()`                                                                                                                                       |
| `midi_processor.ino`                       | MIDI input: clock sync (0xF8), start (0xFA), stop (0xFC)                                                                                                                                                          |
| `navigation.ino`                           | D-pad + enter button: adjusts tempo; enter cycles slider mode (NN/VL/GT)                                                                                                                                          |
| `sequencer_timer.ino`                      | TC4 hardware timer driver: setup, period update, stop/start, ISR                                                                                                                                                  |
| `step_button_routine.ino`                  | Step button presses, LED toggling, hold+tap gate-set gesture                                                                                                                                                      |
| `voice_slider_routine.ino`                 | Reads 16 analog sliders in NN/VL/GT/CC/PR mode → note, velocity, gate, CC, or probability                                                                                                                         |
| `pattern_select_routine.ino`               | Pattern switching, copy, and 4-pattern chain mode                                                                                                                                                                 |
| `LCD.ino`                                  | LCD initialization and display updates                                                                                                                                                                            |
| `midi_note_sending.ino`                    | `step()` blink callback and `midi()` MIDI clock output callback                                                                                                                                                   |
| `diagnostics.ino`                          | Hardware test mode: `bool diag_mode` + `uint8_t diag_submode` gate the main loop; submode 0 = input test (`run_diagnostics_display()`), submode 1 = LED chase (`run_diag_led_test()`); entered from Diagnostics submenu in config menu |
| `storage.ino`                              | EEPROM save/load: `save_to_eeprom()`, `load_from_eeprom()`                                                                                                                                                        |
| `sd_storage.ino`                           | SD card save/load: `save_to_sd()`, `load_from_sd()`, `boot_load()`, `save_everywhere()`                                                                                                                           |
| `config_menu.ino`                          | Modal config menu: save, clear, mode toggle, octave/note shift, note range, swing, clock, channel                                                                                                                 |

**HAL classes** (Button, LED, Potentiometer) provide debouncing and event helpers used throughout.

**Potentiometer / SAMD51 ADC resolution**: `Potentiometer.cpp` was originally written for AVR (10-bit ADC, 0–1023). The SAMD51 uses a 12-bit ADC (0–4095). `getSector()` divides `analogRead()` by `4096/sectors` — not `1024/sectors`. Using 1024 causes the sector value to wrap at ~25% of slider travel, producing a peak-then-drop artifact. Do not revert to 1024.

**Button debouncing**: `Button.h`/`Button.cpp` are sketch-local copies with a 50 ms hardware debounce in `isPressed()`. A `_debounce_until` timestamp gates all state transitions — during the window `CHANGED` is suppressed and the cached state is returned. This prevents electrical bounce from firing `uniquePress()` multiple times per physical press. Call `setDebounceDelay(ms)` to override the default per-instance.

**Button pull-up on SAMD51**: `Button::pullup()` uses `pinMode(pin, INPUT_PULLUP)` — NOT the legacy AVR `digitalWrite(pin, HIGH)` trick. The AVR trick does not reliably enable the internal pull-up on SAMD51 (Cortex-M4). Without a valid pull-up the input pin floats, causing random noise reads and unreliable button detection. Do not revert to `digitalWrite(HIGH)` here.

**Step button / LED decoupling**: `detect_step_button_presses()` toggles `step_data[pattern_value][0][i]` directly and then calls `step_leds[i].on()` / `step_leds[i].off()` to match. It does NOT read `step_leds[i].getState()` as the source of truth. The chase light in `run_chase_lights()` inverts the current step's LED, so reading LED state would cancel out a button press on the currently-playing step. Always keep step toggle logic coupled to `step_data`, not LED state.

**`isPressed()` vs `wasPressed()` for combo checks**: Each call to `isPressed()` updates internal state (PREVIOUS, CHANGED, debounce window). Calling it a second time on the same button within the same loop iteration — after `uniquePress()` already ran — can steal a CHANGED flag and cause the next loop's `uniquePress()` to miss a press. For combo checks (chain toggle) that run after `uniquePress()` has already been called for those buttons, always use `wasPressed()` instead. `wasPressed()` reads the cached CURRENT bit with no side effects. The gate-set gesture in `detect_step_button_presses()` uses `wasPressed()` to check the held step's state on each loop iteration for the same reason.

## Core State

```cpp
// In config.h:
int step_data[16][1][16]              // [pattern][voice][step] — on/off for each step
uint8_t pattern_step_pitches[16][16]  // saved pitch per pattern per step
uint8_t pattern_step_velocities[16][16] // saved velocity per pattern per step (default 127)
uint8_t step_gate[16][16]             // gate length per pattern per step (1–16 steps, default 1)
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
uint8_t slider_mode                   // 1=NN, 2=VL, 3=GT, 4=CC (sequenced), 5=PR, 6=LV (live CC), 7=D (per-step drift)
uint8_t slider_mode_total             // 7 (NN, VL, GT, CC, PR, LV, D)
uint8_t live_cc_number[16]            // CC# per LV lane (global across patterns); default 102–117 (MIDI "Undefined" range — avoids Mod/Vol/Pan/Expression)
uint8_t live_cc_channel               // 1–16 MIDI channel for live CC; independent of MIDICHANNEL
uint8_t live_cc_last_sent[16]         // last sent 0–127 per lane; 255 sentinel = "never sent"
int8_t  live_cc_editing_lane          // -1 = idle; 0..15 = lane currently being edited
int8_t  live_cc_last_lane             // most-recent lane that sent (LCD line 2)
uint8_t step_probability[16][16]      // fire probability per step per pattern (0–100); default 100 = always fires
uint8_t pitch_drift                   // semitones of random pitch wander at send time (0=off, 1–7); global
uint8_t step_drift_enabled[16][16]    // per-step drift on/off (D mode); default 0
uint8_t step_drift_amount[16][16]     // per-step drift amount in semitones (0–12); default 0 — additive with pitch_drift
uint8_t slider_hi_trim                // extra notes added to slider_map_high_value at mapping time for physical calibration (0–4); default 0
uint8_t slider_takeover               // 0=Catch (cross stored value), 1=Jump (engage on first movement), 2=Relative (delta from stored); default 0
uint16_t slider_last_raw[16]          // last 12-bit ADC reading per slider; used by Jump-movement detection and Relative deltas
uint8_t slider_pickup_dir[16]         // overlay direction per slider: 0=engaged, 1=push up ('^'), 2=pull down ('v')
bool slider_pickup_overlay_active     // when true, LCD line 2 shows the per-slider catch-direction overlay instead of step feedback
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
uint8_t scale_type                    // 0=Chromatic 1=Blues 2=Dorian 3=HarmMinor 4=Major 5=Mixolydian 6=NatMinor 7=PentMaj 8=PentMin 9=Phrygian; default 0
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
// Note audition state (step_button_routine.ino):
int8_t         audition_sounding_note  // MIDI pitch of the currently sounding audition note; -1 = none
unsigned long  audition_note_off_ms    // millis() timestamp when the audition note-off should fire
```

## Sequencer Flow

1. `seq.run()` (FifteenStep) ticks the sequencer on each `loop()` call
2. Timing is driven by the **TC4 hardware timer** (internal mode) or **incoming USB-MIDI 0xF8** (external mode) — both call `seq.hardwareClockPulse()` which sets volatile flags; `seq.run()` processes those flags in main-loop context
3. On each step change, `stepsend(current_step, last_step)` fires as the step callback
4. `stepsend()` first computes `play_step` from `current_step` via the `pattern_direction` switch (Fwd=identity; Rev=mirror; Pong=ping_pong_step counter; Rand=random(); Shuf=shuffle_order[shuffle_pos]; E/O=even-then-odd interleave; In=outside-in; Quad=Q1,Q3,Q2,Q4 reordering). Then it scans all 16 `sounding_note_end_step[]` entries — any slot whose end-step equals `current_step` gets a note-off (gate timing is always in hardware clock steps). If `step_data[pattern][0][play_step]` is on, `step_probability[pattern][play_step]` is checked: the step fires only if `prob >= 100` or `random(100) < prob`. If it fires, drift is applied: `total_drift = pitch_drift (if enabled) + step_drift_amount[pattern][play_step] (if step_drift_enabled is set and ft_drift_mode is on)`; a single random offset in `[-total_drift, +total_drift]` is added to the shifted pitch, clamped to `[slider_map_low_value, slider_map_high_value]`, and re-quantized to the active scale if any. Note-on is sent; `sounding_note_end_step[play_step]` is set to `(current_step + step_gate[pattern][play_step]) % pattern_length`. `last_triggered_step` is updated to `play_step` and `update_line2` is set for the LCD. If `cc_step_enabled[pattern][play_step]` is set, a CC message is sent using `cc_number[pattern]` and `cc_step_values[pattern][play_step]`; CC steps fire independently of note probability.
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

# 

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

## Navigation

D-pad up/down on the main screen selects the active pattern (wraps within 1–4 simple / 1–16 advanced). D-pad left is a no-op on the main screen (consumed). Timing modes and tempo editing via d-pad have been removed — tempo is now in the config menu.

**Enter button** (single tap, both simple and advanced mode) cycles the active slider mode, skipping any modes whose feature flag is off: NN → VL → GT → CC → PR → LV → D → NN (only enabled modes appear). Double-tap Enter opens the config menu in both modes. The enter LED is not toggled by the enter button. While `live_cc_editing_lane >= 0` (LV-mode lane edit active), Enter exits editing instead of cycling slider mode.

**`timing_mode` variable has been removed.** `switch_timing_mode_events()` and `set_timing_resolution()` no longer exist. Swing, clock source, MIDI channel, and tempo have all moved to the config menu (double-tap Enter).

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

The 16 voice sliders operate in one of seven modes:

| Mode   | Display                | Slider controls                                                             | Storage                                                         |
| ------ | ---------------------- | --------------------------------------------------------------------------- | --------------------------------------------------------------- |
| 1 — NN | `?5?4` (note icon)     | MIDI note number (mapped to `slider_map_low_value`–`slider_map_high_value`) | `pattern_step_pitches[p][s]`, `voice_slider_midinotenum[s]`     |
| 2 — VL | `?5?2` (velocity icon) | MIDI velocity (1–127)                                                       | `pattern_step_velocities[p][s]`, `voice_slider_midivelocity[s]` |
| 3 — GT | `?5G`                  | Gate length (1–16 steps)                                                    | `step_gate[p][s]`                                               |
| 4 — CC | `?5?3` (CC icon)       | CC value (0–127) per step; step buttons toggle `cc_step_enabled` on/off     | `cc_step_values[p][s]`, `cc_step_enabled[p][s]`                 |
| 5 — PR | `?5P`                  | Fire probability (0–100%) per step                                          | `step_probability[p][s]`                                        |
| 6 — LV | `?5L`                  | **Live** MIDI CC (transmitted on movement, not sequenced)                   | `live_cc_number[s]` (global), `live_cc_last_sent[s]`            |
| 7 — D  | `?5D`                  | Per-step drift amount (0–12 semitones); step buttons toggle `step_drift_enabled` on/off | `step_drift_amount[p][s]`, `step_drift_enabled[p][s]`           |

**Switching modes:**

- **Enter single-tap** (both simple AND advanced mode) cycles NN → VL → GT → CC → PR → LV → D → NN, skipping any mode whose feature flag is off. (This is a change from earlier behavior where Enter had no function in advanced mode.)
- **Advanced mode pattern buttons** remain as quick shortcuts: 1 = NN, 2 = VL, 3 = PR. CC, GT, and LV are reachable from advanced mode only via the Enter cycle (or the relevant config menu shortcut for sequenced CC).
- **Step prob** menu item still jumps directly to PR. There is no equivalent menu shortcut into LV — use the Enter cycle.

**`set_slider_mode(mode)`**: Central entry point for all slider mode changes. Sets `slider_mode`, arms `slider_needs_pickup[i] = true` for all 16 sliders **except mode 6 (LV)** which has no pickup, sets `update_line1 = update_line2 = true`, and switches step LED display: entering CC mode calls `read_cc_step_memory()` (LEDs show `cc_step_enabled`); entering D mode calls `read_drift_step_memory()` (LEDs show `step_drift_enabled`); entering LV mode clears all step LEDs (lit only for the lane currently being edited); leaving CC/D/LV mode restores the appropriate display. Entering LV also resets `live_cc_editing_lane`, `live_cc_last_lane`, and seeds `live_cc_last_sent[i] = 255` so the first move on any lane transmits. Always call this function — never set `slider_mode` directly.

**Slider takeover modes (`slider_takeover`)**: Global setting in the Takeover config menu item; persisted to EEPROM (addr 1861) and SD (`"slider_takeover"` key). LV mode (6) is exempt from this setting — it's always Jump-like.

- **0 = Catch** (default): slider must physically cross within the per-mode tolerance of the stored value before it takes over. Tolerances: ±1 note (NN), ±1 velocity unit (VL), exact match (GT), ±1 CC unit (CC), ±1 probability unit (PR), ±1 semitone (D). While any slider is still pending after a mode switch, LCD line 2 shows a 16-character overlay with `^` (push up), `v` (pull down), or space (engaged) per slider — gated by `slider_pickup_overlay_active`, auto-clears when all sliders engage. Overlay does NOT appear on pattern switches.
- **1 = Jump**: pickup is armed on mode switch, but the unlock criterion is "any movement of the physical slider past `JUMP_RAW_THRESHOLD` (32 ADC units) from the seed position." Once a slider is touched its current value overwrites the stored value immediately. No overlay.
- **2 = Relative**: pickup is bypassed entirely; every read computes `raw_delta = raw - slider_last_raw[i]`, converts it to value units (1:1 — full physical travel = full mode range), and adds the delta to the stored value (clamped). Sub-unit movement accumulates in `slider_last_raw` via the standard remainder pattern (`last_raw = raw - (raw_delta % adc_per_unit)`). For NN with a scale active, the delta moves by N indices through `scale_note_pool`. No overlay.

`slider_needs_pickup[s]` is the pickup-armed flag (Catch and Jump both consult it; Relative ignores it). Pattern switches always arm pickup regardless of takeover mode but never raise the overlay — that's reserved for explicit mode changes via `set_slider_mode()` and the in-menu takeover-mode change.

**LV mode (6) has no pickup** — the slider's position IS the value, and the 255 sentinel in `live_cc_last_sent[]` ensures the first move on any lane always transmits.

**CC mode step buttons**: In CC mode, step buttons toggle `cc_step_enabled[p][s]` on/off (independent of `step_data`). A step can have CC enabled without having a note, and vice versa. The step LED reflects `cc_step_enabled` state while in CC mode.

**D mode step buttons**: In D mode, step buttons toggle `step_drift_enabled[p][s]` on/off (independent of `step_data`). A step can have drift enabled without having a note (no audible effect until the step has a note). The step LED reflects `step_drift_enabled` state while in D mode. No gate-set gesture, no audition.

**D mode drift application**: Per-step drift is **additive** with the global `pitch_drift`. At trigger time, `stepsend()` computes `total_drift = pitch_drift (if ft_pitch_drift) + step_drift_amount[p][s] (if ft_drift_mode && step_drift_enabled[p][s])`, then picks a single random offset in `[-total_drift, +total_drift]` and applies it to the pitch (clamped to note range, then re-quantized to the active scale if any). A step with global drift = 3 and per-step drift = 5 has effective range ±8 semitones on that step only.

**LV mode step buttons**: Step buttons in LV mode select which lane's CC# is being edited. Tap a step → that lane becomes `live_cc_editing_lane` and its LED lights. Tap the same step again → exit editing. Tap a different step → switch focus. No `step_data` toggling, no gate-set gesture, no audition. While `live_cc_editing_lane >= 0`, d-pad up/down adjusts `live_cc_number[lane]` via the same valid-CC list as sequenced CC (skip 32, 96–101); d-pad left or Enter exits editing. Changing the CC# resets `live_cc_last_sent[lane] = 255` so the next slider movement retransmits on the new CC#.

**LV slider read**: `run_voice_slider_routine()` mode-6 branch reads raw 12-bit ADC via `Potentiometer::getValue()` (bypasses the sectorization used by other modes) and maps to 0–127 via `raw >> 5`. If the value differs from `live_cc_last_sent[i]`, sends `controlChange(live_cc_channel - 1, live_cc_number[i], value)` and updates `live_cc_last_sent[i]` and `live_cc_last_lane`. The 20 ms `last_slider_ms` rate-limit at the top of `run_voice_slider_routine()` still applies — live CC will not transmit faster than ~50 Hz per slider, which is plenty for smooth control without flooding USB-MIDI.

**LV MIDI channel** is `live_cc_channel` (1–16), set via config menu → **Live CC ch**. Independent of `MIDICHANNEL` (which the note sequencer and sequenced CC use). This allows routing live CC to a different synth/destination.

**LV persistence**: `live_cc_number[16]` and `live_cc_channel` are saved to both SD JSON (`"live_cc_channel"`, `"live_cc_numbers": [..16..]`) and EEPROM (addresses 1331 and 1332–1347). `ft_live_cc_mode` is also persisted (SD `"ft_live_cc_mode"`; EEPROM address 1330).

**D persistence**: `step_drift_enabled[16][16]` and `step_drift_amount[16][16]` are saved to both SD JSON (per-pattern `"drift_enabled":[16]` and `"drift_amounts":[16]`) and EEPROM (addresses 1349 and 1605). `ft_drift_mode` is also persisted (SD `"ft_drift_mode"`; EEPROM address 1348). The drift arrays are optional in JSON — older saves without them load cleanly and default to all-off. Bumping EEPROM_MAGIC_VALUE to 0xCF forces old saves without these fields to be re-defaulted on first boot after the upgrade.

**Always boots to NN mode** (`slider_mode = 1`). Mode is not saved to SD/EEPROM — it resets to NN on power-up.

**`resetSliders()`**: Resets all 16 steps' pitches to `slider_map_low_value`, velocities to 127, gates to 1, probabilities to 100, and drift to off/0, for the current pattern.

## Multi-Step Gate Lengths

Each step has a gate length (1–16) stored in `step_gate[pattern][step]`. A gate of 1 means the note-off fires at the next step (classic behavior). A gate of N means the note rings for N steps.

**Implementation**: `stepsend()` sets `sounding_note_end_step[current_step] = (current_step + gate) % 16` when a note fires. On every step advance, `stepsend()` scans all 16 `sounding_note_end_step[]` entries and sends note-off for any that match `current_step`. This allows multiple overlapping notes to ring simultaneously and expire independently.

**Note**: gate lengths wrap around step 16→0, so a gate of 16 on step 0 will hold until step 0 of the next loop (or the next pattern if chained).

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

## MIDI Learn

`read_midi()` in `midi_processor.ino` captures incoming USB-MIDI Control Change messages (USB-MIDI CIN `0x0B`) and writes the controller number into the CC# the user is currently editing. No explicit arm gesture — Learn is implicit whenever an edit predicate is true:

- **Sequenced CC# (per-pattern)**: `config_menu_active && config_editing_value && config_menu_item == CONFIG_ITEM_CC_NUMBER`. Writes to `cc_number[pattern_value]`.
- **Live CC# (per-lane)**: `slider_mode == 6 && live_cc_editing_lane >= 0`. Writes to `live_cc_number[live_cc_editing_lane]` and resets `live_cc_last_sent[lane] = 255` so the next slider movement retransmits on the new CC#.

If neither predicate is true, incoming CC messages are ignored (and still passed through MIDI thru, like everything else in `read_midi()`). The same valid-CC filter as manual selection applies — controllers in 1–119 minus 32 and 96–101 are accepted; filtered controllers are ignored and the edit stays armed. Channel is ignored: only the controller number is captured. `live_cc_channel` and `MIDICHANNEL` are never modified by Learn.

## Pattern Operations

- **Set gate length (normal step-edit mode, not CC mode, not advanced nav mode)**: Hold one step button for ≥ 150 ms, then tap a second step button. The source step is turned ON and its gate is set to the forward distance between the two buttons (1–16, wrapping). The destination step does not toggle. LEDs flash the gate range (both endpoints lit) for 300 ms then restore. A plain tap (< 150 ms, released without tapping another) still toggles the step normally. Turning a step OFF (plain tap when already on) always resets its gate to 1.
- **Copy pattern (simple mode)**: Hold a pattern select button for 2s, then press destination pattern button
- **Copy pattern (advanced mode)**: Double-click pattern button 0 (within 400 ms) → phase 1: tap step = source pattern → phase 2: tap step = destination pattern
- **Copy pattern (advanced mode, nav-mode hold-arm shortcut)**: While in pattern-nav mode (`adv_pat_nav_active`), hold any step button for ≥ 2 s → arms phase 2 directly with that step's pattern as the source (skips phase 1, no "copy which pat" prompt) → tap any step = destination. Does NOT fire in normal step-edit mode. If a chain-define gesture has already consumed the hold (second-tap before 2 s), hold-arm is suppressed.
- **Cancel copy (advanced mode)**: D-pad left while copy is armed
- **Chain 4 patterns (simple mode)**: Press pattern buttons 0 + 3 simultaneously to toggle
- **Select pattern 0–15 (advanced mode)**: Hold pattern button 0, tap a step button

**Pattern copy includes velocities, gates, CC data, and drift data**: Both `listen_for_copy_command()` (simple mode) and the advanced mode 2-phase copy loop copy `pattern_step_velocities`, `step_gate`, `cc_step_enabled`, `cc_step_values`, `cc_number`, `step_probability`, `step_drift_enabled`, and `step_drift_amount` along with `step_data` and `pattern_step_pitches`.

**`go_to_pattern(pattern, silent)`**: Turns all 4 pattern LEDs off, except LED 0 stays on when `adv_pat_nav_active` is true. In simple mode, lights the LED for `pattern % 4`. In advanced mode outside nav mode, no LED is lit — buttons are function keys. `toggle()` was previously used but is state-dependent; always use `on()`. The `silent` parameter is accepted but currently unused.

**Pattern LEDs in advanced mode**: While `adv_pat_nav_active` is true, LED 0 stays lit (managed by `run_pattern_select_routine()`). Outside nav mode, no LEDs are lit — buttons are function keys. Do not call `pattern_select_leds[x].on()` from `go_to_pattern()` in advanced mode.

**Advanced mode pattern-nav mode**: Single-click of pattern button 0 (400 ms timeout with no second press) toggles `adv_pat_nav_active`. While active, step buttons select/chain patterns instead of editing steps. The nav loop in `run_pattern_select_routine()` blinking the current pattern's LED at 200 ms period; all chain patterns are solid. Tap one step = single pattern select (clears chain). Hold one step (`adv_chain_hold_step`) and tap a second = define `chain_start..chain_end` range and set `extended_step_length_mode = 1`. Wrap-around chains (start > end) are fully supported. On exit from nav mode, `read_step_memory()` restores the step LEDs.

**Advanced mode 2-phase copy**: Double-click pattern button 0 (two `pattern_select_button_flags[0]` within 400 ms) enters copy mode. Phase 1 (`adv_copy_waiting_source`): LCD shows `copy which pat?` (flag 103); tap a step button to navigate to that pattern and arm phase 2. Phase 2 (`adv_copy_armed`): LCD shows `Copy N->where?` (flag 102); tap a step button to copy to that pattern; LCD shows `Copied X to Y` (flag 101) for 500 ms then returns to main display. D-pad left in `listen_for_navigation_events()` cancels both phases at any point.

**Chain toggle one-shot guard**: The `isPressed()` check for pattern buttons 0+3 fires every loop iteration while both are held. A `static bool chain_toggle_handled` in `run_pattern_select_routine()` ensures the mode flip and `go_to_pattern()` call happen only once per press. It resets when the buttons are released. Do not remove this guard — without it the mode flips back and forth on every loop frame.

**`clear_pattern_memory()` clears all 16 patterns**: Loops over all 16 patterns (`p = 0..15`) and zeros every step, resets pitches to `slider_map_low_value`, resets velocities to 127, resets gates to 1, resets `step_probability` to 100, clears CC data (enabled and values), and clears per-step drift (`step_drift_enabled` and `step_drift_amount` both zero). After clearing, calls `read_step_memory(0, pattern_value)` to refresh the LEDs for the active pattern. `clear_pattern_memory_for_voice(0)` does the same for the current pattern only. These functions are called from the config menu — the old step-button-combo triggers (step 0+15 / step 0+11) have been removed.

**Pattern chain auto-advance**: When `extended_step_length_mode == 1`, `stepsend()` calls `run_auto_pattern_select_routine()` at `current_step == pattern_length - 1`. This advances `current_pattern` within `chain_start..chain_end`. **Wrap-around chains** (where `chain_start > chain_end`, e.g. start=7, end=2 → plays 7,8,…,15,0,1,2) are fully supported. The advance happens at the start of step 15 so step 15 plays from the current pattern; the new pattern takes over from step 0.

## Simple vs Advanced Mode

**Simple mode** (`advanced_mode == false`, default):

- Pattern buttons 0–3 select patterns 0–3 directly
- Pattern buttons 0+3 simultaneously toggle chain mode (4 patterns)
- Hold any pattern button 2s → pattern copy (press destination pattern button)
- Pattern LEDs show the active pattern (0–3)
- D-pad mode 1 navigates patterns 1–4
- Enter single-tap cycles slider mode through all enabled modes: NN → VL → GT → CC → PR → LV → D → NN

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
- **Pattern button 2** → slider mode VL (velocity); LED 2 stays lit
- **Pattern button 3** → slider mode PR (probability); LED 3 stays lit
- Activating advanced mode force-enables `ft_velocity_mode` and `ft_probability` so buttons 2 and 3 always work
- Pattern LEDs: LED 0 = nav mode active; LEDs 1/2/3 = active slider mode indicator
- D-pad mode 1 navigates patterns 1–16
- Hold-for-2s pattern copy is disabled in advanced mode
- Enter single-tap cycles slider mode in advanced mode too (NN → VL → GT → CC → PR → LV → D → NN, skipping disabled modes). Pattern buttons 1/2/3 remain as quick shortcuts to NN/VL/PR.

## Config Menu

Entered by **double-tapping Enter** (two presses within 400 ms). The menu is modal — all d-pad and enter flags are consumed by `run_config_menu()` and `listen_for_navigation_events()` is suppressed while active. The sequencer continues playing normally in the background.

**Navigation**: d-pad up/down scrolls. Line 1 shows `> {current item}`, line 2 shows the next item (no cursor). D-pad left exits from anywhere (also cancels a pending confirmation). Enter selects.

**Menu items (alphabetical order)**:

Items whose feature flag is disabled are skipped during d-pad scrolling (`config_menu_next()` handles this). Disabled items still exist in the item list — they just become invisible until their feature is re-enabled.

1. **CC number** — enter editing sub-state; up/down cycles valid CC numbers (1–119, skipping 32 and 96–101); label shows `CC:{number} {name}`. Hidden when `ft_cc_mode` is off
2. **Channel** — enter editing sub-state; up/down adjust 1–16; Enter or Left exits editing
3. **Clear/Reset** — enters the Reset/Clear submenu (see below)
4. **Clock: int/ext** — toggles immediately via `setExternalClockMode()`; value shown inline on line 1. Hidden when `ft_external_clock` is off
5. **Diagnostics** — opens the Diagnostics submenu (LED test, Input test, Hi trim — see below). Hidden when `ft_diagnostics` is off
6. **Exit** — Enter, left, or right all exit
7. **Features** — enters the Features submenu (see below)
8. **Live CC ch** — enter editing sub-state; up/down adjust 1–16; Enter or Left exits editing. Sets the MIDI channel used by LV slider mode (independent of `MIDICHANNEL`). Hidden when `ft_live_cc_mode` is off
9. **Mode: Simple/Advanced** — confirmation required; Enter toggles Simple↔Advanced; line 1 shows target (`Mode:->Simple ` or `Mode:->Advancd`); Left cancels. Enabling Advanced also force-enables `ft_velocity_mode` and `ft_probability`. Hidden when `ft_advanced_mode` is off
10. **Note range** — opens Note range submenu (see below); label shows `*` when non-default (36/52). Always visible (not gated by any feature flag)
11. **Note scales** — two-phase editor: Enter starts editing scale type (`Sc: Major`), Enter again switches to root note (`Root: C#`), Enter exits; changing scale or root calls `apply_scale_to_all_patterns()` which quantizes all stored pitches immediately; label shows `*` when non-Chromatic/C. Scales: Chromatic Blues Dorian HarmMinor Major Mixolydian NatMinor PentMaj PentMin Phrygian. Hidden when `ft_scale_quantization` is off
12. **Note shift** — enter editing sub-state; up/down adjust ±1 semitone (range -12 to +12); label shows `*` when non-zero. Hidden when `ft_octave_note_shift` is off
13. **Octave shift** — enter editing sub-state; up/down adjust ±1 octave (range -5 to +5); label shows `*` when non-zero. Hidden when `ft_octave_note_shift` is off
14. **Pat dir** — enter editing sub-state; up/down cycles 0–7; names: Fwd/Rev/Pong/Rand/Shuf/E·O/In/Quad. Hidden when `ft_pattern_direction` is off
15. **Pat length** — enter editing sub-state; up/down adjust 1–16; tap any step button sets length to N+1; step LEDs show active length while editing; `seq.setSteps()` called on every change; label shows `*` when not 16. Hidden when `ft_variable_pat_length` is off
16. **Pitch drift** — enter editing sub-state; up/down adjust 0–7 semitones; label shows `*` when non-zero. Hidden when `ft_pitch_drift` is off
17. **Save** — saves to SD (primary) + EEPROM (backup); blocked while playing (shows `Stop first!` on line 2); exits menu and shows `saved!` on success
18. **Slider takeover (Takeover:)** — enter editing sub-state; up/down cycles Catch / Jump / Relative; line 1 shows current value. Changing the value re-seeds `slider_last_raw[]` from current ADC reads and clears/sets `slider_needs_pickup[]` according to the new mode. Always visible.
19. **Step prob** — exits menu immediately and activates PR slider mode; label shows `*` if any step probability < 100. Hidden when `ft_probability` is off
20. **Swing** — enter editing sub-state; up/down adjust 0–5; Enter or Left exits editing. Hidden when `ft_swing` is off
21. **Tempo** — only visible when external clock is OFF; Enter starts editing (line 2 shows resolution); Enter again cycles resolution ±10 → ±1 → ±0.1 BPM; up/down adjusts at current resolution; Left exits. Calls `seq.setTempo()` and `setSequencerTimerPeriod()` on every change

**Reset/Clear submenu**: Scrolled with up/down, Enter shows confirmation (`Entr=ok  Lft=no`), Enter again executes, Left cancels confirmation or exits submenu back to main menu. Items: Clear all pats, Clear pattern, Reset live CCs, Reset sliders. **Reset live CCs** restores `live_cc_number[]` to the default 102–117 (MIDI "Undefined" range) and zeroes `live_cc_last_sent[]` to 255 so the next slider move retransmits — does not touch `live_cc_channel` or any other state.

**Note range submenu**: Scrolled with up/down, Left exits back to main menu. Line 1 shows `> {current item}`, line 2 shows next item preview. Items: Custom, 16 notes (36–51), 12 notes (36–47), 8 notes (36–43), 6 notes (36–41), 4 notes (36–39). Selecting a preset immediately sets `slider_map_low_value=36` and `slider_map_high_value` to the preset top, calls `init_blank_patterns_to_range()` and `build_scale_notes()`, then returns to the main menu. Selecting Custom enters a two-phase inline editor: Enter advances lo→hi, Enter again exits; Left returns to the preset list. All presets anchor the low note at 36.

**Diagnostics submenu**: Scrolled with up/down, Left exits back to main menu. Items: LED test, Input test, Hi trim. LED test enters `diag_submode=1` (non-blocking sequential LED chase through 16 step + 4 pattern + play + enter LEDs at 80 ms/LED; Left exits back to submenu). Input test enters `diag_submode=0` (existing button/slider display; Left or double-tap Enter exits back to submenu). Hi trim enters an inline editor: up/down adjusts `slider_hi_trim` 0–4; Enter or Left exits. Both diag submodes set `diag_mode=true` which gates the main loop; on exit they call `draw_diag_submenu()` and leave `config_menu_active=true` so normal LCD updates are suppressed (the LCD.ino `config_menu_active` guard prevents overwriting the submenu display).

**Features submenu**: Scrolled with up/down, Enter toggles on/off, Left exits. Shows all 14 `ft_*` flags by name. Toggling a flag off has side effects via `_apply_feature_disable()`: switching slider mode off while active falls back to NN mode; disabling advanced mode resets nav state; disabling note audition cancels any currently sounding audition note. Flags are NOT saved automatically — use Save after making changes.

| Feature name      | Flag                  | What it enables                                      |
|-------------------|-----------------------|------------------------------------------------------|
| Advanced mode     | `ft_advanced_mode`    | 16 patterns, pattern-nav/copy/chain, adv button layout |
| CC mode           | `ft_cc_mode`          | Slider mode 4 (CC), CC number menu item              |
| Probability       | `ft_probability`      | Slider mode 5 (PR), Step prob menu item              |
| Gate sliders      | `ft_gate_mode`        | Slider mode 3 (GT) — key-combo gate always works (default OFF) |
| Note scales       | `ft_scale_quantization` | Scale quantization, Note scales menu item (Note range is always visible) |
| Pitch drift       | `ft_pitch_drift`      | Pitch drift menu item                                |
| Pat direction     | `ft_pattern_direction`| Pattern direction menu item                          |
| Pat length        | `ft_variable_pat_length` | Variable pattern length menu item                 |
| Swing             | `ft_swing`            | Swing menu item                                      |
| Ext clock         | `ft_external_clock`   | External clock menu item                             |
| Oct/note shift    | `ft_octave_note_shift`| Octave shift + Note shift menu items                 |
| Diagnostics       | `ft_diagnostics`      | Diagnostics menu item + hardware test mode           |
| Velocity sliders  | `ft_velocity_mode`    | Slider mode 2 (VL)                                   |
| Note audition     | `ft_note_audition`    | MIDI note preview on step-on while stopped (default ON) |
| Live CC mode      | `ft_live_cc_mode`     | Slider mode 6 (LV), Live CC ch menu item (default ON) |
| Drift mode        | `ft_drift_mode`       | Slider mode 7 (D, per-step drift, additive with global pitch_drift, default ON) |

**Double-tap detection**: implemented in the main `loop()` with `last_enter_ms` and `enter_tap_pending` statics. The first tap starts a 400 ms window without immediately setting `enterbutton_flag` — this prevents the first press of a double-tap from accidentally triggering a slider mode change. If a second `uniquePress()` arrives within the window, `enter_config_menu()` fires. If the window expires without a second tap, `enterbutton_flag` is set as a normal single-tap. Single-tap actions are therefore delayed by up to 400 ms, which is imperceptible for mode-cycling use.

**Save timing**: `EEPROM.commit()` can stall the CPU 10–50 ms during flash programming. Saving while playing would cause a timing hiccup. The Save item checks `playstatus` and refuses with `Stop first!` if the sequencer is running.

## SD Card Storage

**Primary storage**: `/synthseqr/autosave.json` on the onboard SD card slot (uses `SDCARD_SS_PIN`, not GPIO 10). The folder is created automatically on first init.

**Boot sequence**: `boot_load()` in `sd_storage.ino` calls `sd_init()` then `load_from_sd()`. On failure (no card, no file), falls back to `load_from_eeprom()`. Called from `setup()` instead of the old direct `load_from_eeprom()`.

**Save**: `save_everywhere()` writes to SD first, then EEPROM. Called from the config menu Save item.

**JSON format**: hand-rolled minimal parser — no ArduinoJson dependency. Scans for known keys by name, ignores unknown keys (forward-compatible with extra fields added by external tools). All 16 patterns are saved including velocities, gates, probabilities, and pitch_drift. Users can hand-edit or generate JSON externally and load it on the device.

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
  "pitch_drift": 0,
  "chain_active": 0,
  "chain_start": 0,
  "chain_end": 3,
  "advanced_mode": 0,
  "pattern_length": 16,
  "pattern_direction": 0,
  "ft_live_cc_mode": 0,
  "ft_drift_mode": 1,
  "live_cc_channel": 1,
  "live_cc_numbers": [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16],
  "patterns": [
    {
      "steps":[1,0,...16 values],
      "pitches":[36,37,...16 values],
      "velocities":[127,127,...16 values],
      "gates":[1,1,...16 values],
      "cc_number": 1,
      "cc_enabled":[0,0,...16 values],
      "cc_values":[0,0,...16 values],
      "probabilities":[100,100,...16 values],
      "drift_enabled":[0,0,...16 values],
      "drift_amounts":[0,0,...16 values]
    },
    ...16 patterns
  ]
}
```

`pitch_drift`, `probabilities`, `drift_enabled`, and `drift_amounts` are optional in the JSON — older saves without them load cleanly and default to 0 / 100 / 0 / 0 respectively.

**Arduino prototype issue**: The Arduino build tool auto-generates function prototypes before `#include`s are processed. Functions with `File&` parameters fail with "File not declared in this scope". All SD helper functions use a module-level `static File _f` handle instead — no `File` type appears in any function signature. Do not add `File&` parameters to helpers in `sd_storage.ino`.

## EEPROM Save / Load

EEPROM is a **silent fallback** — used only when SD is unavailable on boot. Save still writes to both (`save_everywhere()`). Velocities and gates are **not stored in EEPROM** (SD only) — on EEPROM-only boot they default to 127 and 1 respectively. CC data (cc_number, cc_step_enabled, cc_step_values) **is** stored in EEPROM.

**On boot**: `load_from_eeprom()` is called only if `load_from_sd()` returns false. Checks for a magic sentinel byte at address 0. If missing (first boot or layout change), globals keep compiled-in defaults.

**EEPROM layout** (1861 bytes, defined as `#define` constants in `storage.ino`):

| Address | Size | Content                                        |
| ------- | ---- | ---------------------------------------------- |
| 0       | 1    | Magic byte `0xCF`                              |
| 1       | 1    | `MIDICHANNEL`                                  |
| 2       | 1    | `SWING`                                        |
| 3       | 4    | `TEMPO` (float)                                |
| 7       | 1    | `current_pattern`                              |
| 8       | 1    | `extended_step_length_mode`                    |
| 9       | 1    | `external_clock_mode`                          |
| 10      | 256  | `step_data[16][16]` (1 byte per step)          |
| 266     | 256  | `pattern_step_pitches[16][16]`                 |
| 522     | 1    | `octave_shift` (int8_t as raw byte)            |
| 523     | 1    | `advanced_mode` (bool)                         |
| 524     | 1    | `note_shift` (int8_t as raw byte)              |
| 525     | 1    | `slider_map_low_value` (uint8_t)               |
| 526     | 1    | `slider_map_high_value` (uint8_t)              |
| 527     | 1    | `pattern_length` (uint8_t, 1–16)               |
| 528     | 1    | `pattern_direction` (uint8_t, 0–7)             |
| 529     | 1    | `scale_root` (uint8_t, 0–11)                   |
| 530     | 1    | `scale_type` (uint8_t, 0–8)                    |
| 531     | 16   | `cc_number[16]` (one byte per pattern)         |
| 547     | 256  | `cc_step_enabled[16][16]` (one byte per step)  |
| 803     | 256  | `cc_step_values[16][16]` (one byte per step)   |
| 1059    | 1    | `pitch_drift` (uint8_t, 0–7)                   |
| 1060    | 256  | `step_probability[16][16]` (one byte per step) |
| 1316    | 13   | runtime feature flags (`ft_*`)                 |
| 1329    | 1    | `slider_hi_trim` (uint8_t, 0–4)                |
| 1330    | 1    | `ft_live_cc_mode` (bool)                       |
| 1331    | 1    | `live_cc_channel` (uint8_t, 1–16)              |
| 1332    | 16   | `live_cc_number[16]` (CC# per LV lane)         |
| 1348    | 1    | `ft_drift_mode` (bool)                         |
| 1349    | 256  | `step_drift_enabled[16][16]`                   |
| 1605    | 256  | `step_drift_amount[16][16]` (0–12)             |
| 1861    | 1    | `slider_takeover` (uint8_t, 0=Catch 1=Jump 2=Relative) |

**Validation**: All loaded values are range-checked so corrupted flash can't break the sequencer. CC numbers validated to be in safe range (1–119, not 32, not 96–101). `pitch_drift` validated 0–7; `step_probability` validated 0–100; `step_drift_amount` validated 0–12; `step_drift_enabled` coerced to 0/1.

**`EEPROM.commit()` is required**: `storage.ino` uses `FlashAsEEPROM_SAMD`. All writes buffer in RAM until `commit()` burns to flash. Without it, saves vanish on power-off.

**Magic byte**: Increment `EEPROM_MAGIC_VALUE` in `storage.ino` whenever the layout changes. Current value: `0xCF`.

**LCD confirmation**: `lcdflag = 202` shows `saved!` for 2 seconds using a `static unsigned long msg_until` timer inside the LCD case, then returns to the main display.

## Note Audition

When `ft_note_audition` is true and the sequencer is stopped (`!playstatus`), step button presses trigger a live MIDI preview in addition to toggling the step.

- **Step toggled ON**: `do_step_toggle()` calls `audition_step_note(i, 1)` — sends a note-on with gate = 1 × 16th note duration at `TEMPO`.
- **Gate-set gesture fires**: after `do_step_on()` + gate assignment, `detect_step_button_presses()` calls `audition_step_note(src, gate)` — sends a note-on with the actual gate just set, so the full ring time is audible.
- **Step toggled OFF**: no audition (the note is being silenced, not added).
- **CC mode**: unaffected — CC step buttons always toggle `cc_step_enabled` immediately, no audition.

**`audition_step_note(int step, uint8_t gate_steps)`** (in `step_button_routine.ino`):
1. Cancels any currently sounding audition note (note-off + flush).
2. Computes pitch from `voice_slider_midinotenum[step]` with `octave_shift`, `note_shift`, and scale quantization applied — same transforms as `stepsend()`, but **no pitch drift** (preview is deterministic).
3. Sends note-on with `voice_slider_midivelocity[step]`.
4. Sets `audition_sounding_note` and `audition_note_off_ms = millis() + gate_ms`. Minimum gate enforced at 50 ms.

**Note-off timer**: top of `run_step_button_routine()` checks `(long)(millis() - audition_note_off_ms) >= 0` and sends note-off when the gate expires.

**Disable side-effect**: `_apply_feature_disable(6)` in `config_menu.ino` immediately cancels any sounding audition note when the feature is toggled off.

**Default**: `ft_note_audition = true` — the feature is on by default. Disable from Features submenu if you don't want step-toggle previews while stopped.

## Diagnostics Mode

Entered from the config menu: Diagnostics → Enter → opens the Diagnostics submenu. There is no hardware hold-combo entry point.

**`bool diag_mode`** — declared in `diagnostics.ino`, forward-declared as `extern` in `config.h`. When true:

- `loop()` calls `run_diagnostics()` then executes `if (diag_mode) return;` — everything after (navigation, step buttons, pattern select, sliders, `run_LCD_update()`) is skipped.
- `run_LCD_update()` also early-returns if `diag_mode` is true, so the normal LCD system cannot overwrite diagnostics output.

**`uint8_t diag_submode`** — selects which mode runs while `diag_mode` is true: 0 = input test, 1 = LED test.

**`run_diagnostics()`** — branches on `diag_submode`: calls `run_diag_led_test()` (submode 1) or `run_diagnostics_display()` (submode 0).

**LED test (`diag_submode = 1`, `run_diag_led_test()`)** — non-blocking sequential LED chase: one LED on at a time, 80 ms per LED, cycling through 16 step LEDs → 4 pattern LEDs → play LED → enter LED (22 total). Left exits: turns all LEDs off, calls `read_step_memory()` + `go_to_pattern()` to restore state, calls `draw_diag_submenu()`. `diag_mode` and `config_menu_active` both remain in sync so the LCD guard prevents normal updates from overwriting the submenu on re-entry.

**Input test (`diag_submode = 0`, `run_diagnostics_display()`)** — called once per loop iteration. Non-blocking. Polls:

- D-pad up/down and Enter via `uniquePress()` — writes button name + pin to LCD line 1.
- D-pad left — exits input test back to diag submenu (restores LEDs, calls `draw_diag_submenu()`).
- Play button via `play_button_isr_fired` flag (same as main loop) — writes to line 1.
- Step buttons — first pressed wins per frame; LED toggles as secondary visual confirm.
- Pattern select buttons — first pressed wins per frame; PAT1 enters the save-file viewer sub-mode.
- Voice sliders — raw `analogRead()` on the actual analog pin (bypasses `Potentiometer` class); change threshold ±16 ADC counts; rate-limited to once per 100 ms; writes slider index, pin name, and raw 0–4095 value to LCD line 2.
- Auto-clears to idle screen after 2 s of no activity.
- Double-tap Enter exits back to diag submenu.

**LCD format:**

- Line 1 (buttons): `"%-8s pin:%3d"` → 16 chars. Example: `STEP01   pin: 23`
- Line 2 (sliders): `"SL%02d %-3s r:%4d "` → 16 chars. Example: `SL00 A15 r:4095 `

**Slider pin mapping** (matches `config.h` Potentiometer declarations, note A2/A3 swap):
`SL00=A15 SL01=A14 SL02=A13 SL03=A12 SL04=A11 SL05=A10 SL06=A9 SL07=A8 SL08=A7 SL09=A6 SL10=A5 SL11=A4 SL12=A2 SL13=A3 SL14=A1 SL15=A0`

`seq.run()` and `read_midi()` still execute each loop iteration so USB-MIDI does not stall if the sequencer happens to be playing when diagnostics is entered.
