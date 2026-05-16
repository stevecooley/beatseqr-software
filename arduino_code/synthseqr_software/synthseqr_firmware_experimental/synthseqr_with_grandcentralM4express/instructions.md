# Synthseqr Firmware — User Instructions

Hardware: Adafruit Grand Central M4 Express
Firmware version: 2.5

---

## Overview

Synthseqr is a 16-step MIDI sequencer with 16 patterns, 16 voice sliders, a D-pad for navigation, and an LCD display. It outputs MIDI notes and MIDI CC messages over USB and can follow an external MIDI clock. Each step has its own pitch, velocity, gate length, and CC value — all editable with the voice sliders.

---

## Controls at a Glance

| Control                    | Location                                                        |
| -------------------------- | --------------------------------------------------------------- |
| Step buttons (16)          | Main row — toggle steps on/off                                  |
| Step LEDs (16)             | Above each step button                                          |
| Voice sliders (16)         | One per step — set pitch, velocity, or gate depending on mode   |
| Play button                | Transport — start/stop                                          |
| D-pad (up/down/left/right) | Navigation                                                      |
| Enter button               | Cycle slider mode (simple mode) / open config menu (double-tap) |
| Pattern select buttons (4) | Function keys (behavior depends on mode)                        |
| Pattern select LEDs (4)    | Mode indicator                                                  |

---

## Playing the Sequencer

### Start / Stop

Press the **Play button** to start. Press it again to stop.

- Chase lights on the step LEDs follow the current playing position.
- The active step counter on line 1 of the LCD shows the most recently fired step; it shows `>--` when stopped.
- Any note held open when you stop is automatically silenced.

### Step Buttons

Press any **step button** to toggle that step on or off.

- LED on = step is active (note will play)
- LED off = step is silent
- The sequencer fires a MIDI note-on when it arrives at an active step. The note rings for the number of steps set by that step's **gate length**, then a note-off is sent.

When **Note Audition** is enabled (Config Menu → Features → Note audition), pressing a step button to **turn a step ON** also immediately sends a MIDI note preview so you can hear what that step will sound like — without needing to start the sequencer. The note plays for one 16th note at the current tempo. After a gate-set gesture, the note replays with the actual gate length just set. See **Note Audition** below.

### Voice Sliders

The 16 sliders control different data depending on the active **slider mode**. See the **Slider Modes** section below.

---

## Slider Modes

Each step has four editable values: **pitch**, **velocity**, **gate length**, and **CC value**. The voice sliders edit one type at a time.

| Mode                    | LCD indicator               | What sliders control                                                |
| ----------------------- | --------------------------- | ------------------------------------------------------------------- |
| **NN** (note number)    | `♪N` at top-right of line 1 | MIDI pitch for each step                                            |
| **VL** (velocity)       | `♪V` at top-right of line 1 | MIDI velocity (1–127) for each step                                 |
| **GT** (gate)           | `♪G` at top-right of line 1 | Gate length (1–8 steps) for each step                               |
| **CC** (control change) | `♪♩` at top-right of line 1 | MIDI CC value (0–127) per step; step buttons toggle CC steps on/off |
| **PR** (probability)    | `♪P` at top-right of line 1 | Fire probability (0–100%) for each step                             |

The current mode is always shown in the top-right corner of LCD line 1.

### Switching Modes

**Simple mode:** Single-tap the **Enter button** to cycle NN → VL → GT → CC → PR → NN. The mode change fires ~400 ms after the tap so the sequencer can confirm it isn't the start of a double-tap.

**PR mode**, in "Advanced Mode", is accessed via the **Config Menu** (double-tap Enter) → **Step prob**. This exits the menu and activates PR mode. 

**Advanced mode:** Use the **pattern select buttons**:

- **Pattern button 1** → NN mode (LED 1 lights up)
- **Pattern button 2** → GT mode (LED 2 lights up)
- **Pattern button 3** → VL mode (LED 3 lights up)
- For CC and PR modes in advanced mode: use the config menu.

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

### CC (Control Change) Mode

In CC mode, the step LEDs and buttons switch to show and control the **CC automation lane** — independent of whether notes are on or off.

- **Step buttons** toggle CC steps on/off (LED on = CC fires at this step; LED off = no CC).
- **Voice sliders** set the **CC value** for each step (0–127). Values are saved per step, per pattern.
- CC steps fire at the same time as note steps — a step can have a CC without a note, or both.
- The CC **controller number** is set per-pattern in the Config Menu (see **CC Number** below). Default is CC 1 (Modulation Wheel).

When you exit CC mode, step LEDs return to showing note step on/off state.

### Probability (PR) Mode

In PR mode each slider controls the **fire probability** for its corresponding step (0–100%). A step at 100% (default) always fires. A step at 50% fires roughly half the time. A step at 0% never fires (silenced without turning it off).

- PR mode does not change which steps are "on" — it layers randomness on top of existing step data.
- CC steps fire independently: a step can have CC enabled and its note probabilistic.
- Probabilities are saved per step, per pattern.
- Clearing a pattern resets all probabilities to 100%.

