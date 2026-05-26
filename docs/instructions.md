# Synthseqr — User Guide

Synthseqr is a 16-step MIDI step sequencer with 16 patterns, 16 voice sliders, a D-pad, and four pattern select buttons. All settings are saved to the SD card and reload automatically on power-up.

---

## Quick Start

1. Power on — the sequencer loads your last save automatically
2. Press **Play** to start the sequencer
3. Tap any of the 16 **step buttons** to toggle steps on/off
4. Move the **sliders** to set the pitch for each step
5. Double-tap **Enter** to open the config menu and save your work

---

## Slider Modes

The 16 sliders change function depending on the active slider mode. The current mode is shown in the top-right corner of the LCD.

| Mode | Indicator | What the sliders control |
|------|-----------|--------------------------|
| NN   | ♪N        | Note pitch (MIDI note number) |
| VL   | ♪V        | Velocity (1–127) |
| GT   | ♪G        | Gate length (1–16 steps) |
| CC   | ♪C        | MIDI CC value (0–127) per step (sequenced — fires with each step) |
| PR   | ♪P        | Fire probability per step (0–100%) |
| LV   | ♪L        | **Live** MIDI CC — sliders transmit CC continuously on movement |

**Slider takeover**: when you switch modes, the physical slider position won't match the stored value. Choose how sliders re-engage from **Config menu → Takeover**:

- **Catch** (default): the slider must physically pass through the stored value before taking over. While any slider is still waiting to engage, LCD line 2 shows a 16-character overlay (one position per slider): `^` = push the slider up to catch, `v` = pull it down, blank = already engaged. The overlay clears automatically once every slider is engaged.
- **Jump**: touch a slider and it immediately takes over — its current position becomes the new stored value. No overlay.
- **Relative**: every slider movement adds a delta to the stored value (1:1 — full physical travel covers the full mode range). No abrupt jump and no waiting. The slider position no longer corresponds to the stored value, but small movements accumulate so you can dial in fine changes.

LV mode is exempt from this setting — the slider position IS the value and only transmits when you move it.

**Single-tap Enter** cycles through all enabled modes in both Simple and Advanced mode: NN → VL → GT → CC → PR → LV → NN. In Advanced mode, pattern buttons 1/2/3 also jump straight to NN/VL/PR.

---

## Step Buttons

- **Tap** — toggle step on or off
- **Hold one step, tap another** — set gate length. The gate spans from the held step to the tapped step (wraps around). The held step turns on; the tapped step is not toggled.

In **CC mode** the step buttons toggle CC-send on/off for each step independently of the note on/off state.

In **LV mode** the step buttons select which slider lane's CC# is being edited. Tap a step → that lane is in edit mode (LED on); use d-pad up/down to pick its CC number. Tap the same step again, or press Enter or d-pad left, to exit editing. Other sliders keep transmitting live CC while you edit.

---

## Pattern Select Buttons

### Simple Mode (4 patterns)

| Action | Result |
|--------|--------|
| Press button 0–3 | Switch to pattern 1–4 |
| Press 0 + 3 together | Toggle 4-pattern chain loop |
| Hold any button 2s, press another | Copy pattern to destination |

### Advanced Mode (16 patterns)

Pattern buttons are function keys — they do not select patterns directly.

| Button | Function |
|--------|----------|
| 0 (single-click) | Toggle pattern-nav mode |
| 0 (double-click) | Start 2-phase pattern copy |
| 1 | Slider mode → NN (pitch) |
| 2 | Slider mode → VL (velocity) |
| 3 | Slider mode → PR (probability) |

**Pattern-nav mode**: while active, LED 0 stays lit and the 16 step buttons become pattern selectors.
- Tap one step → switch to that pattern
- Hold one step, tap another → define a chain range (wrap-around supported)
- Single-click button 0 again → exit nav mode

---

## D-Pad

