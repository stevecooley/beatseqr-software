template<class T> inline Print &operator <<(Print &obj, T arg) { 
  obj.print(arg); 
  return obj; 
} 

/*
 synthseqr arduino firmware
 2011 steve cooley 
 license: essentially creative commons non-commercial share-alike attribution
 Modify this code as you see fit, don't sell it, share improvements you've made so that we can build on what I've built on, and don't remove my original credit
 
 http://beatseqr.com for the latest news
 http://github.com/stevecooley/beatseqr-software for the latest versions of related software
 */
#include <SoftwareSerial.h>
#include <EEPROM.h>

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
  go_to_pattern(0,1);

}


void loop()
{
  // chase lights are when the step buttons tell you what step you're on when the transport is playing

  process_incoming_serial();

  if(chase_lights_status == 1)
  {

    now = millis();  

    run_chase_lights(now);
  }

  run_step_button_routine();

  run_transport_button_routine();  

  run_diagnostics();

  run_pattern_select_routine();

  run_voice_slider_routine();

}



