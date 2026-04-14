# Synthseqr Firmware — User Instructions

Hardware: Adafruit Grand Central M4 Express
Firmware version: 2.5

---

## Overview

Synthseqr is a 16-step MIDI sequencer with 16 patterns, 16 voice sliders, a D-pad for navigation, and an LCD display. It outputs MIDI notes over USB and can follow an external MIDI clock. Each step has its own pitch, velocity, and gate length — all editable with the voice sliders.

---

## Controls at a Glance

| Control | Location |
|---|---|
| Step buttons (16) | Main row — toggle steps on/off |
| Step LEDs (16) | Above each step button |
| Voice sliders (16) | One per step — set pitch, velocity, or gate depending on mode |
| Play button | Transport — start/stop |
| D-pad (up/down/left/right) | Navigation |
| Enter button | Cycle slider mode (simple mode) / open config menu (double-tap) |
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
- The sequencer fires a MIDI note-on when it arrives at an active step. The note rings for the number of steps set by that step's **gate length**, then a note-off is sent.

### Voice Sliders

The 16 sliders control different data depending on the active **slider mode**. See the **Slider Modes** section below.

---

## Slider Modes

Each step has three editable values: **pitch**, **velocity**, and **gate length**. The voice sliders edit one type at a time.

| Mode | LCD indicator | What sliders control |
|------|--------------|----------------------|
| **NN** (note number) | `♪N` at top-right of line 1 | MIDI pitch for each step |
| **VL** (velocity) | `♪V` at top-right of line 1 | MIDI velocity (1–127) for each step |
| **GT** (gate) | `♪G` at top-right of line 1 | Gate length (1–8 steps) for each step |

The current mode is always shown in the top-right corner of LCD line 1.

### Switching Modes

**Simple mode:** Single-tap the **Enter button** to cycle NN → VL → GT → NN. The mode change fires ~400 ms after the tap so the sequencer can confirm it isn't the start of a double-tap.

**Advanced mode:** Use the **pattern select buttons**:
- **Pattern button 1** → NN mode (LED 1 lights up)
- **Pattern button 2** → GT mode (LED 2 lights up)
- **Pattern button 3** → VL mode (LED 3 lights up)

The lit LED tells you which slider mode is currently active.

### Slider Pickup Guard

When you **switch slider modes**, all 16 sliders are temporarily locked — they won't write any data until each physical slider moves to the position matching what's already stored for that step in the new mode. This prevents accidental overwrites when sliders are at different physical positions for each mode.

The same lock also applies when **switching patterns**: sliders in NN mode won't overwrite the new pattern's stored pitches until the physical position meets the stored value.

### Note Number (NN) Mode

- Each slider maps to a MIDI note within the configured note range (default: notes 36–51).
- Pitches are saved per step, per pattern. Moving a slider immediately updates the pitch for that step.

### Velocity (VL) Mode

- Each slider maps to a MIDI velocity: 1 (softest) to 127 (loudest).
- Velocity is saved per step, per pattern. Default is 127 for all steps.

### Gate (GT) Mode

- Each slider maps to a gate length: 1 to 8 steps.
- **Gate 1** = note off at the very next step (classic one-step behavior).
- **Gate 4** = note rings through 4 steps before note-off fires.
- **Gate 8** = note rings for a full half-bar (8 steps).
- Gates longer than 16 steps wrap around.

---

## Tempo

The D-pad navigates through 5 timing modes in visual left-to-right order. Press **D-pad left/right** to move between them. Press **D-pad up/down** to adjust the selected value.

The cursor on the LCD blinks on the field that up/down currently controls.

| Mode (D-pad left/right) | Up/Down adjusts | LCD location |
|---|---|---|
| 1 | Pattern (wraps; 1–4 in Simple, 1–16 in Advanced) | Line 1 — pattern digit |
| 2 | Tempo ±10 BPM | Line 1 — tempo hundreds/tens |
| 3 | Tempo ±1 BPM | Line 1 — tempo units |
| 4 | Tempo ±0.1 BPM | Line 1 — tempo tenths |
| 5 | Tempo ±0.01 BPM | Line 1 — tempo hundredths |

**Tempo range:** 10–250 BPM

Swing, MIDI channel, and clock source are all in the **Config Menu** — double-tap Enter to open it.

> **Note:** When swing is active, the MIDI clock output (0xF8) also swings. If you are syncing an external device to the sequencer's MIDI clock, set swing to 0.

---

## LCD Display

### Line 1

```
[play/stop] P[pattern] T[tempo]  [mode]
```

Example: `▶ P01 T120.00 ♪N`

