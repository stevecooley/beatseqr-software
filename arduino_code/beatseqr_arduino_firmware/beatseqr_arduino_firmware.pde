template<class T> inline Print &operator <<(Print &obj, T arg) { 
  obj.print(arg); 
  return obj; 
} 

/*
 beatseqr arduino firmware
 2009-2010 steve cooley 
 license: essentially creative commons non-commercial share-alike attribution
 Modify this code as you see fit, don't sell it, share improvements you've made so that we can build on what I've built on, and don't remove my original credit
 
 http://beatseqr.com for the latest news
 http://github.com/stevecooley/beatseqr-software for the latest versions of related software
 */

#include <SoftwareSerial.h>

/*
Hardware Abstraction Resource
Author:  Alexander Brevig
Contact: alexanderbrevig@gmail.com

note: Alexander is awesome. this project wouldn't have taken off as fast as it has without his awesom

*/
#include <LED.h>
#include <Button.h>
#include <Potentiometer.h>
#include <PString.h>

#include "config.h"




void setup(){

  Serial.begin(57600);

  delay(300);

  Serial << "ZIN,Steve Cooley http://beatseqr.com \r\n";
  // analog 11?
  pinMode(65, INPUT); // play stop button
  pinMode(66, INPUT); // voice button mode select?
  pinMode(67, INPUT); // Slider mode select
  pinMode(68, INPUT); // knob mode select

  pattern_select_leds[0].on();

  voice_mode_select_LED.toggle();
  slider_mode_select_LED.toggle();
  knob_mode_select_LED.toggle();

  run_LCD_setup_routine(); // in serial_utilities

  knobs_cleared_to_update[0]=0;
  knobs_cleared_to_update[1]=0;

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;     
  }
  
}

void loop(){

  // chase lights are when the step buttons tell you what step you're on when the transport is playing

  process_incoming_serial();

  if(chase_lights_status == 1)
  {

    now = millis();  

    run_chase_lights(now);
  }

  // transport button
  run_transport_button_routine();

  // voice sliders
  run_voice_slider_routine();

  // voice select buttons
  run_voice_select_button_routine();

  // step buttons
  run_step_button_routine();

  // knob functions
  run_knob_routine();

  // pattern select
  run_pattern_select_routine();

  // run the serial message out... 
  // serial_printer(the_serial_message, last_serial);
  //  last_serial = the_serial_message;
}









