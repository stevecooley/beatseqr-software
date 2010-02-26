template<class T> inline Print &operator <<(Print &obj, T arg) { 
  obj.print(arg); 
  return obj; 
} 

/*

 http://beatseqr.com for the latest and greatest....
 
 beatseqr version 1.0
 2009-08-25 Steve Cooley
 2009-09-02 Steve Cooley - broken out into separate tabs... I know this can look a little intimidating, but I think this will help you figure out what does what and where it is.
 
 beatseqr version 2.0
 2009-10-30 Steve Cooley - modified for use, exclusively, with ubirox instead of Roxor. Roxor is now deprecated.  Sniff.
 
 beatseqr version 3.2
 2009-12-23 Steve Cooley - welp, I changed my mind and we're using ubirox, but I've rebranded it as roxor 2.0.  
 2009-12-23 Steve Cooley - There are a few changes in this release, everything is just getting tightened up so the controls react better and do what you expect. 
 
 Beatseqr version 3.3
 2010-02-17 Steve Cooley - experimental code to light up the voice select LEDs as the current_step is advancing to visually show what voices are triggering.
 2010-02-17 Steve Cooley - Added a build number to the startup screen on the LCD.
 
 */

#include <SoftwareSerial.h>
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









