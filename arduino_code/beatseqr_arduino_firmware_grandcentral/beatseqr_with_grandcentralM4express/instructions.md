# Beatseqr Firmware — User Instructions

Firmware v1.0 for Beatseqr hardware v4.51  
Grand Central M4 Express port — 2026

---

## Overview

Beatseqr is a 16-step MIDI drum sequencer with 8 voices. Each voice represents one drum sound (kick, snare, hi-hat, etc.). Steps are toggled on/off per voice. The sequencer plays back all 8 voices simultaneously on MIDI channel 10 (General MIDI drums).

---

## Controls at a Glance

| Control | Function |
|---------|----------|
| **Play button** | Start / Stop sequencer |
| **16 step buttons** | Toggle steps on/off for the active voice |
| **8 voice-select buttons** | Select which voice to edit |
| **8 voice sliders** | Set pitch / velocity / gate / CC / probability for each voice |
| **Slider mode button** | Cycle slider mode: NN → VL → GT → CC → NN |
| **Tempo knob** | Set BPM (30–250) |
| **Swing knob** | Set swing amount (0–5) |
| **4 pattern buttons** | Select pattern (Simple) or function keys (Advanced) |
| **Knob mode button** (double-tap) | Enter / exit config menu |
| **param_rec button** | Confirm / Enter in config menu |

---

## Basic Operation

### 1. Select a Voice
Press one of the 8 voice-select buttons. The corresponding LED lights up to show which voice is active. The 16 step LEDs refresh to show that voice's current pattern. The selected voice stays active after releasing the button.

### 2. Program Steps
Press any of the 16 step buttons to toggle steps on (LED lit) or off. Active steps will fire a MIDI note when the sequencer reaches them.

### 3. Play
Press the **Play** button. The sequencer starts. A chase light blinks on the current step as it plays.

### 4. Adjust Pitch (NN Mode)
With the **Slider mode button** in NN mode (default on boot), each voice's slider sets the MIDI note number for that voice. All 8 sliders work simultaneously — you don't need to switch voices to adjust them.

### 5. Adjust Tempo and Swing
Turn the **Tempo knob** (A8) to set BPM between 30 and 250.  
Turn the **Swing knob** (A9) to add swing: 0 = straight, 1–5 = progressively heavier.

---

## Slider Modes

Press the **Slider mode button** to cycle through four modes. All 8 sliders are active in every mode — slider N always controls voice N.

| Mode | Display | Each slider sets… |
|------|---------|-------------------|
| **NN** | N | MIDI note number (default 36–51, GM drum range) |
| **VL** | V | Velocity 0–127. Slider fully down = 0 = voice muted |
| **GT** | G | Gate length 1–8 steps (how long the note rings) |
| **CC** | C | CC value 0–127 for a MIDI control change message |
| **PR** | P | Fire probability 0–100% for each voice |

**PR mode** is accessed via the config menu → **Voice prob**. The Slider mode button only cycles NN → VL → GT → CC; use the menu to enter PR mode. PR is saved per pattern.

**Pickup guard**: after switching modes, a slider won't change its value until it physically reaches the currently-stored value. This prevents accidental jumps when sliders are at different positions for different modes.

---

## Pattern Operations

### Simple Mode (default)

- **Buttons 0–3** select patterns 0–3 directly (LED shows active pattern)
- **Hold a pattern button for 2 seconds** to arm a pattern copy — then press a destination pattern button
- **Press buttons 0 + 3 simultaneously** to toggle chain mode (plays patterns 0→1→2→3 looping)

### Advanced Mode

Advanced mode is toggled via the config menu (see below).

In Advanced mode the 4 pattern buttons become function keys:

| Button | Function |
|--------|----------|
| **0 — single tap** (400 ms timeout) | Toggle pattern-nav mode |
| **0 — double-tap** (within 400 ms) | Enter 2-phase pattern copy |
| **1** | Slider mode: NN |
| **2** | Slider mode: GT |
| **3** | Slider mode: VL |

**Pattern-nav mode** (button 0 single-tap): Step buttons select patterns instead of editing steps. Tap one step = jump to that pattern. Hold one step and tap another = chain those patterns. Press button 0 again to exit.

**2-phase copy** (button 0 double-tap):
1. LCD shows `copy which pat?` — tap a step button to select the source pattern
2. LCD shows `Copy N->where?` — tap a step button to select the destination
3. LCD confirms `Copied X to Y`

To cancel a copy in progress: turn the **Swing knob CCW**.

---

## Clear Operations

| Combo | Action |
|-------|--------|
| Hold **step 0** + **step 15** | Clear all steps for the active voice in the current pattern |
| Hold **step 0** + **step 11** | Clear all 16 patterns, all 8 voices |

---

## Config Menu

Enter the config menu by **double-tapping the Knob mode button** (two presses within 400 ms). The LCD shows the menu.

**Navigation:**

| Knob/Button | Action |
|-------------|--------|
| **Tempo knob** CW | Next menu item / increase value |
| **Tempo knob** CCW | Previous menu item / decrease value |
| **Swing knob** CCW | Cancel / exit editing / exit menu |
| **Swing knob** CW | Exit menu (only on the "Exit" item) |
| **param_rec** | Select item / confirm |

**Menu items:**

