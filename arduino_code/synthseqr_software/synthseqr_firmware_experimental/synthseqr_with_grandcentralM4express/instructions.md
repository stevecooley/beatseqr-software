# Synthseqr Firmware — User Instructions

Hardware: Adafruit Grand Central M4 Express
Firmware version: 2.3

---

## Overview

Synthseqr is a 16-step MIDI sequencer with 16 patterns, 16 voice sliders, a D-pad for navigation, and an LCD display. It outputs MIDI notes over USB and can follow an external MIDI clock.

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
| Pattern select buttons (4) | Function keys (behavior depends on mode) |
| Pattern select LEDs (4) | Mode indicator |

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
| 1 | Pattern (wraps; 1–4 in Simple, 1–16 in Advanced) | Line 1 — pattern digit |
| 2 | Tempo ±10 BPM | Line 1 — tempo hundreds/tens |
| 3 | Tempo ±1 BPM | Line 1 — tempo units |
| 4 | Tempo ±0.1 BPM | Line 1 — tempo tenths |
| 5 | Tempo ±0.01 BPM | Line 1 — tempo hundredths |
| 6 | Swing (0–5) | Line 2 — swing digit |
| 7 | Clock source (INT / EXT) | Line 2 — int/ext value |
| 8 | MIDI channel (1–16) | Line 2 — channel digits |

**Tempo range:** 10–250 BPM
**MIDI channel range:** 1–16
**Swing:** 0 = straight, 1–2 = mild, 3 = heavy (classic triplet feel), 5 = maximum shuffle

> **Note:** When swing is active, the MIDI clock output (0xF8) also swings. If you are syncing an external device to the sequencer's MIDI clock, set swing to 0.

### LCD Line 1

```
[play/stop] P{pattern} T{tempo}
```

Example: `▶ P01 T120.00  `

### LCD Line 2

```
s{swing} clk:{int|ext} Ch{channel}
```

Example: `s0 clk:int Ch02 `

Line 2 shows swing, clock source, and MIDI channel together. The cursor sits on the active value as you navigate modes 6, 7, and 8.

---

## Clock Source (INT / EXT)

Navigate to **timing mode 7** (D-pad right six times from default).

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

## Modes: Simple and Advanced

Synthseqr has two modes, switchable from the Config Menu.

### Simple Mode (default)

- 4 patterns (P01–P04)
- Pattern buttons 1–4 directly select patterns 1–4
- Pattern button 1 + 4 simultaneously toggle **chain mode** (4 patterns looping)
- Hold any pattern button 2 seconds to begin **pattern copy**; press destination button to complete

### Advanced Mode

- 16 patterns (P01–P16)
- Pattern buttons are **function keys** — their LEDs only light while held
- **Hold pattern button 1** + tap a step button → jump to that pattern (step 1 = P01, step 16 = P16)
- **Double-click pattern button 1** (two taps within ~400 ms) → arm **pattern copy**
  - LCD shows `Copy P{n} ->`
  - Tap any step button to set the destination pattern
  - D-pad left cancels
- D-pad mode 1 navigates patterns 1–16 with up/down

---

## Patterns

There are up to 16 patterns (P01–P16). Each pattern has its own 16 steps and its own set of 16 pitches.

### Selecting a Pattern

**Simple mode**: Press any of the 4 pattern select buttons to switch instantly.

**Advanced mode**: Hold pattern button 1, then tap a step button (step 1 = P01 ... step 16 = P16).

**Either mode**: Navigate to D-pad mode 1 and use up/down to scroll through patterns.

- Step LEDs update to show the new pattern's data.
- Slider pitches are restored from the pattern's saved values (see below).

### Pattern Memory — Slider Pickup

When you switch patterns, the stored pitches for the new pattern are restored. Sliders do **not** immediately override them.

Each slider is individually locked until its physical position reaches within 1 note of the stored pitch for that step — at that point it "picks up" and tracks normally. This prevents unwanted pitch jumps when switching patterns with sliders in different positions.

### Copying a Pattern

**Simple mode**:
1. Hold a **pattern select button for 2 seconds**. The LCD shows `Copy N ->`.
2. Press the **destination pattern button**. The entire pattern (steps + pitches) is copied there.

**Advanced mode**:
1. **Double-click pattern button 1** (two taps within ~400 ms). The LCD shows `Copy P{n} ->`.
2. Tap any **step button** to select the destination (step 1 = P01 ... step 16 = P16).
3. To cancel, press **D-pad left**.

### Chaining Patterns (Simple Mode)

Press **pattern buttons 1 and 4 simultaneously** to toggle chain mode.

- **Chain on** (`chain 4` on LCD): patterns auto-advance 1 → 2 → 3 → 4 → 1 ... each time step 15 is reached.
- **Chain off** (`single` on LCD): only the active pattern plays on loop.
- The LCD briefly shows the new mode as confirmation.

---

## Octave Shift

Open the **Config Menu** (double-tap Enter), scroll to **Octave shift**, and press Enter.

- D-pad up/down shifts all notes up or down by one octave at a time.
- Range: −5 to +5 octaves.
- The shift is applied at MIDI send time — stored pitches are not changed.
- Press Enter or D-pad left to exit the editor and return to the menu.
- The shift is saved along with other settings.
- The label shows `Octave shift *` in the menu when the value is non-zero.

---

## Note Shift

Open the **Config Menu** (double-tap Enter), scroll to **Note shift**, and press Enter.

