
#include "Button.h"
#include "LED.h"
#include "Potentiometer.h"
#include "PString.h"

const char* firmware_version_number = "4.41x";
const char* hardware_version_number = "4.51";


// Important configuration stuff

// int swingknob = 9;
#define swingknob 9

// int tempoknob = 8;
#define tempoknob 8

int upper_BPM_number = 201;
int lower_BPM_number = 89;


// this is a configuration point to set up the resistors on the analog 10 pin for voice select buttons
int vselectval_lowerranges[8] = {
  920,
  830,
  751,
  700,
  650,
  610,
  576,
  550
};


int vselectval_upperranges[8] = {
  940,
  860,
  795,
  750,
  690,
  645,
  609,
  575
};





// end important configuration stuff


// setup for LCD
#define rxPin 1  // rxPin is immaterial - not used - just make this an unused Arduino pin number
#define lcdTxPin 69 // analog in - pin 15
SoftwareSerial lcd =  SoftwareSerial(rxPin, lcdTxPin);



char N;
int I;
int ByteVar;

int NN;
int Remainder;
int Num_5;


// setup for serial output
char buffer[30];
char last_buffer[30];
char copy_patter_return_message[30];
PString the_serial_message(buffer, sizeof(buffer));
PString last_serial(last_buffer, sizeof(last_buffer));
const char* last_last_message;

////////////////////////////////
//
// sliders
//
////////////////////////////////

Potentiometer voice_sliders[8] = { 
  Potentiometer(7),Potentiometer(6),Potentiometer(5),Potentiometer(4),Potentiometer(3),Potentiometer(2),Potentiometer(1),Potentiometer(0)};

int voice_slider_values[8]; 

int raw_voice_slider_values[8];


int CC_cleared_to_update_values[8];
int NN_cleared_to_update_values[8];
int VL_cleared_to_update_values[8];
int MC_cleared_to_update_values[8];

int voice_slider_midivelocity[8];
int voice_slider_midicc[8];
int voice_slider_midinotenum[8] = {
  36,37,38,39,40,41,42,43};
int voice_slider_midichannel[8] = {
  1,1,1,1,1,1,1,1};

int last_voice_slider_values[8];
int slider_mode = 1;
int slider_mode_total = 4;
int slider_reset_counter = 0;
const char *slider_message_header = "VL";
int slider_map_low_value = -2;
int slider_map_high_value = 127;
int slider_step_value = 2;

////////////////////////////////
//
// voice select buttons and leds
//
////////////////////////////////


LED voice_select_leds[8] = { 
  LED(2), LED(3), LED(4), LED(5), LED(6), LED(7), LED(8), LED(9)};

int voice_mode = 1;
int voice_mode_total = 3;

int voice_mode_pulse_counter = 1;
int pulse_adder = 1;

// int last_voice_selected = 0;
int mute_mode_memory[8] = {
  0,0,0,0,0,0,0,0};
int toggle_mode_memory[8] = {
  0,0,0,0,0,0,0,0};


const int numReadings = 5;      // number of readings to take when you click on a voice select button.  This number gets added up and averaged to 
int readings[numReadings];      // the readings from the voice select input
int last_reading;
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0; 


Potentiometer voice_select_buttons = Potentiometer(10);

int last_voice_selected = 0;






////////////////////////////////
// 
// knobs
//
////////////////////////////////

Potentiometer swing(swingknob);
int this_swing = 0;
int send_swing = 0;
int last_swing = 0;

Potentiometer tempo(tempoknob);
int this_tempo;
float last_tempo;

int knob_mode = 1;
int knob_mode_total = 3;


int raw_knob_values[3];
int knobs_cleared_to_update[3];
int knobs_cleared_to_update_values[3];



//int the_tempo_value = 120;
int this_BPM_is_new = 1;
int this_raw_tempo_value;
int this_mapped_tempo_value = 120;
int last_raw_tempo_value;
int last_mapped_tempo_value = 123;

int the_tempo_adjust_value;
int the_mapped_tempo_adjust_value;
int this_tempo_adjust_value;
int last_tempo_adjust_value;
int last_mapped_tempo_adjust_value;
float the_tempo_adjust_float_value;


int the_octave_adjust_value;
int the_mapped_octave_adjust_value;
int this_octave_adjust_value;
int last_octave_adjust_value;
int last_mapped_octave_adjust_value;
float the_octave_adjust_float_value;

/*
Potentiometer transport_button_1 = Potentiometer(11);
 Potentiometer transport_button_2 = Potentiometer(12);
 Potentiometer transport_button_3 = Potentiometer(13);
 */

///////////////////////////////
//
// mode select buttons and LEDs
//
///////////////////////////////


Button transport_button_1 = Button(65, PULLUP);
Button voice_mode_select = Button(66, PULLUP);
Button slider_mode_select = Button(67, PULLUP);
Button knob_mode_select = Button(68, PULLUP);
Button param_rec = Button(11, PULLUP);

int isRecordingArmed = 0;

// play / stop
LED transport_led_1 = LED(10);

// Nope, not anymore
// LED voice_mode_select_LED = LED(11);
// LED slider_mode_select_LED = LED(12);
// LED knob_mode_select_LED = LED(13);

int play_status = 0;
int play_state = 1;

float mode_select_held_for;
float mode_select_let_go_of_at;

int play_started_at;
int chase_lights_status = 0;
int step_counter = 1;

int sixty = 60;
int fifteen = 15; // 60/4

unsigned long now;
unsigned long last_step_time;
unsigned long next_step_time;

int millis_per_step;


// incoming serial data
int inByte;
int lastInByte;
int current_step;

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

LED pattern_select_leds[4] = {
  LED(17),LED(16),LED(15),LED(14)};
int pattern_select_button_pressing_counter = 0;

Button pattern_select_buttons[4] = { 
  Button(21, PULLUP),Button(20, PULLUP),Button(19, PULLUP),Button(18, PULLUP)};

int extended_step_length_mode = 0;
int current_pattern = 0;
int patterns_to_play_in_a_row = 1;

char* pattern_padding;
char* step_padding;


// custom functions

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