---

## Tempo

The D-pad navigates through 4 timing modes in visual left-to-right order. Press **D-pad left/right** to move between them. Press **D-pad up/down** to adjust the selected value.

The cursor on the LCD blinks on the field that up/down currently controls.

| Mode (D-pad left/right) | Up/Down adjusts                                  | LCD location                 |
| ----------------------- | ------------------------------------------------ | ---------------------------- |
| 1                       | Pattern (wraps; 1–4 in Simple, 1–16 in Advanced) | Line 1 — pattern digit       |
| 2                       | Tempo ±10 BPM                                    | Line 1 — tempo hundreds/tens |
| 3                       | Tempo ±1 BPM                                     | Line 1 — tempo units         |
| 4                       | Tempo ±0.1 BPM                                   | Line 1 — tempo tenths        |

**Tempo range:** 10–250 BPM

Swing, MIDI channel, and clock source are all in the **Config Menu** — double-tap Enter to open it.

> **Note:** When swing is active, the MIDI clock output (0xF8) also swings. If you are syncing an external device to the sequencer's MIDI clock, set swing to 0.

---

## LCD Display

### Line 1

```
P[pattern] >[step] [tempo] [mode]
```

Example: `P01 >03 120.0 ♪N`

- No play/stop icon — space freed for step counter.
- Pattern is zero-padded to 2 digits (P01–P16).
- `>[step]` shows the most-recently-fired step (01–16). Shows `>--` when stopped or before the first step fires.
- Tempo is zero-padded, 1 decimal place (e.g. `120.0`, `090.5`).
- The cursor blinks on the field that D-pad up/down currently adjusts.
- The two-character indicator at the far right shows the active slider mode.

### Line 2 — Step Trigger Display

Line 2 updates live each time a step fires, showing the data for the most recently triggered step:

```
♪[pitch]  ♩[velocity]  G[gate]  ♩[cc]
```

Example: `♪045 ♩127 G4♩064`

- `♪045` — MIDI note 45
- `♩127` — velocity 127
- `G4` — gate length 4 steps
- `♩064` — CC value 64 (shows `---` if no CC is enabled for this step)

This display makes it easy to see exactly what each step is playing in real time, regardless of which slider mode is active.

---

## Note Audition

Note audition lets you hear what each step sounds like while the sequencer is **stopped** — no need to start playback just to check a note.

**To enable**: Config Menu (double-tap Enter) → **Features** → scroll to **Note audition** → Enter to toggle **active**.

**To disable**: same path, Enter to toggle **inactive**. Disabling immediately silences any note currently sounding from an audition.

### What it does

When note audition is active and the sequencer is stopped:

- **Turn a step ON** (press a step button whose LED is currently off): the step toggles on *and* a MIDI note plays immediately for one 16th note at the current tempo. The note uses that step's current pitch, velocity, and any active octave/note shift and scale — exactly what the sequencer would play.
- **Turn a step OFF** (press a step button whose LED is on): the step toggles off. No note plays.
- **Gate-set gesture** (hold one step button ≥ 150 ms, tap a second step): the gate is set and the note plays again using the newly set gate length — so you hear the full ring time.

### What it doesn't do

- Audition does **not** fire while the sequencer is playing — you can hear the note naturally when the step is reached.
- Audition does **not** apply to CC mode — step buttons in CC mode always toggle `cc_step_enabled` immediately with no note sent.
- Pitch drift is **not** applied to audition notes — the preview is always the stable stored pitch so you hear exactly what is recorded.

### Note

Note audition is **off by default**. Enable it from the Features submenu, then save (Config Menu → Save) if you want the setting to persist across power cycles.

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
- The local Play button still works to arm or start the sequencer.
  - **If the external clock is already running when you press Play**, the sequencer waits for the next beat boundary (the next 6th 0xF8 pulse) before starting, so it locks in phase with the incoming clock automatically.
  - **If the external clock is not running**, pressing Play arms the sequencer; it starts as soon as the first clock pulse arrives (or when the host sends 0xFA).
- MIDI notes are still output normally.
- **Swing works in EXT mode**: odd-step transitions are deferred by the swing amount relative to the measured clock interval. The effect is slightly milder than internal swing at the same setting.

---

## Swing

Open the **Config Menu** (double-tap Enter) and scroll to **Swing**. Press Enter to enter editing, then use D-pad up/down to adjust. Press Enter or D-pad left to exit.

- **0** = straight timing
- **1–2** = mild swing
- **3** = classic triplet feel (2:1 ratio)
- **5** = maximum shuffle

Swing is applied in both internal and external clock modes. If you apply swing while an external clock is already running, you might not hear the swing. Restart the external clock to hear it.

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

- Step LEDs update to show the new pattern's data (note steps in normal mode, CC steps in CC mode).
- Slider values are restored from the pattern's saved values; all sliders are locked until each physical position crosses its stored value (see **Slider Pickup Guard** above).

