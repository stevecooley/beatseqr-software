# Synthseqr Firmware — User Instructions

Hardware: Adafruit Grand Central M4 Express
Firmware version: 2.2

---

## Overview

Synthseqr is a 16-step MIDI sequencer with 4 patterns, 16 voice sliders, a D-pad for navigation, and an LCD display. It outputs MIDI notes over USB and can follow an external MIDI clock.

---

## Controls at a Glance

| Control | Location |
|---|---|
| Step buttons (16) | Main row — toggle steps on/off |
| Step LEDs (16) | Above each step button |
| Voice sliders (16) | One per step — set MIDI note pitch |
| Play button | Transport — start/stop |
| D-pad (up/down/left/right) | Navigation |
| Enter button | Confirm / toggle LCD mode |
| Pattern select buttons (4) | Select active pattern |
| Pattern select LEDs (4) | Show active pattern |

---

## Playing the Sequencer

### Start / Stop

Press the **Play button** to start. Press it again to stop.

- The play/stop icon on the top-left of the LCD updates with each press.
- Chase lights on the step LEDs follow the current playing position.
- Any note held open when you stop is automatically silenced.

### Step Buttons

Press any **step button** to toggle that step on or off.

- LED on = step is active (note will play)
- LED off = step is silent
- The sequencer fires a MIDI note-on when it arrives at an active step and a note-off when it moves away.

### Voice Sliders

Each of the 16 sliders sets the **MIDI pitch** for the corresponding step.

