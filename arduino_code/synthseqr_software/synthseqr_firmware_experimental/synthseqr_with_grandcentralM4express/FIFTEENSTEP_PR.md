# FifteenStep Upstream PR

**Status: PR submitted to https://github.com/adafruit/FifteenStep — 2026-05-07**

---

## PR Title

`Improve timing precision, add float tempo, getters, direct shuffle setter, and hardware timer ISR mode`

---

## PR Body

### Summary

This PR brings several improvements developed and battle-tested in the
[Synthseqr](https://github.com/stevecooley/beatseqr-software) hardware MIDI
sequencer project (Adafruit Grand Central M4 Express / ATSAMD51). Each change
is independent and described in detail below.

---

### 1. Float tempo — `setTempo(float)`, `begin(float, ...)`, `getTempo()`

**What changed:** `_tempo` promoted from `int` to `float`. All `begin()` and
`setTempo()` overloads updated to accept `float`. New `getTempo()` getter added.
`increaseTempo()` / `decreaseTempo()` now step by `0.1` instead of `5`.

**Why:** Integer BPM resolution is too coarse for musical use — the difference
between 119 and 120 BPM is perceptible. Sub-BPM precision (e.g. 119.8, 120.0,
120.2) allows fine-grained tempo nudging and matches the resolution expected by
DAWs and hardware sync sources. The `0.1` step size in `increaseTempo()` /
`decreaseTempo()` keeps those helpers useful without jumping a full BPM at a
time.

---

### 2. `micros()` timing in software polling path

**What changed:** The software-polling `run()` path switched from `millis()` to
`micros()` for all timing comparisons and advances. All internal timing values
(`_sixteenth`, `_clock`, `_beatlength`) computed in microseconds. New private
member `_beatlength` (µs per quarter note) derived from tempo; `_sixteenth` and
`_clock` derive from it.

**Why:** `millis()` has 1 ms resolution, which at 120 BPM represents ≈0.5% of a
sixteenth note (~2 ms). That quantisation error is audible as jitter and
accumulates over time. `micros()` has 1 µs resolution — roughly 500× finer —
and eliminates the perceptible jitter entirely.

---

### 3. Drift-free scheduling: `+=` instead of `= now +`

**What changed:** `run()` advances `_next_beat` and `_next_clock` with `+=`
(relative to the *scheduled* time) rather than `= now +` (relative to the
*actual* time the check ran).

**Why:** The original code re-anchors the schedule to the real clock on every
step. If the main loop is slow (e.g. due to USB, Serial, or other work), the
extra delay gets baked into the next step's duration. Over many bars this
accumulates into noticeable drift. With `+=`, late processing is absorbed as a
one-off micro-skip; the long-term average tempo stays correct.

```cpp
// Before (drifts):
_next_beat = now + _sixteenth + _shuffle;

// After (drift-free):
_next_beat += _sixteenth + _shuffle;
```

---

### 4. Hardware timer ISR mode — `setHardwareTimerMode()` + `hardwareClockPulse()`

**What changed:** Two new public methods:

```cpp
void setHardwareTimerMode(bool enabled);
void hardwareClockPulse();          // ISR-safe
```

Three new private members: `volatile bool _hw_clock_pending`,
`volatile bool _hw_step_pending`, `volatile uint8_t _hw_pulse_count`.

`run()` gains a fast code path: when `_hw_timer_mode` is true it processes the
two volatile flags and returns immediately, skipping all `micros()` polling.

**Why:** The `micros()` polling path still has jitter proportional to main-loop
latency. When a hardware peripheral timer (e.g. SAMD51 TC4) is available, it
can fire at *exactly* the right moment regardless of main-loop timing. However,
the timer ISR must not call USB or allocate memory, so the pattern is:

- ISR calls `hardwareClockPulse()` — touches only volatile flags, ~10 CPU
  cycles, fully ISR-safe.
- `run()` in main-loop context reads the flags and does all real work (MIDI,
  callbacks) there.

Every 6 pulses (= 24 PPQN / 4 sixteenth notes per beat) `hardwareClockPulse()`
also sets `_hw_step_pending`, advancing the sequencer one step. This gives
sample-accurate step timing limited only by the timer's precision, not by how
fast the main loop runs.

Usage:

```cpp
// In setup():
seq.setHardwareTimerMode(true);
// ... configure your hardware timer to fire at the MIDI clock rate ...

// In the timer ISR:
void TC4_Handler() {
  TC4->COUNT16.INTFLAG.bit.MC0 = 1;  // clear flag
  seq.hardwareClockPulse();           // sets volatile flags only
}

// In loop() — unchanged:
seq.run();
```

---

### 5. `start(int position = -1)` — optional start position + timer seeding

**What changed:** `start()` gains an optional `position` parameter (default
`-1`, which wraps to 255 as a `byte`, so the first `_step()` call increments to
0 — same net effect as before). In hardware timer mode, `start()` now resets
`_hw_pulse_count` and pre-arms the step and clock pending flags so step 0 and
the first clock fire on the very next `run()` call. In software mode, `start()`
seeds `_next_beat` and `_next_clock` to `micros()` so the drift-free `+=`
scheduling begins from a correct baseline.

**Why:** Without seeding the software timers in `start()`, the first step fires
immediately because `_next_beat` was 0 from `_init()`, and then all subsequent
steps drift against the elapsed time since power-on. Seeding from `micros()` in
`start()` gives a clean baseline.

---

### 6. `setShuffle(uint8_t swing)` — direct shuffle setter

**What changed:** New `setShuffle(uint8_t swing)` sets the shuffle amount
directly as a multiple of `_shuffleDivision()`, rather than requiring repeated
`increaseShuffle()` / `decreaseShuffle()` calls to reach the desired value.

**Why:** When loading a saved preset or restoring state after a reset, you want
to set swing to a specific value in one call. Calling `increaseShuffle()` N
times is fragile and order-dependent.

---

### 7. `getShuffle()` and `getbeatlength()` getters

**What changed:** `getShuffle()` returns `_shuffle` (the current shuffle offset
in µs). `getbeatlength()` returns `_beatlength` (µs per quarter note).

**Why:** Needed for external code that adjusts timing — e.g. a hardware timer
driver that needs to know the exact beat duration to program its compare
register, or swing logic that defers a step by a fraction of the beat interval.

---

### 8. `_shuffleDivision()` divisor changed from 16 to 6

**What changed:** `_shuffleDivision()` now returns `_sixteenth / 6` instead of
`_sixteenth / 16`.

**Why:** There are exactly 6 MIDI clock pulses per sixteenth note (24 PPQN / 4
sixteenth notes per beat). Using 6 as the divisor means each shuffle increment
corresponds to exactly one MIDI clock pulse of delay — the smallest perceptible
swing unit when synced to an external clock. The original divisor of 16 produced
increments that didn't align with MIDI clock boundaries and gave finer steps
than the clock could represent.

---

### Notes on what is NOT included

One local change in the Synthseqr fork (`_tick()` sending channel `0xFF`
instead of `0x0`) is intentionally excluded from this PR. That change is a
project-specific sentinel used to distinguish clock ticks from note messages
inside a single MIDI callback; it would break existing users who rely on the
channel argument being `0x0`.

---

### Testing

All changes tested on Adafruit Grand Central M4 Express (ATSAMD51J19A) running
the Synthseqr firmware at tempos 10–250 BPM with SWING 0–5, pattern lengths
1–16, and both internal (hardware timer) and external (USB-MIDI 0xF8) clock
sources.
