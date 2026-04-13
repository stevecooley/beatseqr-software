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

  delay(500);
  run_LCD_setup_routine();

  // start sequencer and set callbacks
  seq.begin(TEMPO, 16);
  seq.stop();
  seq.setMidiHandler(midi);
  seq.setStepHandler(stepsend);

  // Restore saved state. SD is tried first (all 16 patterns + settings);
  // falls back to EEPROM (4 patterns) if no card or no autosave found.
  boot_load();
  // Apply loaded pattern_length to FifteenStep (default is 16; may differ after load).
  seq.setSteps(pattern_length);
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
  if (ext_swing_pulse_pending && (long)(micros() - ext_swing_pulse_fire_us) >= 0) {
    ext_swing_pulse_pending = false;
    seq.hardwareClockPulse();
  }

  seq.run();

  read_midi();

  if (dpad_left.uniquePress()) {
    Serial.println("listening for nav-left events");
    dpad_left_flag = true;
  }

  if (dpad_right.uniquePress()) {
    Serial.println("listening for nav-right events");
    dpad_right_flag = true;
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
  // Single tap sets enterbutton_flag for normal navigation handling.
  {
    static unsigned long last_enter_ms = 0;
    if (enterbutton.uniquePress()) {
      unsigned long now_ms = millis();
      if (now_ms - last_enter_ms <= 400 && last_enter_ms != 0) {
        // Double-tap detected — enter config menu, suppress the flag.
        last_enter_ms = 0;
        enter_config_menu();
      } else {
        last_enter_ms = now_ms;
        enterbutton_flag = true;
        Serial.println("enter button pressed");
      }
    }
  }

  // Keep Button state current for heldFor() (diagnostics combo).
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

  run_diagnostics();

  // listen for pattern select button presses and set flags
  for (uint8_t i = 0; i < 4; i++) {
    if (pattern_select_buttons[i].uniquePress()) {
      pattern_select_button_flags[i] = true;
    }

    // pattern copy — simple mode only; advanced mode uses holds for function keys
    if (!advanced_mode && pattern_select_buttons[i].heldFor(2000)) {
      // Serial.print(last_serial);
      told_which_pattern_to_copy_to =
          true;       // this is us being told to copy the pattern
      lcdflag = 100;  next_lcdflag = 100;  // pattern copy
    }
  }

  // Advanced mode: pattern button 0 single/double-click detection.
  //   Single click (400 ms timeout with no second press) → toggle pattern-nav mode.
  //   Double click (second press within 400 ms) → enter pattern copy mode:
  //     Phase 1 (adv_copy_waiting_source): tap a step to pick the source pattern.
  //     Phase 2 (adv_copy_armed):          tap a step to pick the destination.
  if (advanced_mode) {
    static unsigned long last_pat0_press_ms = 0;
    unsigned long now_ms = millis();

    if (pattern_select_button_flags[0]) {
      pattern_select_button_flags[0] = false;
      if (last_pat0_press_ms != 0 && now_ms - last_pat0_press_ms <= 400) {
        // Double-click confirmed — enter copy mode, pick source first.
        last_pat0_press_ms = 0;
        adv_copy_waiting_source = true;
        adv_copy_armed = false;
        lcdflag = 103;  next_lcdflag = 103;  // "copy which pat?"
        Serial.println("copy mode: pick source pattern");
      } else {
        // First press — record timestamp; wait to see if second press comes.
        last_pat0_press_ms = now_ms;
      }
    }

    // Single-click: 400 ms elapsed without a second press → toggle nav mode.
    if (last_pat0_press_ms != 0 && now_ms - last_pat0_press_ms > 400) {
      last_pat0_press_ms = 0;
      adv_pat_nav_active = !adv_pat_nav_active;
      if (adv_pat_nav_active) {
        adv_chain_hold_step = -1;
        adv_blink_state = true;
        adv_blink_last_ms = millis();
        Serial.println("pattern nav mode ON");
      } else {
        adv_chain_hold_step = -1;
        read_step_memory(0, pattern_value);
        Serial.println("pattern nav mode OFF");
      }
    }
  }

  run_pattern_select_routine();

  run_voice_slider_routine();

  run_LCD_update();
}