### Copying a Pattern

Copies include steps, pitches, velocities, gate lengths, CC on/off, CC values, and CC controller number.

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

Open the **Config Menu** (double-tap Enter), scroll to **Note range**, and press Enter to open the Note Range submenu.

**Submenu items** (D-pad up/down to scroll, Enter to select, D-pad left to exit submenu):

| Item          | Low note   | High note  |
| ------------- | ---------- | ---------- |
| **Custom**    | (editable) | (editable) |
| **16 notes**  | 36         | 51         |
| **12 notes**  | 36         | 47         |
| **8 notes**   | 36         | 43         |
| **6 notes**   | 36         | 41         |
| **4 notes**   | 36         | 39         |

Line 1 shows `> ` before the currently selected item. Pressing Enter on a preset applies it immediately and exits the submenu.

**Custom** opens the two-phase editor:

1. **Edit Low** — line 2 shows `Edit Lo: 36`. D-pad up/down adjusts the low note (0–126, must stay below high). Press Enter to move to the high editor.
2. **Edit High** — line 2 shows `Edit Hi: 52`. D-pad up/down adjusts the high note (low+1–127). Press Enter to confirm and exit.

D-pad left exits either phase immediately, keeping whatever values were set.

The label shows `Note range   *` in the menu when the values differ from the defaults (36/52).

The range determines how the sliders map to MIDI notes in NN mode: the full travel of each slider covers the note range from low to high.

---

## Note Scales

Open the **Config Menu** (double-tap Enter), scroll to **Note scales**, and press Enter.

The editor has two phases:

1. **Scale type** — line 2 shows `Sc: Major` (or whichever scale is active). D-pad up/down cycles through the 10 available scales. Press Enter to move to the root note editor.
2. **Root note** — line 2 shows `Root: C#` (or current root). D-pad up/down cycles C → C# → D → D# → E → F → F# → G → G# → A → A# → B. Press Enter to confirm and exit.

D-pad left exits either phase immediately.

**Each change takes effect immediately** — as soon as you move up or down, all stored pitches across all 16 patterns are snapped to the nearest note in the new scale. Sliders are also re-locked until each physical position crosses the new pitch.

The label shows `Note scales  *` in the menu when the scale is not Chromatic/C.

### Available Scales

