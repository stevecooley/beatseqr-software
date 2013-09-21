/*
  Analog Input
 
 2011-11-23 for synthseqr by Steve Cooley
 
 */


#include "Potentiometer.h"

////////////////////////////////
//
// sliders
//
////////////////////////////////

Potentiometer voice_sliders[16] = { 
  Potentiometer(15),Potentiometer(14),Potentiometer(13),Potentiometer(12),Potentiometer(11),Potentiometer(10),Potentiometer(9),Potentiometer(8), Potentiometer(7),Potentiometer(6),Potentiometer(5),Potentiometer(4),Potentiometer(2),Potentiometer(3),Potentiometer(1),Potentiometer(0)};

int voice_slider_values[16]; 

int raw_voice_slider_values[16];


int CC_cleared_to_update_values[16];
int NN_cleared_to_update_values[16];
int VL_cleared_to_update_values[16];
int MC_cleared_to_update_values[16];

int voice_slider_midivelocity[16];
int voice_slider_midicc[16];
int voice_slider_midinotenum[16] = {
  36,37,38,39,40,41,42,43,44,45,46,47,48,50,49,51};
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



void setup() {
  Serial.begin(57600);  
}

void loop() {

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

