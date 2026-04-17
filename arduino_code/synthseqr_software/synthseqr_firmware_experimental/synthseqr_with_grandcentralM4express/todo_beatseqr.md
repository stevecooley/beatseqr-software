# Beatseqr Port TODO

## Hardware / Calibration
- [ ] ADC resolution: using `analogReadResolution(10)` in setup to keep voice-select resistor
      ladder thresholds valid (vselectval_lowerranges/upperranges were calibrated on 10-bit ADC).
      Future task: recalibrate thresholds × 4 for native 12-bit (0–4095) range if better
      precision is needed.

## Future Ideas
- [ ] TRS MIDI jack: omgwtf LED pin (currently wired directly to 5V, always on) — not usable
      as input. A spare analog pin would be needed for TRS MIDI input. Defer.

## Confirmed Decisions

### Hardware pin assignments
| Function                  | Pin          |
|---------------------------|--------------|
| Play button (interrupt)   | A11 / pin 65 |
| Play LED                  | pin 10       |
| Enter (param_rec/omgwtf)  | D11          |
| Voice select LEDs         | pins 2–9     |
| Voice select ladder       | A10          |
| Sliders (8 voices)        | A0–A7        |
| Tempo knob                | A8           |
| Swing knob                | A9           |
| slider_mode_select        | A13 / pin 67 |
| voice_mode_select         | A12 / pin 66 |
| knob_mode_select          | A14 / pin 68 |
| Pattern LEDs              | 14–17        |
| Pattern buttons           | 18–21        |
| Step LEDs                 | 22–52 even   |
| Step buttons              | 23–53 odd    |
| LCD                       | Serial1 (pin 1 TX) |

### Config Menu Navigation
- Double-click `knob_mode_select` (pin 68) to enter/exit config menu
- Tempo knob (A8): jog-wheel up/down — track delta from saved position on menu entry;
  crossing threshold fires scroll event; update saved position after each step
- Swing knob (A9): jog-wheel for value adjustment (left = decrease, right = increase)
- param_rec / D11: Enter / confirm selection

### Data Model
- step_data[16][8][16]    — [pattern][voice][step] on/off
- voice_pitch[16][8]      — per-voice pitch per pattern (NN slider)
- voice_velocity[16][8]   — per-voice velocity per pattern (VL slider = live volume fader)
- voice_gate[16][8]       — per-voice gate length per pattern (GT slider)
- voice_cc_value[16][8]   — per-voice CC value per pattern (CC slider)
- voice_cc_enabled[8]     — per-voice CC on/off flag
- cc_number[16]           — per-pattern CC number (shared across voices)
- current_voice           — which voice step buttons / slider edits (0–7)
- sounding_notes[8]       — indexed by voice (not step)
- sounding_note_end_step[8] — indexed by voice

### Voice Velocity as Live Volume Fader
Per-voice velocity (not per-step). In VL mode, sliding a voice's fader to minimum
effectively mutes that voice in real time — great for live jamming. This matches the
original Beatseqr live-use intent.