- Default range is MIDI notes 36–51 (C2–D#3).
- Moving a slider immediately updates the pitch for that step in the current pattern.
- Pitches are saved per-pattern — see **Pattern Memory** below.

---

## Tempo and Swing

The D-pad navigates through 8 timing modes in visual left-to-right order. Press **D-pad left/right** to move between them. Press **D-pad up/down** to adjust the selected value.

The cursor on the LCD blinks on the field that up/down currently controls.

| Mode (D-pad left/right) | Up/Down adjusts | LCD location |
|---|---|---|
| 1 | Pattern (1–4, wraps) | Line 1 — pattern digit |
| 2 | Tempo ±10 BPM | Line 1 — tempo hundreds/tens |
| 3 | Tempo ±1 BPM | Line 1 — tempo units |
| 4 | Tempo ±0.1 BPM | Line 1 — tempo tenths |
| 5 | Tempo ±0.01 BPM | Line 1 — tempo hundredths |
| 6 | Swing (0–6) | Line 2 — swing digit |
| 7 | Clock source (INT / EXT) | Line 2 — int/ext value |
| 8 | MIDI channel (1–16) | Line 2 — channel digits |

**Tempo range:** 10–250 BPM
**MIDI channel range:** 1–16
**Swing:** 0 = straight, 1–2 = mild, 3 = heavy (classic triplet feel), 6 = maximum shuffle

> **Note:** When swing is active, the MIDI clock output (0xF8) also swings. If you are syncing an external device to the sequencer's MIDI clock, set swing to 0.

### LCD Line 1

```
[play/stop] P{pattern} T{tempo}
```

Example: `▶P1 T120.00     `

### LCD Line 2

```
s{swing} clk:{int|ext} C{channel}
```

Example: `s0 clk:int C02  `

Line 2 shows swing, clock source, and MIDI channel together. The cursor sits on the active value as you navigate modes 6, 7, and 8.

---

## Clock Source (INT / EXT)

Navigate to **timing mode 6** (D-pad right five times from default).

- **D-pad up** — switch to **EXT**: the sequencer follows incoming USB-MIDI clock (0xF8 / 0xFA / 0xFC from an external device such as a DAW or drum machine).
- **D-pad down** — switch to **INT**: the sequencer uses its own internal hardware timer at the current BPM.

### External Clock Mode

When EXT is active:

- The sequencer advances only when it receives 0xF8 clock bytes from the host.
- 0xFA (MIDI Start) from the host starts the sequencer.
- 0xFC (MIDI Stop) from the host stops it.
- The sequencer does **not** send its own MIDI clock, start, or stop messages — it is purely a follower.
- The local Play button still works to arm or start the sequencer before the host sends start.
- MIDI notes are still output normally.

---

## Patterns

There are 4 patterns (1–4). Each pattern has its own 16 steps and its own set of 16 pitches.

### Selecting a Pattern

Press any of the 4 **pattern select buttons** to switch to that pattern immediately.

- The pattern select LED for the active pattern is lit.
- Step LEDs update to show the new pattern's data.
- Slider pitches are restored from the pattern's saved values (see below).

### Pattern Memory — Slider Pickup

When you switch patterns, the stored pitches for the new pattern are restored. Sliders do **not** immediately override them.

Each slider is individually locked until its physical position reaches within 1 note of the stored pitch for that step — at that point it "picks up" and tracks normally. This prevents unwanted pitch jumps when switching patterns with sliders in different positions.

### Copying a Pattern

1. Hold a **pattern select button for 2 seconds**. The LCD shows `Copy N ->`.
2. Press the **destination pattern button**. The entire pattern (steps + pitches) is copied there.

### Chaining 4 Patterns

Press **pattern buttons 1 and 4 simultaneously** to toggle chain mode.

- **Chain on** (`chain 4` on LCD): patterns auto-advance 1 → 2 → 3 → 4 → 1 ... each time step 15 is reached.
- **Chain off** (`single` on LCD): only the active pattern plays on loop.
- The LCD briefly shows the new mode (`chain 4` or `single`) as confirmation.

---

## Clearing Patterns

These combos work while the sequencer is stopped or playing.

| Action | Combo |
|---|---|
| Clear current pattern | Hold **step button 0** + **step button 15** |
| Clear all patterns | Hold **step button 0** + **step button 11** |

Clearing turns off all step LEDs and silences all steps. Slider pitches are not affected.

---

## Resetting Slider Pitches

Calling `resetSliders()` (wired to your preferred button combo or Serial command) resets all 16 pitches for the current pattern to the defaults (MIDI notes 36–51) and unlocks all sliders immediately.

---

## MIDI Output

- **Channel:** set by `MIDICHANNEL` in config.h (default: channel 2)
- **Note-on velocity:** 127 (fixed)
- **Note-off:** sent automatically when the sequencer leaves a step, and for all open notes on stop
- **MIDI clock (0xF8):** output continuously while playing (internal mode only)
- **MIDI Start (0xFA) / Stop (0xFC):** output on play/stop (internal mode only)

---

## LCD Display

### Line 1

```
[play/stop] P[pattern] T[tempo]
```

Example: `▶ P1 T120.00    `

### Line 2

```
s[swing] clk:[int|ext] Ch[channel]
```

Example: `s0 clk:int Ch02 `

The cursor blinks on the field that D-pad up/down currently adjusts.

### Enter Button

Press the **Enter button** to return to the main display from any temporary LCD screen, and to toggle the Enter LED indicator.

---

## Diagnostics Mode

Hold **Play + Enter for 2 seconds** to enter diagnostics mode. The Serial monitor (57600 baud) will print hardware test output.

In diagnostics mode:

- Press any **step button** — its LED toggles and the button index is printed to Serial.
- Press any **pattern select button** — its LED toggles and the index is printed.
- Move any **voice slider** — its index and mapped value are printed.
- Press **D-pad up** and hold — hold duration is printed continuously.
- Press **D-pad right** — prints confirmation.
- Hold **D-pad left for 2 seconds** — prints EEPROM values (addresses 100–115).

**Exit diagnostics:** press **D-pad left + D-pad right simultaneously**.

---

## Serial Monitor

Connect at **57600 baud** to see:

- Tempo, shuffle, beat length, and position on every up/down D-pad press
- Clock mode changes (`clock mode: INT` / `clock mode: EXT`)
- MIDI start/stop events
- All LCD commands mirrored as text

---

## Quick Reference Card

| Action | How |
|---|---|
| Start / stop | Play button |
| Toggle a step | Step button |
| Set step pitch | Voice slider |
| Select pattern (d-pad) | D-pad left to mode 1, up/down cycles patterns |
| Adjust tempo (coarse) | D-pad right to mode 2–3, then up/down |
| Adjust tempo (fine) | D-pad right to mode 4–5, then up/down |
| Adjust swing | D-pad right to mode 6, then up/down |
| Set clock source | D-pad right to mode 7, up=EXT / down=INT |
| Set MIDI channel | D-pad right to mode 8, up/down = channel 1–16 |
| Select pattern | Pattern button 1–4 |
| Copy pattern | Hold pattern button 2s → press destination |
| Chain 4 patterns | Pattern buttons 1 + 4 simultaneously |
| Clear current pattern | Hold step 0 + step 15 |
| Clear all patterns | Hold step 0 + step 11 |
| Diagnostics | Hold Play + Enter 2s |
| Exit diagnostics | D-pad left + right simultaneously |
