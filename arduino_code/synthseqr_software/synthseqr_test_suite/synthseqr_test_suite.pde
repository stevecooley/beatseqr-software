/*
 synthseqr assembly test suite
 
 2011-11-28 Steve Cooley
 
 */


#include "Button.h"
#include "LED.h"
#include "Potentiometer.h"

////////////////////////////////
//
// sliders
//
////////////////////////////////

Potentiometer voice_sliders[16] = { 
  Potentiometer(15),Potentiometer(14),Potentiometer(13),Potentiometer(12),Potentiometer(11),Potentiometer(10),Potentiometer(9),Potentiometer(8), Potentiometer(7),Potentiometer(6),Potentiometer(5),Potentiometer(4),Potentiometer(3),Potentiometer(2),Potentiometer(1),Potentiometer(0)};

int voice_slider_values[16]; 

int raw_voice_slider_values[16];


int CC_cleared_to_update_values[16];
int NN_cleared_to_update_values[16];
int VL_cleared_to_update_values[16];
int MC_cleared_to_update_values[16];

int voice_slider_midivelocity[16];
int voice_slider_midicc[16];
int voice_slider_midinotenum[16] = {
  36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
int voice_slider_midichannel[16] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int last_voice_slider_values[16];
int slider_mode = 1;
int slider_mode_total = 4;
int slider_reset_counter = 0;
const char *slider_message_header = "VL";
int slider_map_low_value = -2;
int slider_map_high_value = 127;
int slider_step_value = 2;


///////////////////////////////////
//
// Step buttons and LEDs
//
///////////////////////////////////

LED step_leds[16] = { 
  LED(22), LED(24), LED(26), LED(28), 
  LED(30), LED(32), LED(34), LED(36), 
  LED(38), LED(40), LED(42), LED(44), 
  LED(46), LED(48), LED(50), LED(52)}
; 

Button step_buttons[16] = { 
  Button(23,PULLUP),Button(25,PULLUP),Button(27,PULLUP),Button(29,PULLUP),
  Button(31,PULLUP),Button(33,PULLUP),Button(35,PULLUP),Button(37,PULLUP),
  Button(39,PULLUP),Button(41,PULLUP),Button(43,PULLUP),Button(45,PULLUP),
  Button(47,PULLUP),Button(49,PULLUP),Button(51,PULLUP),Button(53,PULLUP)}
;

int step_data[4][8][16];
int pattern_value;
int step_value;


//////////////////////////////////
//
// Pattern select buttons and LEDS
//
//////////////////////////////////

LED pattern_select_leds[6] = {
 LED(2),
 LED(3),
 LED(4),
 LED(5),
 LED(7),
 LED(9)
 };
 
int pattern_select_button_pressing_counter = 0;

Button pattern_select_buttons[6] = { 
  Button(21, PULLUP), 
  Button(20, PULLUP),
  Button(15, PULLUP),
  Button(14, PULLUP),
  Button(6, PULLUP),
  Button(8, PULLUP)
};

int extended_step_length_mode = 0;
int current_pattern = 0;
int patterns_to_play_in_a_row = 1;

char* pattern_padding;
char* step_padding;


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




}


void loop()
{


  // step buttons
  for(int i=0; i<=15; i++)
  {
    if(step_buttons[i].uniquePress()){
      step_leds[i].toggle();
      Serial.print(i);
      Serial.println(" received");


    }     
  }

  // pattern select buttons
  for(int i=0; i<=5; i++)
  {
    if(pattern_select_buttons[i].uniquePress()){
      pattern_select_leds[i].toggle();
      Serial.print(i);
      Serial.println(" received");
    }
  }
  
   // get the raw value
  for(int j = 0; j<=15; j++)
  {
    int sensorValue = map(voice_sliders[j].getValue(), 0,1023,slider_map_low_value,slider_map_high_value);


    if(sensorValue > 10) {
      Serial.print(j);
      Serial.print(" "); 
      Serial.println(sensorValue);
    }
  } 
  
}


