// sequencer_timer.ino
//
// Hardware timer driver for precise sequencer timing on the
// Adafruit Grand Central M4 (ATSAMD51J19A, 120 MHz Cortex-M4).
//
// Uses TC4 in 16-bit Match Frequency (MFRQ) mode driven by GCLK0 (120 MHz)
// with a /1024 prescaler → 117,187.5 ticks/sec (8.533 µs/tick).
//
// The timer fires at the MIDI clock rate (24 pulses per quarter note).
// The ISR calls seq.hardwareClockPulse() which sets two volatile flags:
//   _hw_clock_pending — main loop sends one MIDI clock byte
//   _hw_step_pending  — main loop advances the sequencer one step
//                       (set every 6 clock pulses = one 16th note)
//
// All USB-MIDI operations stay in main-loop context; the ISR only sets
// flags, making it safe and extremely short (~10 CPU cycles).
//
// Timer period range with /1024 prescaler:
//   10 BPM  → 250,000 µs per clock → 29,297 ticks  (fits 16-bit ✓)
//   250 BPM →  10,000 µs per clock →  1,172 ticks  (fits 16-bit ✓)
//
// If TC4 conflicts with an analogWrite() or tone() pin on your hardware,
// try TC3 or TC5 (update TC4 → TCx and TC4_IRQn → TCx_IRQn throughout).

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Convert a MIDI clock interval (µs) to a 16-bit TC compare value.
// Formula: ticks = interval_µs × (120,000,000 / 1,024) / 1,000,000
//                = interval_µs × 15 / 128   (exact integer approximation)
// MFRQ period = (CC[0] + 1) ticks, so we subtract 1.
static uint16_t clockUsToTicks(unsigned long interval_us)
{
  uint32_t ticks = ((uint32_t)interval_us * 15UL) / 128UL;
  if (ticks < 2)   ticks = 2;      // floor: avoid zero/one-tick glitch
  if (ticks > 65535) ticks = 65535; // ceil: slowest possible tempo
  return (uint16_t)(ticks - 1);     // MFRQ: period = CC[0]+1 ticks
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

// setupSequencerTimer(clock_interval_us)
//
// Configure TC4 and start firing the ISR at the given MIDI clock interval.
// Call once from setup(), after seq.setHardwareTimerMode(true).
//
void setupSequencerTimer(unsigned long clock_interval_us)
{
  // 1. Route GCLK0 (120 MHz) to TC4/TC5 peripheral channel.
  GCLK->PCHCTRL[TC4_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0;
  while (!GCLK->PCHCTRL[TC4_GCLK_ID].bit.CHEN);

  // 2. Enable APB-D clock for TC4.
  // The CMSIS-Atmel 1.2.2 header used by Adafruit SAMD 1.7.17 omits the
  // MCLK_APBDMASK_TC4 macro (it defines TCC4 but not TC4). Per the SAMD51P20A
  // datasheet Table 14-8: TCC4 = bit 4, TC4 = bit 5 of APBDMASK.
  MCLK->APBDMASK.reg |= (1ul << 5); // TC4: APBDMASK bit 5

  // 3. Software-reset TC4 and wait for sync.
  TC4->COUNT16.CTRLA.bit.SWRST = 1;
  while (TC4->COUNT16.SYNCBUSY.bit.SWRST);

  // 4. Configure: 16-bit counter, prescaler /1024.
  TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16
                          | TC_CTRLA_PRESCALER_DIV1024;

  // 5. Match Frequency mode: counter resets to 0 on CC[0] match.
  // The WAVE register is an APB-side register with no peripheral sync bit —
  // the sync wait is omitted intentionally.
  TC4->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;

  // 6. Set compare value (timer period).
  TC4->COUNT16.CC[0].reg = clockUsToTicks(clock_interval_us);
  while (TC4->COUNT16.SYNCBUSY.bit.CC0);

  // 7. Enable compare-match interrupt on channel 0.
  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC0;

  // 8. Highest NVIC priority so the ISR is never delayed by other interrupts.
  NVIC_SetPriority(TC4_IRQn, 0);
  NVIC_EnableIRQ(TC4_IRQn);

  // 9. Enable the timer.
  TC4->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);
}

// setSequencerTimerPeriod(clock_interval_us)
//
// Update the timer period on a tempo change without stopping the timer.
// Uses the double-buffered CCBUF register so the new period takes effect
// at the next counter reset — no glitch or skipped pulse.
// Call this immediately after seq.setTempo() whenever tempo changes.
//
void setSequencerTimerPeriod(unsigned long clock_interval_us)
{
  TC4->COUNT16.CCBUF[0].reg = clockUsToTicks(clock_interval_us);
  // CCBUF is latched automatically on next period boundary; no sync needed.
}

// resetSequencerTimerSync()
//
// Reset the TC4 counter to zero, synchronising the hardware clock stream
// to the moment play is pressed. Call this right after seq.start() so
// that the first timer-driven step fires exactly one 16th note later,
// cleanly aligned with the manually-triggered step 0.
//
void resetSequencerTimerSync()
{
  TC4->COUNT16.COUNT.reg = 0;
  while (TC4->COUNT16.SYNCBUSY.bit.COUNT);
}

// stopSequencerTimer()
//
// Disable TC4 and its NVIC interrupt. Call this when switching to external
// MIDI clock mode so the ISR no longer drives hardwareClockPulse().
//
void stopSequencerTimer()
{
  NVIC_DisableIRQ(TC4_IRQn);
  TC4->COUNT16.CTRLA.bit.ENABLE = 0;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);
}

// startSequencerTimer()
//
// Re-enable TC4 after stopSequencerTimer(). Call this when switching back to
// internal clock mode. Does not re-initialise the timer — call
// setupSequencerTimer() first if TC4 has never been started.
//
void startSequencerTimer()
{
  TC4->COUNT16.CTRLA.bit.ENABLE = 1;
  while (TC4->COUNT16.SYNCBUSY.bit.ENABLE);
  NVIC_EnableIRQ(TC4_IRQn);
}

// ---------------------------------------------------------------------------
// TC4 Interrupt Service Routine
// ---------------------------------------------------------------------------
//
// Kept deliberately minimal: clear the flag, call hardwareClockPulse().
// hardwareClockPulse() sets two volatile bools — that's it.
// The main loop's seq.run() reads those bools and does the real work.
//
void TC4_Handler()
{
  // Clear the MC0 interrupt flag (write-1-to-clear).
  TC4->COUNT16.INTFLAG.bit.MC0 = 1;

  // Notify the sequencer. Only touches volatile flags — ISR-safe.
  seq.hardwareClockPulse();
}