- The cursor blinks on the field that D-pad up/down currently adjusts.
- The two-character indicator at the far right shows the active slider mode.

### Line 2 — Step Trigger Display

Line 2 updates live each time a step fires, showing the data for the most recently triggered step:

```
>[step]  ♪[pitch]  ♩[velocity]  G[gate]
```

Example: `>03 ♪045 ♩127 G4`

- `>03` — step 3 just fired
- `♪045` — MIDI note 45
- `♩127` — velocity 127
- `G4` — gate length 4 steps

This display makes it easy to see exactly what each step is playing in real time, regardless of which slider mode is active.

---

## Clock Source

Open the **Config Menu** (double-tap Enter) and scroll to **Clock: int/ext**. Press Enter to toggle between INT and EXT.

- **INT**: the sequencer uses its own internal hardware timer at the current BPM.
- **EXT**: the sequencer follows incoming USB-MIDI clock (0xF8 / 0xFA / 0xFC from an external device such as a DAW or drum machine).

### External Clock Mode

When EXT is active:

- The sequencer advances only when it receives 0xF8 clock bytes from the host.
- 0xFA (MIDI Start) from the host starts the sequencer.
- 0xFC (MIDI Stop) from the host stops it.
- The sequencer does **not** send its own MIDI clock, start, or stop messages — it is purely a follower.
- The local Play button still works to arm or start the sequencer before the host sends start.
- MIDI notes are still output normally.
- **Swing works in EXT mode**: odd-step transitions are deferred by the swing amount relative to the measured clock interval. The effect is slightly milder than internal swing at the same setting.

---

## Swing

Open the **Config Menu** (double-tap Enter) and scroll to **Swing**. Press Enter to enter editing, then use D-pad up/down to adjust. Press Enter or D-pad left to exit.

- **0** = straight timing
- **1–2** = mild swing
- **3** = classic triplet feel (2:1 ratio)
- **5** = maximum shuffle

Swing is applied in both internal and external clock modes.

---

## MIDI Channel

Open the **Config Menu** (double-tap Enter) and scroll to **Channel**. Press Enter to enter editing, then use D-pad up/down to select channel 1–16. Press Enter or D-pad left to exit.

---

## Modes: Simple and Advanced

Synthseqr has two modes, switchable from the Config Menu.

### Simple Mode (default)

- 4 patterns (P01–P04)
- Pattern buttons 1–4 directly select patterns 1–4
- Pattern button 1 + 4 simultaneously toggle **chain mode** (4 patterns looping)
- Hold any pattern button 2 seconds to begin **pattern copy**; press destination button to complete
- **Enter button** single-tap cycles slider mode: NN → VL → GT → NN

### Advanced Mode

- 16 patterns (P01–P16)
- Pattern buttons are **function keys**:
  - **Pattern button 1**: single-click → toggle pattern-nav mode; double-click → 2-phase pattern copy
  - **Pattern button 2**: slider mode → **NN** (LED 1 lights up)
  - **Pattern button 3**: slider mode → **GT** (LED 2 lights up)
  - **Pattern button 4**: slider mode → **VL** (LED 3 lights up)
- **Single-click pattern button 1** (tap, then wait ~400 ms) → toggle **pattern-nav mode**
  - Pattern button 1 LED stays lit while nav mode is active
  - Step LEDs show pattern selection: the currently playing pattern blinks; all chain patterns are solid
  - **Tap a step button** → jump to that pattern (step 1 = P01 ... step 16 = P16); clears any active chain
  - **Hold a step button, then tap a second step button** → define a chain range (first held = start, second tap = end)
    - Wrap-around chains are supported: e.g. hold step 8, tap step 3 → plays P08, P09, ..., P16, P01, P02, P03
  - **Single-click pattern button 1 again** → exit nav mode; step LEDs return to normal step display
- **Double-click pattern button 1** (two taps within ~400 ms) → **pattern copy** (2 phases)
  - Phase 1: LCD shows `copy which pat?` → tap a step button to select the **source** pattern
  - Phase 2: LCD shows `Copy N->where?` → tap a step button to select the **destination** pattern
  - LCD briefly shows `Copied X to Y`, then returns to main display
  - **D-pad left** cancels at any point
- D-pad mode 1 navigates patterns 1–16 with up/down
- Enter button has no slider mode function in advanced mode — use pattern buttons 2/3/4 instead

---

## Patterns

There are up to 16 patterns (P01–P16). Each pattern has its own 16 steps with individual pitch, velocity, and gate length per step.

### Selecting a Pattern

**Simple mode**: Press any of the 4 pattern select buttons to switch instantly.

**Advanced mode**: Single-click pattern button 1 to enter nav mode, then tap a step button (step 1 = P01 ... step 16 = P16). Single-click pattern button 1 again to exit nav mode.

