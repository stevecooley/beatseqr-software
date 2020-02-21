template <class T>
inline Print &operator<<(Print &obj, T arg)
{
  obj.print(arg);
  return obj;
}

/*
  synthseqr arduino firmware
  2011 steve cooley
  2020 steve cooley, my how time flies
  license: public domain

  http://beatseqr.com for the latest news
  http://github.com/stevecooley/beatseqr-software for the latest versions of related software
*/

#include "config.h"

/*
  Hardware Abstraction Resource
  Author:  Alexander Brevig
  Contact: alexanderbrevig@gmail.com

  note: Alexander is awesome. this project wouldn't have taken off as fast as it has without his awesome code

*/

void setup()
{

  Serial.begin(57600);
  lcd.begin(9850);

  // patter 1 15, led 4
  // pattern 2 14, led 5
  // pattern 3 6, led 7
  // pattern 4 8, led 9

  // right 16
  // left 17 button 17,
  // down 18
  // up 19

  // enter  button 20, led 3

  delay(500);
  run_LCD_setup_routine();
  go_to_pattern(0, 1);

  // start sequencer and set callbacks
  seq.begin(TEMPO, 16);
  seq.stop();
  seq.setMidiHandler(midi);
  seq.setStepHandler(stepsend);
}

void loop()
{

  read_midi();
  // this is needed to keep the sequencer
  // running. there are other methods for
  // start, stop, and pausing the steps
  seq.run();

  listen_for_navigation_events();
  
  listen_for_transport_events();
  // process_incoming_serial();

  // chase lights are high when the step buttons tell you what step you're on when the transport is playing

  run_step_button_routine();

  // current_step = seq.getPosition();

  run_chase_lights(seq.getPosition());

  // run_diagnostics();

  run_pattern_select_routine();

  run_voice_slider_routine();

  run_LCD_update();
}
