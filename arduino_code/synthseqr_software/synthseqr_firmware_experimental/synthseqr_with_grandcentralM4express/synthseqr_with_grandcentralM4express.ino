template <class T>
inline Print& operator<<(Print& obj, T arg) {
  obj.print(arg);
  return obj;
}

// template <typename T>
/*
long Round(T x) {
  return (x >= 0) ? (long)(x + 0.5) : (long)(x - 0.5);
}
#define round(x) Round(x)  // for backwards compatibility
*/

/*
  synthseqr arduino firmware
  2011 steve cooley
  2020 steve cooley, my how time flies
  license: public domain

  http://beatseqr.com for the latest news
  http://github.com/stevecooley/beatseqr-software for the latest versions of
  related software
*/

#include "config.h"

/*
  Hardware Abstraction Resource
  Author:  Alexander Brevig
  Contact: alexanderbrevig@gmail.com

  note: Alexander is awesome. this project wouldn't have taken off as fast as it
  has without his awesome code

*/

void setup() {
  Serial.begin(57600);
  analogReadResolution(12);  // SAMD51 native 12-bit ADC (0-4095); default is 10-bit
  // Initialize Serial1 (LCD TX) immediately so the TX line idles HIGH
  // during the startup delay. Without this, pin 1 floats or sits LOW,
  // which the LCD can misinterpret as a break or garbage before init.
  lcd.begin(9850);
  init_trs_midi();

  delay(500);
  run_LCD_setup_routine();

  // start sequencer and set callbacks
  seq.begin(TEMPO, 16);
  seq.stop();
  seq.setMidiHandler(midi);
  seq.setStepHandler(stepsend);

  // Default all captured chord slots to -1 (empty) before load_from_*.
  // Loaders only overwrite slots that were explicitly saved, so any pattern
  // saved before this feature existed stays in the "no captured chord" state.
  for (int p = 0; p < 16; p++)
    for (int s = 0; s < 16; s++)
      for (int n = 0; n < MAX_CHORD_NOTES; n++)
        step_custom_chord[p][s][n] = -1;
  for (int b = 0; b < 16; b++) midi_capture_held[b] = 0;

  // Restore saved state. SD is tried first (all 16 patterns + settings);
  // falls back to EEPROM (4 patterns) if no card or no autosave found.
  boot_load();
  // Seed slider_last_raw[] from current physical positions so Relative-mode
  // takeover doesn't see a phantom delta against a default-zero baseline.
  for (int i = 0; i < 16; i++) slider_last_raw[i] = voice_sliders[i].getValue();
  // Apply loaded pattern_length to FifteenStep (default is 16; may differ after load).
  seq.setSteps(pattern_length);
  // Apply loaded clock-divide setting to both clock paths.
  apply_clock_div();
  // If shuffle mode was saved, prime the permutation array for pattern_length.
  if (pattern_direction == 4) init_shuffle();
  go_to_pattern(current_pattern, 1);
  seq.setTempo(TEMPO);
  update_line2 = true;  // ensure swing/clock/channel redraw with loaded values

  // Hardware interrupt for play button — fires immediately on press (falling
  // edge), independent of loop() timing. Debounce is handled in the ISR.
  attachInterrupt(digitalPinToInterrupt(21), playButtonISR, FALLING);

  // Enable hardware timer mode and start TC4.
  // The timer fires at the MIDI clock rate (24x per quarter note) regardless
  // of play state; seq.run() only processes the resulting flags when playing.
  seq.setHardwareTimerMode(true);
  setupSequencerTimer(60000000UL / (unsigned long)TEMPO / 24UL);
  // If external clock mode was saved, stop the internal timer now.
  if (external_clock_mode) stopSequencerTimer();
}