| Name           | Notes (intervals from root)                                      |
| -------------- | ---------------------------------------------------------------- |
| **Chromatic**  | All 12 semitones — no constraint (default behavior)              |
| **Blues**      | Minor pentatonic + blue note (e.g. A C D D# E G)                |
| **Dorian**     | Like natural minor with raised 6th (e.g. D E F G A B C)         |
| **HarmMinor**  | Natural minor with raised 7th (e.g. A B C D E F G#)             |
| **Major**      | W-W-H-W-W-W-H (e.g. C D E F G A B)                              |
| **Mixolydian** | Like major with lowered 7th (e.g. G A B C D E F)                |
| **NatMinor**   | W-H-W-W-H-W-W (e.g. A B C D E F G)                              |
| **PentMaj**    | Major pentatonic — 5 notes (e.g. C D E G A)                     |
| **PentMin**    | Minor pentatonic — 5 notes (e.g. A C D E G)                     |
| **Phrygian**   | H-W-W-W-H-W-W — dark, Spanish flavor (e.g. E F G A B C D)      |

### How It Works With Sliders

In NN mode, the 16 sliders map evenly across only the in-scale notes within the configured note range. For example, with C Major and a note range of C3–C5, each slider position snaps to a C, D, E, F, G, A, or B — never an out-of-scale note. When you set the scale to Chromatic, sliders return to the full chromatic range.

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

| Direction                | Display | Sequence (example, length 8)                                |
| ------------------------ | ------- | ----------------------------------------------------------- |
| **Foreward**             | `Fwd`   | 0, 1, 2, 3, 4, 5, 6, 7                                      |
| **Reverse**              | `Rev`   | 7, 6, 5, 4, 3, 2, 1, 0                                      |
| **Ping Pong**            | `Pong`  | 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, …                 |
| **Random**               | `Rand`  | random step each tick (repeats allowed)                     |
| **Shuffled Permutation** | `Shuf`  | random permutation — every step once before reshuffling     |
| **Even/Odd**             | `E/O`   | all even-indexed steps then all odd: 0, 2, 4, 6, 1, 3, 5, 7 |
| **Outside In**           | `In`    | outside-in: 0, 7, 1, 6, 2, 5, 3, 4                          |
| **Quadrants**            | `Quad`  | Q1, Q3, Q2, Q4 — quarters reordered: 0,1, 4,5, 2,3, 6,7     |

Pattern direction is global — it applies to all patterns simultaneously. Gate lengths are measured in hardware clock steps regardless of direction, so gate behavior is consistent across all modes.

---

## Clearing Patterns

Clearing turns off all step LEDs, silences all steps, resets pitches to the low note value, resets velocities to 127, resets gate lengths to 1, resets all step probabilities to 100%, and clears CC step data (all CC steps disabled, CC values set to 0).

---

## MIDI Output

- **Channel:** selectable 1–16 via Config Menu (default: channel 2)
- **Note-on velocity:** per-step, set in VL slider mode (default: 127 for all steps)
- **Note-off:** sent after the gate length expires (1–8 steps after note-on), and for all open notes on stop
- **MIDI CC:** sent on each CC-enabled step, using the per-pattern controller number and per-step value
- **MIDI clock (0xF8):** output continuously while playing (internal mode only)
- **MIDI Start (0xFA) / Stop (0xFC):** output on play/stop (internal mode only)

---

## Config Menu

Double-tap the **Enter button** to open the config menu. The sequencer keeps playing while the menu is open.

**Navigation**: D-pad up/down scrolls through items. Line 1 shows the selected item, line 2 shows the next item as a preview.

**Exit**: D-pad left exits from anywhere. When `> Exit` is selected, Enter or D-pad right also exit.

**Menu items:**

| Item                  | Action                                                                                                                                                                                                                    |
| --------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| CC number             | CC controller number for the current pattern (1–119, skipping reserved CCs); up/down cycles; Enter or Left exits; line 2 shows name of selected CC                                                                        |
| Channel               | MIDI output channel 1–16; up/down to change, Enter or Left to exit                                                                                                                                                        |
| Clear/Reset           | Opens the Reset/Clear submenu — Clear all pats, Clear pattern, Reset sliders (all require confirmation)                                                                                                                   |
| Clock: int/ext        | Toggle internal clock / external USB-MIDI clock                                                                                                                                                                           |
| Diagnostics           | Opens the Diagnostics submenu: LED test (sequential LED chase), Input test (button/slider tester), Hi trim (slider high-end calibration offset 0–4)                                                       |
| Exit                  | Leave the config menu                                                                                                                                                                                                     |
| Features              | Enter the Features submenu to toggle individual feature flags on/off                                                                                                                                                      |
| Mode: Simple/Advanced | Toggle between Simple and Advanced mode (confirmation required)                                                                                                                                                           |
| Note range            | Opens the Note Range submenu: select a preset (4/6/8/12/16 notes anchored at 36) or Custom (two-phase lo/hi editor); label shows * when non-default (36/52)                                               |
| Note scales           | Two-phase editor: Enter to choose scale type, Enter again to choose root note, Enter to exit; changing either value immediately quantizes all stored pitches to the nearest in-scale note; label shows * when non-default |
| Note shift            | Adjust semitone offset ±12; up/down to change, Enter or Left to exit; label shows * when non-zero                                                                                                                         |
| Octave shift          | Adjust octave offset ±5; up/down to change, Enter or Left to exit; label shows * when non-zero                                                                                                                            |
| Pat dir               | Playback direction; up/down cycles Fwd/Rev/Pong/Rand/Shuf/E/O/In/Quad                                                                                                                                                     |
| Pat length            | Step count 1–16; up/down or tap a step button; step LEDs show current length; label shows * when not 16                                                                                                                   |
| Pitch drift           | Semitones of random pitch wander applied at note-send time (0=off, 1–7); up/down to adjust; Enter or Left exits; label shows * when non-zero                                                                              |
| Save                  | Save to SD card (primary) + EEPROM (backup); sequencer must be stopped                                                                                                                                                    |
| Step prob             | Exits the menu immediately and activates PR slider mode — sliders set per-step fire probability 0–100%; label shows * when any step is below 100%                                                                         |
| Swing                 | Swing amount 0–5; up/down to change, Enter or Left to exit                                                                                                                                                                |
| Tempo                 | BPM editor (hidden when external clock is on); Enter starts editing; Enter again cycles resolution ±10/±1/±0.1 BPM; up/down adjusts; Left exits                                                                          |

**Confirmation prompt**: for destructive actions, line 2 shows `Entr=ok  Lft=no`. Press Enter to confirm or D-pad left to cancel.

---

## Pitch Drift

Open the **Config Menu** (double-tap Enter) and scroll to **Pitch drift**. Press Enter to enter editing.

- D-pad up/down adjusts the drift amount from 0 to 7 semitones.
- **0** = no drift (deterministic pitch, default).
- At each note-on, a random offset in the range `[-drift, +drift]` is added to the note's pitch.
- The result is clamped to the configured note range (low/high) so drift never exceeds the range.
- If a note scale is active, the drifted pitch is then snapped to the nearest in-scale note.
- Drift applies at send time — stored pitches are not modified.
- Press Enter or D-pad left to exit.
- The label shows `Pitch drift  *` when the value is non-zero.

**Musical uses**: at low values (1–2) it adds subtle pitch variation like a slightly unstable oscillator. Higher values (4–7) create wide random leaps, useful for generative melodic content.

---

## CC Number

Open the **Config Menu** (double-tap Enter) and scroll to **CC number**. Press Enter to enter editing.

- D-pad up/down cycles through valid CC controller numbers (1–119).
- Reserved CC numbers are skipped automatically: CC 32 (Bank LSB) and CC 96–101 (RPN/NRPN data entry).
- Line 2 shows the full LCD name of the selected CC while editing.
- Press Enter or D-pad left to exit.
- **CC number is per-pattern** — each of the 16 patterns can output a different CC.
- Default: CC 1 (Modulation Wheel) for all patterns.

The CC controller number determines what parameter the CC automation lane controls on your synth or DAW. For example: CC 1 = Mod Wheel, CC 7 = Volume, CC 10 = Pan, CC 74 = Filter Cutoff (on many synths).

See the **MIDI CC Number Reference** table at the end of this document for a full list.

---

## Saving Your Work

Open the config menu (double-tap Enter), scroll to **Save**, and press Enter. The sequencer must be **stopped** first — saving while playing is blocked to prevent timing hiccups.

The following are saved:

- All 16 patterns (step on/off, pitch, velocity, gate length, CC on/off, CC value, CC controller number, and step probability per pattern)
- Pitch drift (global)
- Tempo, swing, MIDI channel
- Active pattern, chain mode on/off, clock source (INT/EXT)
- Octave shift, Note shift, Note range (low/high)
- Note scale (type and root note)
- Simple / Advanced mode
- Pattern length and Pattern direction

**Primary storage**: SD card (`/synthseqr/autosave.json`). The file is human-readable JSON and can be edited or generated externally with any tool you prefer.

**Fallback**: EEPROM (flash storage on the microcontroller). Used automatically on boot if no SD card or save file is found. Note: velocity and gate data are only saved to SD — on an EEPROM-only boot they reset to defaults (127 / 1).

On the next power-up, everything is automatically restored exactly as you left it. On first boot (no save yet), the sequencer starts with factory defaults.

> **SD card tip**: You can create sequences in any external tool (DAW, text editor, custom script) and write them to the SD card as `/synthseqr/autosave.json`. The sequencer will load them on the next boot.

---

## Diagnostics Mode

Open the **Config Menu** (double-tap Enter), scroll to **Diagnostics**, and press Enter to open the Diagnostics submenu. All normal sequencer functions are suspended while any diagnostics mode is active. Press **D-pad left** to exit any diagnostics mode and return to the submenu; press D-pad left again to return to the Config Menu.

**Diagnostics submenu items** (D-pad up/down to scroll, Enter to select):

| Item           | What it does                                                                   |
| -------------- | ------------------------------------------------------------------------------ |
| **LED test**   | Non-blocking sequential chase through all 22 LEDs, 80 ms per LED              |
| **Input test** | Interactive button and slider tester — LCD shows raw values as you press/move  |
| **Hi trim**    | Slider high-end calibration offset (0–4), applied at MIDI send time in NN mode |

### LED Test

Press Enter on **LED test** to start. The sequencer cycles through all 22 LEDs in order (step LEDs 1–16, pattern select LEDs 1–4, Play LED, Enter LED), lighting one at a time at 80 ms per step, looping continuously.

Press **D-pad left** to stop and return to the Diagnostics submenu.

### Input Test

Press Enter on **Input test** to start. The LCD shows an idle prompt:

```
  INPUT TEST    
Lft=back PAT1:sf
```

Press any button. **Line 1** of the LCD immediately shows the button name and the GPIO pin it is wired to:

```
STEP01   pin: 23
DPAD-UP  pin: 19
PLAY     pin: 21
PAT1     pin: 15
ENTER    pin: 20
```

Format: `<name (8 chars)> pin:<pin number>`

Button names: `STEP01`–`STEP16`, `DPAD-UP`, `DPAD-DN`, `ENTER`, `PLAY`, `PAT1`–`PAT4`.

Step buttons and pattern select buttons also toggle their LED as a secondary visual confirmation.

Move any voice slider. **Line 2** of the LCD shows the slider index, its analog pin, and the raw 12-bit ADC value (0–4095):

```
SL00 A15 r:4095 
SL09 A6  r:2048 
```

Format: `SL<index> <pin> r:<raw value>`

The display updates only when the value changes by more than 16 ADC counts. Only one slider update appears per 100 ms to keep the LCD readable.

A full-travel slider should smoothly sweep from near 0 to near 4095. A dead or noisy slider will show erratic values, a very narrow range, or no movement at all.

After **2 seconds** of no activity, both LCD lines return to the idle prompt.

**Save-file viewer**: Press **PAT1** (pattern button 1) to view the saved JSON fields from the SD card. D-pad up/down scrolls through fields. Press any other button to return to the idle prompt.

**Exit**: Press **D-pad left** to return to the Diagnostics submenu. Double-tap **Enter** also exits.

### Hi Trim

Press Enter on **Hi trim** to enter the calibration offset editor.

- D-pad up/down adjusts the trim from 0 to 4.
- The offset is added on top of the configured note range high value when sliders map to pitches in NN mode. This compensates for physical faders that lose resistance near the top of travel and plateau before reaching true full travel.
- Press Enter or D-pad left to exit.
- Save the setting via Config Menu → Save to persist it across power cycles.

---

## Serial Monitor

Connect at **57600 baud** to see:

- Tempo, shuffle, beat length, and position on every up/down D-pad press
- Clock mode changes (`clock mode: INT` / `clock mode: EXT`)
- MIDI start/stop events
- All LCD commands mirrored as text

---

## Quick Reference Card

| Action                      | How                                                                        |
| --------------------------- | -------------------------------------------------------------------------- |
| Start / stop                | Play button                                                                |
| Toggle a step               | Step button                                                                |
| Set step pitch              | Voice slider (NN mode)                                                     |
| Set step velocity           | Voice slider (VL mode)                                                     |
| Set step gate length        | Voice slider (GT mode)                                                     |
| Set step CC value           | Voice slider (CC mode)                                                     |
| Toggle CC step on/off       | Step button (CC mode)                                                      |
| Set step probability        | Voice slider (PR mode)                                                     |
| Enter PR mode               | Config menu → Step prob                                                    |
| Set pitch drift             | Config menu → Pitch drift, up/down                                         |
| Enable note audition        | Config menu → Features → Note audition → Enter                             |
| Cycle slider mode (simple)  | Enter button                                                               |
| Slider mode → NN (advanced) | Pattern button 2                                                           |
| Slider mode → GT (advanced) | Pattern button 3                                                           |
| Slider mode → VL (advanced) | Pattern button 4                                                           |
| Set CC controller number    | Config menu → CC number                                                    |
| Select pattern (d-pad)      | D-pad left to mode 1, up/down cycles patterns                              |
| Select pattern (simple)     | Pattern button 1–4                                                         |
| Select pattern (advanced)   | Single-click pattern button 1 (nav mode on), tap step button               |
| Adjust tempo (coarse)       | D-pad right to mode 2–3, then up/down                                      |
| Adjust tempo (fine)         | D-pad right to mode 4, then up/down                                        |
| Adjust swing                | Config menu → Swing                                                        |
| Set clock source            | Config menu → Clock                                                        |
| Set MIDI channel            | Config menu → Channel                                                      |
| Copy pattern (simple)       | Hold pattern button 2s → press destination button                          |
| Copy pattern (advanced)     | Double-click pattern button 1 → tap step = source → tap step = destination |
| Cancel copy (advanced)      | D-pad left (works at either phase)                                         |
| Chain 4 patterns (simple)   | Pattern buttons 1 + 4 simultaneously                                       |
| Define chain (advanced)     | Nav mode on → hold step (start) + tap step (end)                           |
| Clear current pattern       | Hold step 1 + step 16                                                      |
| Clear all patterns          | Hold step 1 + step 12                                                      |
| Octave shift                | Config menu → Octave shift, up/down                                        |
| Note shift                  | Config menu → Note shift, up/down                                          |
| Note range                  | Config menu → Note range → submenu (select preset or Custom for lo/hi)     |
| Set note scale              | Config menu → Note scales, Enter for scale/root phases                     |
| Set pattern length          | Config menu → Pat length, up/down or tap step button                       |
| Set pattern direction       | Config menu → Pat dir, up/down cycles 8 modes                              |
| Save                        | Config menu → Save (sequencer stopped)                                     |
| Toggle Simple/Advanced      | Config menu → Mode                                                         |
| Open config menu            | Double-tap Enter                                                           |
| Enter diagnostics           | Config menu → Diagnostics → submenu (LED test / Input test / Hi trim)      |

---

## Troubleshooting

### "The device boots into Advanced mode even though I want Simple mode"

The sequencer saves and restores all settings including Simple/Advanced mode. If a previous session saved with Advanced mode active, every boot will load it back.

**To fix:**

1. Double-tap Enter to open the config menu.
2. Scroll to **Mode: Advanced** (or **Mode: Simple** — it shows the current state).
3. Press **Enter** — this enters the value editor. Line 2 shows `  Advanced  ` or `  Simple  `.
4. Press **D-pad up or down** to toggle to `  Simple  `.
5. Press **Enter** (or D-pad left) to confirm and exit editing. Line 1 should now show `> Mode: Simple  `.
6. Scroll up to **Save** and press Enter. The sequencer must be stopped to save.

The next boot will start in Simple mode.

> **Why this happens:** Save writes exactly what is currently in memory — it does not reset any settings to defaults. If the mode was Advanced when you saved, it stays Advanced. You must explicitly change the Mode item before saving.

---

### "Save is blocked — it shows 'Stop first!'"

Saving while the sequencer is playing is intentionally blocked to prevent timing glitches during the flash write.

**Steps:**
1. Press the Play button to stop the sequencer.
2. The config menu redraws automatically — the **Save** item is ready.
3. Press Enter to save.

---

### "I pressed Enter on Mode but the value won't change / keeps toggling back"

The Mode item uses an editing sub-state:

- Pressing **Enter** on Mode *enters the editor* — it does not immediately change the value.
- Use **D-pad up or down** to toggle between Simple and Advanced.
- Press **Enter** (or D-pad left) to *confirm and exit* the editor.

If you press Enter twice without pressing up or down, you enter and then exit editing without changing anything — the mode stays the same. This is intentional: it prevents accidental mode changes.

---

### "The sequencer is playing but not following my external MIDI clock"

- Confirm **Clock: ext** is shown in the config menu (not `Clock: int`).
- Make sure the external device is sending MIDI clock (0xF8 at 24 PPQN) over USB to the sequencer.
- In EXT mode the sequencer does not output its own clock — it only follows.
- If you press Play before the external clock is running, the sequencer arms and waits; it starts on the next incoming 0xF8 pulse.
- If the external clock is already running when you press Play, the sequencer waits for the next beat boundary (6th pulse) before starting, to lock in phase.

---

### "Step LEDs show pattern numbers instead of step data"

You are in **pattern-nav mode** (Advanced mode only). Pattern button 1 LED is lit.

- **Single-click pattern button 1** (tap and wait ~400 ms) to exit nav mode. Step LEDs return to normal step display.

---

### "After clearing or saving, the LCD seems stuck on an old message"

The config menu draws directly to the LCD while it is open, bypassing the normal update rate limit. If the display looks frozen after a stop or a menu action, press a D-pad button or wait for the next step to fire — the LCD will refresh.

---

### "Settings are lost after power-off"

- Check that the SD card is seated properly. If the SD card is missing or unreadable, the sequencer falls back to EEPROM on boot, which may have older data.
- The sequencer does **not** auto-save — you must save manually via Config Menu → Save.
- If no SD card and no valid EEPROM save exists, the sequencer starts with factory defaults on every boot.
- After saving successfully, the LCD briefly shows `saved!` for 2 seconds. If you do not see this, the save did not complete.

---

## MIDI CC Number Reference

The CC Number config menu item selects which MIDI CC controller the current pattern outputs. The sequencer skips reserved CC numbers automatically. The LCD shows a 7-character abbreviated name; the full name is listed below.

Forbidden/skipped: CC 32 (Bank LSB), CC 96–101 (RPN/NRPN data entry), CC 120–127 (Channel Mode Messages — not accessible).

| CC  | LCD Name | Full Name                              |
| --- | -------- | -------------------------------------- |
| 1   | ModWhl   | Modulation Wheel                       |
| 2   | BrethC   | Breath Controller                      |
| 3   | Ctrl 3   | Controller 3 (undefined)               |
| 4   | FootC    | Foot Controller                        |
| 5   | PortTm   | Portamento Time                        |
| 6   | DataEn   | Data Entry MSB                         |
| 7   | Volume   | Channel Volume                         |
| 8   | Balance  | Balance                                |
| 9   | Ctrl 9   | Controller 9 (undefined)               |
| 10  | Pan      | Pan                                    |
| 11  | ExprC    | Expression Controller                  |
| 12  | FxCtl1   | Effect Control 1                       |
| 13  | FxCtl2   | Effect Control 2                       |
| 14  | Ctl 14   | Controller 14 (undefined)              |
| 15  | Ctl 15   | Controller 15 (undefined)              |
| 16  | GPC 1    | General Purpose Controller 1           |
| 17  | GPC 2    | General Purpose Controller 2           |
| 18  | GPC 3    | General Purpose Controller 3           |
| 19  | GPC 4    | General Purpose Controller 4           |
| 20  | Ctl 20   | Controller 20 (undefined)              |
| 21  | Ctl 21   | Controller 21 (undefined)              |
| 22  | Ctl 22   | Controller 22 (undefined)              |
| 23  | Ctl 23   | Controller 23 (undefined)              |
| 24  | Ctl 24   | Controller 24 (undefined)              |
| 25  | Ctl 25   | Controller 25 (undefined)              |
| 26  | Ctl 26   | Controller 26 (undefined)              |
| 27  | Ctl 27   | Controller 27 (undefined)              |
| 28  | Ctl 28   | Controller 28 (undefined)              |
| 29  | Ctl 29   | Controller 29 (undefined)              |
| 30  | Ctl 30   | Controller 30 (undefined)              |
| 31  | Ctl 31   | Controller 31 (undefined)              |
| 33  | ModLSB   | Modulation Wheel LSB                   |
| 34  | BrhLSB   | Breath Controller LSB                  |
| 35  | C3 LSB   | Controller 3 LSB                       |
| 36  | FotLSB   | Foot Controller LSB                    |
| 37  | PrtLSB   | Portamento Time LSB                    |
| 38  | DatLSB   | Data Entry LSB                         |
| 39  | VolLSB   | Channel Volume LSB                     |
| 40  | BalLSB   | Balance LSB                            |
| 41  | C9 LSB   | Controller 9 LSB                       |
| 42  | PanLSB   | Pan LSB                                |
| 43  | ExpLSB   | Expression Controller LSB              |
| 44  | Fx1LSB   | Effect Control 1 LSB                   |
| 45  | Fx2LSB   | Effect Control 2 LSB                   |
| 46  | C14LSB   | Controller 14 LSB                      |
| 47  | C15LSB   | Controller 15 LSB                      |
| 48  | G1 LSB   | General Purpose Controller 1 LSB       |
| 49  | G2 LSB   | General Purpose Controller 2 LSB       |
| 50  | G3 LSB   | General Purpose Controller 3 LSB       |
| 51  | G4 LSB   | General Purpose Controller 4 LSB       |
| 52  | C20LSB   | Controller 20 LSB                      |
| 53  | C21LSB   | Controller 21 LSB                      |
| 54  | C22LSB   | Controller 22 LSB                      |
| 55  | C23LSB   | Controller 23 LSB                      |
| 56  | C24LSB   | Controller 24 LSB                      |
| 57  | C25LSB   | Controller 25 LSB                      |
| 58  | C26LSB   | Controller 26 LSB                      |
| 59  | C27LSB   | Controller 27 LSB                      |
| 60  | C28LSB   | Controller 28 LSB                      |
| 61  | C29LSB   | Controller 29 LSB                      |
| 62  | C30LSB   | Controller 30 LSB                      |
| 63  | C31LSB   | Controller 31 LSB                      |
| 64  | Sustain  | Sustain Pedal (Damper)                 |
| 65  | Portan   | Portamento On/Off                      |
| 66  | Sost     | Sostenuto Pedal                        |
| 67  | SoftP    | Soft Pedal                             |
| 68  | LegatoF  | Legato Footswitch                      |
| 69  | Hold2    | Hold 2                                 |
| 70  | SndCtl1  | Sound Controller 1 (Sound Variation)   |
| 71  | SndCtl2  | Sound Controller 2 (Timbre/Harmonic)   |
| 72  | SndCtl3  | Sound Controller 3 (Release Time)      |
| 73  | SndCtl4  | Sound Controller 4 (Attack Time)       |
| 74  | SndCtl5  | Sound Controller 5 (Brightness/Cutoff) |
| 75  | SndCtl6  | Sound Controller 6 (Decay Time)        |
| 76  | SndCtl7  | Sound Controller 7 (Vibrato Rate)      |
| 77  | SndCtl8  | Sound Controller 8 (Vibrato Depth)     |
| 78  | SndCtl9  | Sound Controller 9 (Vibrato Delay)     |
| 79  | SndC10   | Sound Controller 10                    |
| 80  | GPC 5    | General Purpose Controller 5           |
| 81  | GPC 6    | General Purpose Controller 6           |
| 82  | GPC 7    | General Purpose Controller 7           |
| 83  | GPC 8    | General Purpose Controller 8           |
| 84  | PrtCtl   | Portamento Control                     |
| 85  | Ctl 85   | Controller 85 (undefined)              |
| 86  | Ctl 86   | Controller 86 (undefined)              |
| 87  | Ctl 87   | Controller 87 (undefined)              |
| 88  | HiResV   | High-Resolution Velocity Prefix        |
| 89  | Ctl 89   | Controller 89 (undefined)              |
| 90  | Ctl 90   | Controller 90 (undefined)              |
| 91  | FxDpth   | Effects 1 Depth (Reverb Send)          |
| 92  | TrmDpth  | Effects 2 Depth (Tremolo Depth)        |
| 93  | ChoDepth | Effects 3 Depth (Chorus Send)          |
| 94  | CelDpth  | Effects 4 Depth (Celeste/Detune Depth) |
| 95  | PhaDepth | Effects 5 Depth (Phaser Depth)         |
| 102 | Ctl102   | Controller 102 (undefined)             |
| 103 | Ctl103   | Controller 103 (undefined)             |
| 104 | Ctl104   | Controller 104 (undefined)             |
| 105 | Ctl105   | Controller 105 (undefined)             |
| 106 | Ctl106   | Controller 106 (undefined)             |
| 107 | Ctl107   | Controller 107 (undefined)             |
| 108 | Ctl108   | Controller 108 (undefined)             |
| 109 | Ctl109   | Controller 109 (undefined)             |
| 110 | Ctl110   | Controller 110 (undefined)             |
| 111 | Ctl111   | Controller 111 (undefined)             |
| 112 | Ctl112   | Controller 112 (undefined)             |
| 113 | Ctl113   | Controller 113 (undefined)             |
| 114 | Ctl114   | Controller 114 (undefined)             |
| 115 | Ctl115   | Controller 115 (undefined)             |
| 116 | Ctl116   | Controller 116 (undefined)             |
| 117 | Ctl117   | Controller 117 (undefined)             |
| 118 | Ctl118   | Controller 118 (undefined)             |
| 119 | Ctl119   | Controller 119 (undefined)             |