**Either mode**: Navigate to D-pad mode 1 and use up/down to scroll through patterns.

- Step LEDs update to show the new pattern's data.
- Slider values are restored from the pattern's saved values; all sliders are locked until each physical position crosses its stored value (see **Slider Pickup Guard** above).

### Copying a Pattern

Copies include steps, pitches, velocities, and gate lengths.

**Simple mode**:
1. Hold a **pattern select button for 2 seconds**. The LCD shows `Copy N ->`.
2. Press the **destination pattern button**. The entire pattern is copied there.

**Advanced mode**:
1. **Double-click pattern button 1** (two taps within ~400 ms). The LCD shows `copy which pat?`.
2. Tap a **step button** to select the **source** pattern (step 1 = P01 ... step 16 = P16). The LCD shows `Copy N->where?`.
3. Tap a **step button** to select the **destination** pattern. The LCD briefly shows `Copied X to Y`.
4. To cancel at any point, press **D-pad left**.

### Chaining Patterns

**Simple mode**: Press **pattern buttons 1 and 4 simultaneously** to toggle chain mode.

- **Chain on** (`chain 4` on LCD): patterns auto-advance 1 → 2 → 3 → 4 → 1 ... each time step 16 is reached.
- **Chain off** (`single` on LCD): only the active pattern plays on loop.
- The LCD briefly shows the new mode as confirmation.

**Advanced mode (pattern-nav mode)**: Enter nav mode (single-click pattern button 1), then hold a step button and tap a second step button to define the chain range. The first button held is the chain start, the second tap is the chain end. Wrap-around is supported — e.g. hold step 8, tap step 3 → plays P08, P09, ..., P16, P01, P02, P03. Tap a single step (no hold) to return to single-pattern playback.

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

## Note Range

Open the **Config Menu** (double-tap Enter), scroll to **Note range**, and press Enter.

The editor has two phases:

1. **Edit Low** — line 2 shows `Edit Lo: 36`. D-pad up/down adjusts the low note (0–126, must stay below high). Press Enter to move to the high editor.
2. **Edit High** — line 2 shows `Edit Hi: 52`. D-pad up/down adjusts the high note (low+1–127). Press Enter to confirm and exit.

D-pad left exits either phase immediately, keeping whatever values were set. The label shows `Note range   *` in the menu when the values differ from the defaults (36/52).

The range determines how the sliders map to MIDI notes in NN mode: the full travel of each slider covers the note range from low to high.

---

## Pattern Length

Open the **Config Menu** (double-tap Enter), scroll to **Pat length**, and press Enter.

- D-pad up/down adjusts the length from 1 to 16 steps.
- **Tap any step button** to set the length directly: tapping step N sets the length to N (step 1 = length 1, step 16 = length 16). Step LEDs light up to show the current length while editing.
- Press Enter or D-pad left to exit.
- The sequencer always loops back to step 1 after the last active step, and chains advance at the end of the active length.
- The label shows `Pat length   *` when the value is not 16.
- Pattern length is global — it applies to all patterns simultaneously.

---

## Pattern Direction

Open the **Config Menu** (double-tap Enter), scroll to **Pat dir**, and press Enter.

D-pad up/down cycles through 8 directions. Press Enter or D-pad left to exit.

| Direction | Display | Sequence (example, length 8) |
|---|---|---|
| **Fwd** | `Fwd` | 0, 1, 2, 3, 4, 5, 6, 7 |
| **Rev** | `Rev` | 7, 6, 5, 4, 3, 2, 1, 0 |
| **Pong** | `Pong` | 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, … |
| **Rand** | `Rand` | random step each tick (repeats allowed) |
| **Shuf** | `Shuf` | random permutation — every step once before reshuffling |
| **E/O** | `E/O` | all even-indexed steps then all odd: 0, 2, 4, 6, 1, 3, 5, 7 |
| **In** | `In` | outside-in: 0, 7, 1, 6, 2, 5, 3, 4 |
| **Quad** | `Quad` | Q1, Q3, Q2, Q4 — quarters reordered: 0,1, 4,5, 2,3, 6,7 |

Pattern direction is global — it applies to all patterns simultaneously. Gate lengths are measured in hardware clock steps regardless of direction, so gate behavior is consistent across all modes.

---

## Clearing Patterns

These combos work while the sequencer is stopped or playing.

| Action | Combo |
|---|---|
| Clear current pattern | Hold **step button 1** + **step button 16** |
| Clear all patterns | Hold **step button 1** + **step button 12** |

Clearing turns off all step LEDs, silences all steps, resets velocities to 127, and resets gate lengths to 1.

