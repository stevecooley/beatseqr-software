template <class T>
inline Print& operator<<(Print& obj, T arg) {
  obj.print(arg);
  return obj;
}

/*
  beatseqr arduino firmware
  2011 steve cooley (original AVR version)
  2026 steve cooley (Grand Central M4 port, based on synthseqr firmware)
  license: public domain

  http://beatseqr.com
  http://github.com/stevecooley/beatseqr-software
*/

#include "config.h"

void setup() {
  Serial.begin(57600);

  // Use 12-bit ADC globally for accurate slider, knob, and voice select reads.
  analogReadResolution(12);

  // Start Serial1 (LCD TX, pin 1) so the line idles HIGH during startup.
  lcd.begin(9850);
  delay(500);
  run_LCD_setup_routine();

  // Sequencer init
  seq.begin(TEMPO, 16);
  seq.stop();
  seq.setMidiHandler(midi);
  seq.setStepHandler(stepsend);

  // Restore saved state from SD (all 16 patterns) or fall back to EEPROM.
  boot_load();
  seq.setSteps(pattern_length);
  if (pattern_direction == 4) init_shuffle();
  go_to_pattern(current_pattern, 1);
  seq.setTempo(TEMPO);
  update_line2 = true;

  // Hardware interrupt for play button on A11 (pin 65), falling edge.
  attachInterrupt(digitalPinToInterrupt(A11), playButtonISR, FALLING);

  // Start TC4 hardware timer for internal clock.
  seq.setHardwareTimerMode(true);
  setupSequencerTimer(60000000UL / (unsigned long)TEMPO / 24UL);
  if (external_clock_mode) stopSequencerTimer();

  // Light the first voice LED so the user knows which voice is active.
  voice_select_leds[current_voice].on();

  // Seed the normal-play swing-knob jog baseline to its current position.
  anchor_swing_norm_jog();
}

void loop() {
  // Fire any swing-delayed external clock pulse before seq.run().
  if (ext_swing_pulse_pending && (long)(micros() - ext_swing_pulse_fire_us) >= 0) {
    ext_swing_pulse_pending = false;
    seq.hardwareClockPulse();
  }

  seq.run();
  read_midi();

  // Diagnostics mode: all normal subsystems skipped via early return.
  run_diagnostics();
  if (diag_mode) return;

  // Read hardware knobs (tempo / swing) every loop.
  // In config menu, knob positions are used for jog-wheel navigation instead.
  run_knob_routine();

  // Read voice select resistor ladder and update voice LEDs / step LEDs.
  run_voice_select_routine();

  // Slider mode select button — single press cycles NN → VL → GT → CC.
  if (slider_mode_select.uniquePress()) {
    slider_mode_flag = true;
  }

  // Voice mode select button — single press toggles internal / external clock mode.
  if (voice_mode_select.uniquePress()) {
    voice_mode_flag = true;
    setExternalClockMode(!external_clock_mode);
    // Show brief confirmation on LCD.
    lcd.print("?f");
    lcd.print("?x00?y0");
    if (external_clock_mode) {
      lcd.print("Clock: ext      ");
    } else {
      lcd.print("Clock: int      ");
    }
    update_line1 = true;
    update_line2 = true;
  }

  // Knob mode select button — double-click enters/exits config menu.
  {
    static unsigned long last_knob_mode_ms = 0;
    static bool knob_mode_tap_pending = false;
    unsigned long now_ms = millis();

    if (knob_mode_select.uniquePress()) {
      if (knob_mode_tap_pending && now_ms - last_knob_mode_ms <= 400) {
        knob_mode_tap_pending = false;
        last_knob_mode_ms = 0;
        if (config_menu_active) {
          exit_config_menu();
        } else {
          enter_config_menu();
        }
      } else {
        knob_mode_tap_pending = true;
        last_knob_mode_ms = now_ms;
      }
    }
    if (knob_mode_tap_pending && now_ms - last_knob_mode_ms > 400) {
      knob_mode_tap_pending = false;
      last_knob_mode_ms = 0;
      // Single tap inside the config menu = back out one level (consumed by
      // run_config_menu / submenus). Outside the menu it is still unused.
      if (config_menu_active) knob_mode_back_flag = true;
    }
  }

  // param_rec (D11) is the Enter / confirm button for the config menu.
  if (param_rec.uniquePress()) {
    param_rec_flag = true;
  }

  // Keep play button HAL state current for heldFor() (diagnostics combo).
  playbutton.isPressed();
  if (play_button_isr_fired) {
    play_button_isr_fired = false;
    playbutton_flag = true;
  }

  // Config menu is modal — consumes param_rec_flag and knob jog events.
  run_config_menu();

  if (!config_menu_active) {
    // Handle slider mode cycle outside config menu.
    if (slider_mode_flag) {
      slider_mode_flag = false;
      // Skip modes whose feature flag is off (NN is always enabled).
      set_slider_mode(next_slider_mode(slider_mode));
    }
    // Adv copy cancel and any other out-of-menu nav events.
    listen_for_navigation_events();
  }

  listen_for_transport_events();

  run_step_button_routine();

  // Pattern select button flag collection.
  for (uint8_t i = 0; i < 4; i++) {
    if (pattern_select_buttons[i].uniquePress()) {
      pattern_select_button_flags[i] = true;
    }
    // Simple mode: hold 2 s → pattern copy armed (held() fires once per press).
    if (!advanced_mode && pattern_select_buttons[i].held(2000)) {
      told_which_pattern_to_copy_to = true;
      lcdflag = 100; next_lcdflag = 100;
    }
  }

  // Advanced mode: pattern button 0 single / double-click.
  if (advanced_mode) {
    static unsigned long last_pat0_press_ms = 0;
    unsigned long now_ms = millis();

    if (pattern_select_button_flags[0]) {
      pattern_select_button_flags[0] = false;
      if (last_pat0_press_ms != 0 && now_ms - last_pat0_press_ms <= 400) {
        last_pat0_press_ms = 0;
        adv_copy_waiting_source = true;
        adv_copy_armed = false;
        lcdflag = 103; next_lcdflag = 103;
      } else {
        last_pat0_press_ms = now_ms;
      }
    }
    if (last_pat0_press_ms != 0 && now_ms - last_pat0_press_ms > 400) {
      last_pat0_press_ms = 0;
      adv_pat_nav_active = !adv_pat_nav_active;
      if (adv_pat_nav_active) {
        adv_chain_hold_step = -1;
        adv_blink_state = true;
        adv_blink_last_ms = millis();
      } else {
        adv_chain_hold_step = -1;
        read_step_memory(current_voice, pattern_value);
      }
    }
  }

  run_pattern_select_routine();

  run_voice_slider_routine();

  run_LCD_update();
}