void loop() {
  // this is needed to keep the sequencer
  // running. there are other methods for
  // start, stop, and pausing the steps

  // Fire any swing-delayed external clock pulse. Must run before seq.run()
  // so the deferred hardwareClockPulse is processed in the same frame.
  if (ft_swing && ext_swing_pulse_pending && (long)(micros() - ext_swing_pulse_fire_us) >= 0) {
    ext_swing_pulse_pending = false;
    seq.hardwareClockPulse();
  }

  seq.run();

  read_midi();

  // Diagnostics mode: polls all inputs directly and writes to LCD.
  // When active, all normal subsystems are skipped via the early return below.
#if FEATURE_DIAGNOSTICS
  run_diagnostics();
  if (diag_mode) return;
#endif

  // Consistency guard: nav mode is only valid in advanced mode.
  if (!advanced_mode && adv_pat_nav_active) {
    adv_pat_nav_active = false;
    adv_chain_hold_step = -1;
    read_step_memory(0, pattern_value);
  }

  if (dpad_left.uniquePress()) {
    Serial.println("listening for nav-left events");
    dpad_left_flag = true;
  }

  if (dpad_up.uniquePress()) {
    Serial.println("listening for nav-up events");
    dpad_up_flag = true;
  }

  if (dpad_down.uniquePress()) {
    Serial.println("listening for nav-down events");
    dpad_down_flag = true;
  }

  // Double-tap Enter (two presses within 400 ms) enters the config menu.
  // The single-tap action is deferred until the 400 ms window expires so the
  // first tap of a double-tap never accidentally triggers a slider mode change.
  // While the config menu is already active the double-tap machinery is bypassed
  // entirely: every press goes directly and immediately to enterbutton_flag so
  // the menu responds without the 400 ms lag and so double-taps inside the menu
  // can never accidentally call enter_config_menu() and swallow both presses.
  {
    static unsigned long last_enter_ms = 0;
    static bool enter_tap_pending = false;
    unsigned long now_ms = millis();

    if (config_menu_active) {
      // Menu is open: route Enter directly, no double-tap detection.
      if (enterbutton.uniquePress()) {
        enterbutton_flag = true;
        Serial.println("enter button pressed (menu)");
      }
      // Discard any pending window state so it can't misfire after menu closes.
      enter_tap_pending = false;
      last_enter_ms = 0;
    } else {
      if (enterbutton.uniquePress()) {
        if (enter_tap_pending && now_ms - last_enter_ms <= 400) {
          // Second tap within window — confirmed double-tap.
          enter_tap_pending = false;
          last_enter_ms = 0;
          enter_config_menu();
        } else {
          // First tap — start the window; don't fire single-tap yet.
          enter_tap_pending = true;
          last_enter_ms = now_ms;
        }
      }

      // Window expired without a second tap — fire as single tap now.
      if (enter_tap_pending && now_ms - last_enter_ms > 400) {
        enter_tap_pending = false;
        last_enter_ms = 0;
        enterbutton_flag = true;
        Serial.println("enter button pressed");
      }
    }
  }

  // Keep Button state current for heldFor().
  playbutton.isPressed();
  // Use the hardware interrupt flag for play press detection — much more
  // responsive than polling uniquePress() through the main loop.
  if (play_button_isr_fired) {
    play_button_isr_fired = false;
    Serial.println("play button interrupt fired");
    playbutton_flag = true;
  }

  // Config menu is modal — when active it consumes all d-pad/enter flags and
  // normal navigation is suppressed.
  run_config_menu();

  if (!config_menu_active) {
    listen_for_navigation_events();
  }

  listen_for_transport_events();
  // process_incoming_serial();

  // chase lights are high when the step buttons tell you what step you're on
  // when the transport is playing

  run_step_button_routine();

  // Pattern buttons are modal to the config menu — a stray press while the
  // menu is open must NOT leak into advanced-mode single/double-click state.
  // Without this guard, a uniquePress() during save-and-exit can arm the
  // nav-mode timer, which then fires ~400 ms later and makes it look like
  // saving the file flipped the device into advanced mode.
  if (!config_menu_active) {
    for (uint8_t i = 0; i < 4; i++) {
      if (pattern_select_buttons[i].uniquePress()) {
        pattern_select_button_flags[i] = true;
      }

      // pattern copy — simple mode only; advanced mode uses holds for function keys
      if (!advanced_mode && pattern_select_buttons[i].heldFor(2000)) {
        told_which_pattern_to_copy_to =
            true;       // this is us being told to copy the pattern
        lcdflag = 100;  next_lcdflag = 100;  // pattern copy
      }
    }
  }

  // Advanced mode: pattern button 0 single/double-click detection.
  //   Single click (400 ms timeout with no second press) → toggle pattern-nav mode.
  //   Double click (second press within 400 ms) → enter pattern copy mode:
  //     Phase 1 (adv_copy_waiting_source): tap a step to pick the source pattern.
  //     Phase 2 (adv_copy_armed):          tap a step to pick the destination.
  if (advanced_mode && !config_menu_active) {
    unsigned long now_ms = millis();

    if (pattern_select_button_flags[0]) {
      pattern_select_button_flags[0] = false;
      if (adv_pat0_press_ms != 0 && now_ms - adv_pat0_press_ms <= 400) {
        // Double-click confirmed — enter copy mode, pick source first.
        adv_pat0_press_ms = 0;
        adv_copy_waiting_source = true;
        adv_copy_armed = false;
        adv_pat_nav_active = true;
        adv_chain_hold_step = -1;
        adv_blink_state = true;
        adv_blink_last_ms = millis();
        lcdflag = 103;  next_lcdflag = 103;  update_line1 = true;  // "copy which pat?"
        Serial.println("copy mode: pick source pattern");
      } else {
        // First press — record timestamp; wait to see if second press comes.
        adv_pat0_press_ms = now_ms;
      }
    }

    // Single-click: 400 ms elapsed without a second press → toggle nav mode.
    if (adv_pat0_press_ms != 0 && now_ms - adv_pat0_press_ms > 400) {
      adv_pat0_press_ms = 0;
      adv_pat_nav_active = !adv_pat_nav_active;
      if (adv_pat_nav_active) {
        adv_chain_hold_step = -1;
        adv_blink_state = true;
        adv_blink_last_ms = millis();
        Serial.println("pattern nav mode ON");
      } else {
        adv_chain_hold_step = -1;
        extended_step_length_mode = 0;  // exit chain mode when leaving nav
        read_step_memory(0, pattern_value);
        Serial.println("pattern nav mode OFF");
      }
    }
  }

  run_pattern_select_routine();

  run_voice_slider_routine();

  run_LCD_update();
}