| Direction | Action |
|-----------|--------|
| Up / Down | Select next/previous pattern (in LV edit mode: change the focused lane's CC number) |
| Left      | No action on main screen; exits menus, cancels operations, exits LV lane edit |

---

## Config Menu

Double-tap **Enter** to open. D-pad up/down scrolls. Enter selects. D-pad left exits from anywhere.

Only items whose feature is active appear in the list. Disabled features hide their menu items automatically.

| Item | What it does |
|------|-------------|
| Exit | Close the menu |
| Save | Save all settings to SD card + EEPROM |
| Tempo | Adjust BPM — Enter cycles resolution (±10 / ±1 / ±0.1), up/down adjusts, left exits. Hidden when using external clock. |
| Clear pattern | Erase all steps and reset sliders for the current pattern |
| Clear all pats | Erase all 16 patterns |
| Reset sliders | Reset pitches, velocities, and gates for the current pattern to defaults |
| Mode | Toggle Simple / Advanced mode |
| Clock | Toggle internal / external MIDI clock |
| Channel | Set MIDI output channel (1–16) |
| Swing | Set swing amount (0=off, 1–5) |
| Diagnostics | Toggle hardware test mode on next boot |
| Octave shift | Shift all notes ±5 octaves at send time |
| Note shift | Shift all notes ±12 semitones at send time |
| Note range | Set low and high MIDI note limits for the sliders (defaults: 36–52, always accessible) |
| Note scales | Set scale type and root note; all stored pitches are quantized immediately |
| Pat length | Set pattern length 1–16 steps |
| Pat dir | Set playback direction: Fwd / Rev / Pong / Rand / Shuf / E·O / In / Quad |
| CC number | Set the MIDI CC number sent for this pattern (1–119) |
| Live CC ch | Set the MIDI channel used by LV (live CC) mode — independent of the main MIDI channel |
| Step prob | Switch sliders to PR (probability) mode |
| Pitch drift | Add random pitch wander ±0–7 semitones per note-on |
| Takeover | How sliders re-engage after a mode switch: Catch / Jump / Relative (see Slider Modes) |
| Features | Open the Features submenu |

---

## Features Submenu

**Config menu → Features**. Up/down scrolls, Enter toggles on/off, left exits.

Turning a feature off hides its menu items and disables the associated slider mode or behavior. Settings are not saved automatically — use **Save** after making changes.

| Feature | What it controls |
|---------|-----------------|
| Advanced mode | 16-pattern mode, pattern nav/copy/chain, advanced button layout |
| CC mode | Slider mode CC, CC number menu item |
| Probability | Slider mode PR, Step prob menu item |
| Gate sliders | Slider mode GT (the hold+tap key-combo for gates always works regardless) |
| Note scales | Scale quantization, Note scales menu item (Note range is always accessible) |
| Pitch drift | Pitch drift menu item |
| Pat direction | Pattern direction menu item |
| Pat length | Variable pattern length menu item |
| Swing | Swing menu item |
| Ext clock | External clock menu item |
| Oct/note shift | Octave shift and Note shift menu items |
| Diagnostics | Diagnostics menu item and hardware test mode |
| Velocity sliders | Slider mode VL |
| Note audition | MIDI preview note when toggling a step while stopped (on by default) |
| Live CC mode | Slider mode LV and Live CC ch menu item (on by default) |

**Tip**: for a minimal setup, turn off everything except the features you actually use. The config menu becomes much shorter and easier to navigate.

---

## Saving

**Config menu → Save** writes to the SD card (`/synthseqr/autosave.json`) and to EEPROM as a backup. The sequencer must be stopped before saving.

On power-up the SD card is read first. If no card or file is found, EEPROM is used. If neither has a save, factory defaults are loaded.

---

## Pattern Chains

When a chain is active, the sequencer automatically advances to the next pattern at the end of each cycle.

**Simple mode**: press buttons 0 + 3 together to toggle a 4-pattern chain (patterns 1→2→3→4→1…).

**Advanced mode**: in pattern-nav mode, hold one step button and tap another to define a chain start and end. Wrap-around chains (e.g. patterns 8→16→1→3) are supported.

---

## External Clock

**Config menu → Clock → ext** switches to external MIDI clock mode. TC4 is stopped and the sequencer follows incoming USB-MIDI 0xF8 clock pulses instead.

- Play button arms the sequencer; it starts on the next beat boundary
- MIDI Start (0xFA) and Stop (0xFC) messages are also respected
- Tempo editing is hidden in the config menu while in external clock mode
- Swing still works in external clock mode

---

## Scale Quantization

**Config menu → Note scales**: choose a scale type (Chromatic, Major, Natural Minor, Pentatonic Major/Minor, Dorian, Mixolydian, Harmonic Minor, Blues) and a root note (C–B).

When you change scale or root, all stored pitches across all 16 patterns are immediately snapped to the nearest in-scale note. Pickup guards are re-armed so sliders don't overwrite on the next touch.

---

## Note Range

**Config menu → Note range**: sets the lowest and highest MIDI note the sliders can reach. Default is 36 (C2) to 52 (E3). Adjusting the range changes what pitch corresponds to each physical slider position across the full travel.

---

## Pitch Drift

**Config menu → Pitch drift**: adds a random semitone offset (±0–7) to each note at the moment it fires. Drift is applied after octave/note shift and before scale quantization, so the output always stays in-scale when a scale is active.

---

## Live CC Mode (LV)

LV mode turns each slider into a live MIDI CC controller. Sliders transmit CC messages continuously as you move them — they are **not** sequenced; the values play in real time alongside the running sequence.

**Turning it on**: enabled by default. Toggle via **Config menu → Features → Live CC mode**. Single-tap Enter to cycle into LV (`♪L` indicator on the LCD).

**Assigning CC numbers**: tap any **step button** to put that lane into edit mode (its LED lights up). The LCD shows the lane's current CC number and name (e.g. `L05:007 Volume`). Use **d-pad up/down** to change the CC number. **Tap the same step again**, or press **Enter** or **d-pad left**, to exit editing. Other sliders keep sending live CC while you edit.

**MIDI channel**: LV uses its own channel, set in **Config menu → Live CC ch** (1–16). This is independent of the main MIDI channel — point LV at a different synth or destination without affecting the note sequence.

**Lane defaults**: lanes default to CC 1 through CC 16. Per-lane CC numbers and the LV channel are saved with **Save**.

**Concurrency**: LV runs alongside the note sequencer and the existing sequenced CC mode (slider mode CC). The two CC modes use independent channels by default, so they won't collide.

---

## Pattern Directions

| Direction | Behaviour |
|-----------|-----------|
| Fwd | Steps 1→16 (default) |
| Rev | Steps 16→1 |
| Pong | 1→16→15→…→1 (ping-pong) |
| Rand | Random step each tick |
| Shuf | Randomised order, reshuffled each cycle |
| E·O | Even steps first, then odd |
| In | Outside-in (1,16,2,15,3,14…) |
| Quad | Quadrant reorder (Q1, Q3, Q2, Q4) |

---

## Diagnostics Mode

Hold D-pad left + right simultaneously for 1 second to enter hardware test mode. All sequencer operations are paused. Buttons, sliders, and D-pad inputs are shown on the LCD in real time. Press the same combination again to exit.
