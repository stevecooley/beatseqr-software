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

  // Restore saved state. On first boot the magic byte won't match and
  // globals keep their defaults. After a save the last working session
  // is fully restored: patterns, pitches, tempo, swing, channel, clock mode.
  load_from_eeprom();
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

  if (enterbutton.uniquePress()) {
    Serial.println("listening for enter button events");
    enterbutton_flag = true;
  }

  if (enterbutton.uniquePress()) {
    Serial.println("listening for enter button events");
    enterbutton_flag = true;
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

  listen_for_navigation_events();

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

    // pattern copy
    if (pattern_select_buttons[i].heldFor(2000)) {
      // Serial.print(last_serial);
      told_which_pattern_to_copy_to =
          true;       // this is us being told to copy the pattern
      lcdflag = 100;  next_lcdflag = 100;  // pattern copy
    }
  }

  run_pattern_select_routine();

  run_voice_slider_routine();

  run_LCD_update();
}