---

## MIDI Output

- **Channel:** selectable 1–16 via Config Menu (default: channel 2)
- **Note-on velocity:** per-step, set in VL slider mode (default: 127 for all steps)
- **Note-off:** sent after the gate length expires (1–8 steps after note-on), and for all open notes on stop
- **MIDI clock (0xF8):** output continuously while playing (internal mode only)
- **MIDI Start (0xFA) / Stop (0xFC):** output on play/stop (internal mode only)

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
| Reset sliders | Reset all pitches, velocities, and gates to defaults (confirmation required) |
| Clock: int/ext | Toggle internal clock / external USB-MIDI clock |
| Channel | MIDI output channel 1–16; up/down to change, Enter or Left to exit |
| Swing | Swing amount 0–5; up/down to change, Enter or Left to exit |
| Mode: Simple/Advanced | Toggle between Simple and Advanced mode |
| Octave shift | Adjust octave offset ±5; up/down to change, Enter or Left to exit; label shows * when non-zero |
| Note shift | Adjust semitone offset ±12; up/down to change, Enter or Left to exit; label shows * when non-zero |
| Note range | Two-phase editor: Enter to edit low note, Enter again to edit high note, Enter to exit; Left exits either phase; label shows * when non-default (36/52) |
| Note scales | (Coming soon) |
| Pat length | Step count 1–16; up/down or tap a step button; step LEDs show current length; label shows * when not 16 |
| Pat dir | Playback direction; up/down cycles Fwd/Rev/Pong/Rand/Shuf/E/O/In/Quad |

**Confirmation prompt**: for destructive actions, line 2 shows `Entr=ok  Lft=no`. Press Enter to confirm or D-pad left to cancel.

---

## Saving Your Work

Open the config menu (double-tap Enter), scroll to **Save**, and press Enter. The sequencer must be **stopped** first — saving while playing is blocked to prevent timing hiccups.

The following are saved:

- All 16 patterns (step on/off, pitch, velocity, and gate length per step)
- Tempo, swing, MIDI channel
- Active pattern, chain mode on/off, clock source (INT/EXT)
- Octave shift, Note shift, Note range (low/high)
- Simple / Advanced mode
- Pattern length and Pattern direction

**Primary storage**: SD card (`/synthseqr/autosave.json`). The file is human-readable JSON and can be edited or generated externally with any tool you prefer.

**Fallback**: EEPROM (flash storage on the microcontroller). Used automatically on boot if no SD card or save file is found. Note: velocity and gate data are only saved to SD — on an EEPROM-only boot they reset to defaults (127 / 1).

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
| Set step pitch | Voice slider (NN mode) |
| Set step velocity | Voice slider (VL mode) |
| Set step gate length | Voice slider (GT mode) |
| Cycle slider mode (simple) | Enter button |
| Slider mode → NN (advanced) | Pattern button 2 |
| Slider mode → GT (advanced) | Pattern button 3 |
| Slider mode → VL (advanced) | Pattern button 4 |
| Select pattern (d-pad) | D-pad left to mode 1, up/down cycles patterns |
| Select pattern (simple) | Pattern button 1–4 |
| Select pattern (advanced) | Single-click pattern button 1 (nav mode on), tap step button |
| Adjust tempo (coarse) | D-pad right to mode 2–3, then up/down |
| Adjust tempo (fine) | D-pad right to mode 4–5, then up/down |
| Adjust swing | Config menu → Swing |
| Set clock source | Config menu → Clock |
| Set MIDI channel | Config menu → Channel |
| Copy pattern (simple) | Hold pattern button 2s → press destination button |
| Copy pattern (advanced) | Double-click pattern button 1 → tap step = source → tap step = destination |
| Cancel copy (advanced) | D-pad left (works at either phase) |
| Chain 4 patterns (simple) | Pattern buttons 1 + 4 simultaneously |
| Define chain (advanced) | Nav mode on → hold step (start) + tap step (end) |
| Clear current pattern | Hold step 1 + step 16 |
| Clear all patterns | Hold step 1 + step 12 |
| Octave shift | Config menu → Octave shift, up/down |
| Note shift | Config menu → Note shift, up/down |
| Note range | Config menu → Note range, Enter for lo/hi phases |
| Set pattern length | Config menu → Pat length, up/down or tap step button |
| Set pattern direction | Config menu → Pat dir, up/down cycles 8 modes |
| Save | Config menu → Save (sequencer stopped) |
| Toggle Simple/Advanced | Config menu → Mode |
| Open config menu | Double-tap Enter |
| Enter / exit diagnostics | Hold D-pad left + right 1s |
