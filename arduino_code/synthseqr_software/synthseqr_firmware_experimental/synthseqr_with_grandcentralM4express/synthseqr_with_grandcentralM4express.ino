template <class T>
inline Print& operator<<(Print& obj, T arg) {
  obj.print(arg);
  return obj;
}

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
  lcd.begin(9850);

  delay(500);
  run_LCD_setup_routine();
  go_to_pattern(0, 1);

  // start sequencer and set callbacks
  seq.begin(TEMPO, 16);
  seq.stop();
  seq.setMidiHandler(midi);
  seq.setStepHandler(stepsend);
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

  if (playbutton.uniquePress()) {
    Serial.println("listening for play button events");
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
  for (uint8_t i = 0; i < 4; i++)
  {
    if (pattern_select_buttons[i].uniquePress()) {
      pattern_select_button_flags[i] = true;
    }

    // pattern copy
    if (pattern_select_buttons[i].heldFor(2000)) {
      // Serial.print(last_serial);
      told_which_pattern_to_copy_to =
          false;      // this is us being told to copy the pattern
      lcdflag = 100;  // pattern copy
    }

  }

 

    run_pattern_select_routine();

    run_voice_slider_routine();

    run_LCD_update();
  }