- D-pad up/down shifts all notes up or down by one semitone at a time.
- Range: −12 to +12 semitones.
- Applied at MIDI send time on top of Octave shift — stored pitches are not changed.
- Press Enter or D-pad left to exit the editor and return to the menu.
- The shift is saved along with other settings.
- The label shows `Note shift   *` in the menu when the value is non-zero.

---

## Clearing Patterns

These combos work while the sequencer is stopped or playing.

| Action | Combo |
|---|---|
| Clear current pattern | Hold **step button 1** + **step button 16** |
| Clear all patterns | Hold **step button 1** + **step button 12** |

Clearing turns off all step LEDs and silences all steps. Slider pitches are not affected.

---

## MIDI Output

- **Channel:** selectable 1–16 (default: channel 2)
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

Example: `▶ P01 T120.00  `

### Line 2

```
s[swing] clk:[int|ext] Ch[channel]
```

Example: `s0 clk:int Ch02 `

The cursor blinks on the field that D-pad up/down currently adjusts.

### Enter Button

- **Single tap** — return to the main display from any temporary LCD screen, toggle the Enter LED indicator.
- **Double-tap** (two taps within ~400 ms) — open the **Config Menu**.

---

## Config Menu

Double-tap the **Enter button** to open the config menu. The sequencer keeps playing while the menu is open.

**Navigation**: D-pad up/down scrolls through items. Line 1 shows the selected item, line 2 shows the next item as a preview.

**Exit**: D-pad left exits from anywhere. When `> Exit` is selected, Enter or D-pad right also exit.

**Menu items:**

| Item | Action |
|---|---|
| Exit | Leave the config menu |
| Save | Save to SD card (primary) + EEPROM (backup); sequencer must be stopped |
| Clear pattern | Clear current pattern (confirmation required) |
| Clear all pats | Clear all 16 patterns (confirmation required) |
| Reset sliders | Reset all pitches to defaults (confirmation required) |
| Mode: Simple/Advanced | Toggle between Simple and Advanced mode |
| Octave shift | Adjust octave offset ±5; up/down to change, Enter or Left to exit; label shows * when non-zero |
| Note shift | Adjust semitone offset ±12; up/down to change, Enter or Left to exit; label shows * when non-zero |
| Note range | (Coming soon) |
| Note scales | (Coming soon) |

**Confirmation prompt**: for destructive actions, line 2 shows `Entr=ok  Lft=no`. Press Enter to confirm or D-pad left to cancel.

---

## Saving Your Work

Open the config menu (double-tap Enter), scroll to **Save**, and press Enter. The sequencer must be **stopped** first — saving while playing is blocked to prevent timing hiccups.

The following are saved:

- All 16 patterns (step on/off + pitch per step)
- Tempo, swing, MIDI channel
- Active pattern, chain mode on/off, clock source (INT/EXT)
- Octave shift, Note shift
- Simple / Advanced mode

**Primary storage**: SD card (`/synthseqr/autosave.json`). The file is human-readable JSON and can be edited or generated externally with any tool you prefer.

**Fallback**: EEPROM (flash storage on the microcontroller). Used automatically on boot if no SD card or save file is found.

On the next power-up, everything is automatically restored exactly as you left it. On first boot (no save yet), the sequencer starts with factory defaults.

> **SD card tip**: You can create sequences in any external tool (DAW, text editor, custom script) and write them to the SD card as `/synthseqr/autosave.json`. The sequencer will load them on the next boot.

---

## Diagnostics Mode

Hold **D-pad left + D-pad right simultaneously for 1 second** to enter diagnostics mode. The Serial monitor (57600 baud) will print hardware test output.

In diagnostics mode:

- Press any **step button** — its LED toggles and `step N pressed` is printed to Serial.
- Press any **pattern select button** — its LED toggles and `pattern N pressed` is printed.
- Move any **voice slider** — `slider N value` is printed only when the value changes.
- Press **D-pad up** and hold — hold duration in ms is printed continuously while held.
- Press **D-pad down** — prints `dpad down pressed`.
- Press **D-pad left** — prints `dpad left pressed`.
- Press **D-pad right** — prints `dpad right pressed`.
- Press **Enter** — prints `enter pressed`.

**Enter / exit diagnostics:** hold **D-pad left + D-pad right simultaneously for 1 second**, then release. The same combo exits.

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
| Select pattern (simple) | Pattern button 1–4 |
| Select pattern (advanced) | Hold pattern button 1 + tap step button |
| Adjust tempo (coarse) | D-pad right to mode 2–3, then up/down |
| Adjust tempo (fine) | D-pad right to mode 4–5, then up/down |
| Adjust swing | D-pad right to mode 6, then up/down |
| Set clock source | D-pad right to mode 7, up=EXT / down=INT |
| Set MIDI channel | D-pad right to mode 8, up/down = channel 1–16 |
| Copy pattern (simple) | Hold pattern button 2s → press destination button |
| Copy pattern (advanced) | Double-click pattern button 1 → tap step = destination |
| Cancel copy (advanced) | D-pad left while copy is armed |
| Chain 4 patterns (simple) | Pattern buttons 1 + 4 simultaneously |
| Clear current pattern | Hold step 1 + step 16 |
| Clear all patterns | Hold step 1 + step 12 |
| Octave shift | Config menu → Octave shift, up/down |
| Note shift | Config menu → Note shift, up/down |
| Save | Config menu → Save (sequencer stopped) |
| Toggle Simple/Advanced | Config menu → Mode |
| Open config menu | Double-tap Enter |
| Enter / exit diagnostics | Hold D-pad left + right 1s |