| Item | Description |
|------|-------------|
| **Exit** | Leave the config menu |
| **Save** | Save to SD card + EEPROM backup (must be stopped) |
| **Clear pattern** | Clear all 8 voices for the current pattern (confirm required) |
| **Clear all pats** | Clear all 16 patterns (confirm required) |
| **Reset sliders** | Reset pitch/velocity/gate/CC to defaults (confirm required) |
| **Mode** | Toggle Simple / Advanced mode |
| **Clock** | Toggle internal / external USB-MIDI clock |
| **Channel** | MIDI channel (1–16; default 10 for GM drums) |
| **Diagnostics** | Hardware self-test mode |
| **Octave shift** | Transpose all notes ±5 octaves at send time |
| **Note shift** | Fine transpose ±12 semitones at send time |
| **Note range** | Set low/high MIDI note range for sliders (two-phase: Lo then Hi) |
| **Note scales** | Constrain notes to a musical scale; retroactively quantizes all patterns |
| **Pat length** | Pattern length 1–16 steps (or tap a step button to set directly) |
| **Pat dir** | Playback direction: Fwd / Rev / Pong / Rand / Shuf / E/O / In / Quad |
| **CC number** | MIDI CC controller number for this pattern (1–119) |
| **Tempo** | BPM (integer); tempo knob CW/CCW to adjust; param_rec to exit |
| **Swing** | Swing amount 0–5; same as the hardware swing knob |
| **Voice prob** | Exits the menu and activates PR slider mode — sliders set per-voice probability 0–100% |

---

## Voice Probability (PR Mode)

Each voice can have an independent fire probability per pattern. A voice with probability 100 (the default) always fires on its active steps. A voice at 50 fires roughly half the time. A voice at 0 is silenced entirely.

**To set probabilities:**
1. Open the config menu (double-tap Knob mode button)
2. Scroll to **Voice prob** and press param_rec
3. The menu closes and slider mode switches to **PR** (LCD shows `P` in the mode indicator)
4. Move any of the 8 sliders — slider N sets voice N's probability for the current pattern
5. To exit PR mode, press the **Slider mode button** to cycle back to NN

**Notes:**
- Probability is per-pattern — each of the 16 patterns has its own set of probabilities
- The menu label shows `*` when any voice in the current pattern has probability below 100%
- Patterns are silenced completely if their voice probability reaches 0, but any CC messages still fire normally
- Probabilities are saved with the pattern (SD card and EEPROM)
- Clearing a pattern or resetting sliders resets all probabilities to 100%

---

## Saving Your Work

Settings and patterns are **not saved automatically**. To save:
1. **Stop the sequencer** (press Play if it is running)
2. Enter the config menu (double-tap Knob mode button)
3. Navigate to **Save** and press param_rec

Saves go to `/beatseqr/autosave.json` on the SD card (primary) and to EEPROM flash (backup). On boot, the SD card is loaded first; EEPROM is only used if no SD card is found.

---

## External MIDI Clock

Go to config menu → **Clock: ext** to follow an incoming USB-MIDI clock.

- The Play button still works as a local start/stop
- When Play is pressed while a clock is running, the sequencer starts on the next step boundary (beat-sync start)
- MIDI Start (0xFA) and Stop (0xFC) from the host also control the sequencer
- Swing works in external clock mode

---

## Pattern Directions

| Mode | Behavior |
|------|----------|
| **Fwd** | Steps 1→16 (default) |
| **Rev** | Steps 16→1 |
| **Pong** | Steps 1→16→15→…→1, bouncing |
| **Rand** | Random step each tick |
| **Shuf** | Fisher-Yates shuffle, new permutation each cycle |
| **E/O** | Even steps then odd steps (1,3,5…2,4,6…) |
| **In** | Outside-in (1,16,2,15,3,14…) |
| **Quad** | Q1,Q3,Q2,Q4 quadrant reordering |

---

## Note Scales

When a scale is set (config menu → **Note scales**), the NN sliders are constrained to only produce in-scale notes. Changing the scale or root retroactively quantizes all stored pitches across all 16 patterns to the nearest in-scale note.

Available scales: Chromatic (off), Major, Natural Minor, Pentatonic Major, Pentatonic Minor, Dorian, Mixolydian, Harmonic Minor, Blues.

Root notes: C through B (all 12 chromatic roots).

---

## CC Output

In **CC mode** (slider mode 4), each voice's slider sets a CC value. A CC message is sent on each active step if `voice_cc_enabled` is set for that voice. The CC controller number is shared across all voices in a pattern and is set via config menu → **CC number**.

---

## Boot Behavior

On power-up:
1. LCD shows `beatseqr v4.51` splash
2. Loads patterns and settings from SD card (or EEPROM if no SD)
3. Sequencer starts stopped on pattern 1
4. Slider mode defaults to NN
5. Voice 0 (first voice) LED lights up

---

## Default Voice Assignments (GM Drums, channel 10)

| Voice | Note | GM Drum |
|-------|------|---------|
| 0 | 36 | Bass Drum 1 |
| 1 | 38 | Snare Drum 1 |
| 2 | 42 | Closed Hi-Hat |
| 3 | 46 | Open Hi-Hat |
| 4 | 49 | Crash Cymbal 1 |
| 5 | 51 | Ride Cymbal 1 |
| 6 | 37 | Side Stick |
| 7 | 39 | Hand Clap |

These are defaults; the NN slider (or note range / octave shift settings) can map any voice to any MIDI note.
