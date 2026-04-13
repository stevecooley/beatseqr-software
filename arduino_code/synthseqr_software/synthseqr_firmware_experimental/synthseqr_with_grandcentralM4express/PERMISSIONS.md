# Synthseqr User Interaction Reference

Quick reference for hardware controls and their current behavior.

## Slider Modes

The 16 faders control different data depending on the active mode.

| Mode | What sliders do |
|------|----------------|
| NN (note number) | Sets the MIDI pitch for each step (within note range) |
| VL (velocity) | Sets the MIDI velocity for each step (1–127) |
| GT (gate) | Sets how many steps each note holds (1–8) |

**Switching modes (simple mode):** Single-tap Enter cycles NN → VL → GT → NN.

**Switching modes (advanced mode):** Pattern button 1 = NN, button 2 = GT, button 3 = VL. The LED above the active mode button stays lit as a reminder.

**Important:** When you switch modes, sliders are locked out until each physical slider moves through its stored value for the new mode. This prevents accidental data corruption when the faders are at different positions between modes.

---

## Enter Button

| Context | Single tap | Double tap |
|---------|-----------|------------|
| Simple mode | Cycle slider mode (NN/VL/GT) | Open config menu |
| Advanced mode | No function | Open config menu |

---

## Pattern Buttons

### Simple Mode

| Button | Short press | Hold 2s |
|--------|------------|---------|
| Pat 0 | Select pattern 0 | Copy current pattern (press dest to complete) |
| Pat 1 | Select pattern 1 | Copy current pattern |
| Pat 2 | Select pattern 2 | Copy current pattern |
| Pat 3 | Select pattern 3 | Copy current pattern |
| Pat 0 + Pat 3 simultaneously | Toggle 4-pattern chain | — |

### Advanced Mode

| Button | Single click | Double click | Hold |
|--------|-------------|--------------|------|
| Pat 0 | Toggle pattern-nav mode | Enter 2-phase pattern copy | — |
| Pat 1 | Slider mode → NN | — | — |
| Pat 2 | Slider mode → GT | — | — |
| Pat 3 | Slider mode → VL | — | — |

**Pattern-nav mode** (after single-click Pat 0, LED 0 lit):
- Tap a step button → jump to that pattern
- Hold one step, tap another → define a chain (wrap-around supported)
- Single-click Pat 0 again → exit nav mode

**2-phase copy** (after double-click Pat 0):
- Phase 1: tap a step → that becomes the source pattern
- Phase 2: tap a step → copies source to that destination
- D-pad left → cancel at any point

---

## D-Pad Navigation

D-pad left/right moves between 5 timing modes. Up/down adjusts the selected value.

| Timing mode | Up/Down adjusts |
|-------------|----------------|
| 1 | Pattern (wraps: 1–4 simple, 1–16 advanced) |
| 2 | Tempo ±10 BPM |
| 3 | Tempo ±1 BPM |
| 4 | Tempo ±0.1 BPM |
| 5 | Tempo ±0.01 BPM |

---

## Step Buttons

| Action | Result |
|--------|--------|
| Single press (not playing) | Toggle step on/off |
| Single press (playing) | Toggle step on/off for current pattern |
| Hold step 0 + press step 15 | Clear current pattern |
| Hold step 0 + press step 11 | Clear all 16 patterns |

**Advanced mode pattern-nav** (when LED 0 is lit): step buttons select/chain patterns instead of editing steps.

---

## Config Menu (double-tap Enter)

| Item | Action |
|------|--------|
| Exit | D-pad left, or Enter |
| Save | Saves to SD + EEPROM (must stop sequencer first) |
| Clear pattern | Clears current pattern (confirmation required) |
| Clear all pats | Clears all 16 patterns (confirmation required) |
| Reset sliders | Resets pitches/velocities/gates to defaults (confirmation required) |
| Clock: int/ext | Toggle internal TC4 / external USB-MIDI clock |
| Channel | MIDI output channel (1–16) |
| Swing | Swing amount (0–5; 0 = straight, 2 = classic triplet) |
| Mode | Toggle Simple / Advanced mode |
| Octave shift | Transpose all notes ±5 octaves |
| Note shift | Transpose all notes ±12 semitones |
| Note range | Set low/high MIDI note for slider NN range |
| Note scales | Coming soon |

---

# Claude Code Permissions

## Auto-Allowed (no prompt required)

These are configured in `.claude/settings.local.json` for this project:

| Permission | Scope |
|---|---|
| `arduino-cli board` commands | List/detect connected boards |
| `arduino-cli compile` commands | Compile the firmware |
| `arduino-cli upload` commands | Flash firmware to the device |
| `arduino-cli lib` commands | Manage Arduino libraries |
| `Bash(echo "EXIT:$?")` | Read exit codes from shell commands |
| Read Adafruit SAMD library files | `~/Library/Arduino15/packages/adafruit/hardware/samd/1.7.17/libraries/**` |
| Read Arduino library files | `~/Documents/Arduino/libraries/**` |

## Require User Approval (prompt shown)

Everything not listed above will prompt the user before executing:

- All other `Bash` commands (git, find, system commands, etc.)
- `Write` — creating new files
- `Edit` — modifying existing files
- `Read` — reading files outside the explicitly allowed library paths (though in practice project files are routinely approved)
- `WebFetch` / `WebSearch` — network requests
- Spawning background agents

## Never Allowed (hardcoded restrictions)

Regardless of settings, Claude Code will not:

- Generate destructive techniques, DoS attacks, or mass-targeting tools
- Help evade detection for malicious purposes
- Assist with supply chain compromise
- Push to remote git repositories without explicit confirmation
- Run `git push --force` to main/master
- Skip commit hooks (`--no-verify`) without explicit request

## Notes

- The `/flash` skill (arduino-cli compile + upload) runs automatically because both `compile` and `upload` are pre-approved.
- File reads/writes within the project directory will prompt if not pre-approved, but the user can approve them per-session or add them to `settings.local.json`.
- To pre-approve additional commands, add them to `.claude/settings.local.json` in the `permissions.allow` array.
