

// #include "Button.h"
#include "LED.h"
#include "Potentiometer.h"
#include "PString.h"
#include "ClickButton.h"

const char* firmware_version_number = "4.5x";
const char* hardware_version_number = "4.51";


// Important configuration stuff

int swingknob = 9;
int tempoknob = 8;

int upper_BPM_number = 201;
int lower_BPM_number = 89;


// this is a configuration point to set up the resistors on the analog 10 pin for voice select buttons
int vselectval_lowerranges[8] = {
  920,
  830,
  751,
  700,
  650,
  611,
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
  610,
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


const int numReadings = 5;
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

// the Play/stop Button
const int transport_button_pin = 65;
ClickButton transport_button(transport_button_pin, LOW, CLICKBTN_PULLUP);

// the Play/stop Button
const int voice_mode_pin = 66;
ClickButton voice_mode_select(voice_mode_pin, LOW, CLICKBTN_PULLUP);

// the Play/stop Button
const int slider_mode_pin = 67;
ClickButton slider_mode_select(slider_mode_pin, LOW, CLICKBTN_PULLUP);

// the Play/stop Button
const int knob_mode_pin = 68;
ClickButton knob_mode_select(knob_mode_pin, LOW, CLICKBTN_PULLUP);

// the Play/stop Button
const int param_rec_pin = 11;
ClickButton param_rec(param_rec_pin, LOW, CLICKBTN_PULLUP);

// play / stop
LED transport_led_1 = LED(10);

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

/*
Button step_buttons[16] = { 
  Button(23,BUTTON_PULLUP),Button(25,BUTTON_PULLUP),Button(27,BUTTON_PULLUP),Button(29,BUTTON_PULLUP),
  Button(31,BUTTON_PULLUP),Button(33,BUTTON_PULLUP),Button(35,BUTTON_PULLUP),Button(37,BUTTON_PULLUP),
  Button(39,BUTTON_PULLUP),Button(41,BUTTON_PULLUP),Button(43,BUTTON_PULLUP),Button(45,BUTTON_PULLUP),
  Button(47,BUTTON_PULLUP),Button(49,BUTTON_PULLUP),Button(51,BUTTON_PULLUP),Button(53,BUTTON_PULLUP)}
;
*/
const int number_of_step_buttons = 16;

const int buttonPin1 = 23;
const int buttonPin2 = 25;
const int buttonPin3 = 27;
const int buttonPin4 = 29;
const int buttonPin5 = 31;
const int buttonPin6 = 33;
const int buttonPin7 = 35;
const int buttonPin8 = 37;
const int buttonPin9 = 39;
const int buttonPin10 = 41;
const int buttonPin11 = 43;
const int buttonPin12 = 45;
const int buttonPin13 = 47;
const int buttonPin14 = 49;
const int buttonPin15 = 51;
const int buttonPin16 = 53;

// Instantiate ClickButton objects in an array
ClickButton step_buttons[number_of_step_buttons] = {
  ClickButton (buttonPin1, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin2, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin3, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin4, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin5, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin6, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin7, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin8, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin9, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin10, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin11, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin12, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin13, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin14, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin15, LOW, CLICKBTN_PULLUP),
  ClickButton (buttonPin16, LOW, CLICKBTN_PULLUP),
};


int pattern_value;

int step_data[4][8][16];
int step_value;

int flam_data[4][8][16];
int flam_value;


//////////////////////////////////
//
// Pattern select buttons and LEDS
//
//////////////////////////////////

LED pattern_select_leds[4] = {
  LED(17),LED(16),LED(15),LED(14)};
int pattern_select_button_pressing_counter = 0;

const int pattern1button_pin = 21;
const int pattern2button_pin = 20;
const int pattern3button_pin = 19;
const int pattern4button_pin = 18;

int copyfrompattern = -1;
int copytopattern = -1;

ClickButton  pattern_select_buttons[4] = {
  ClickButton (pattern1button_pin, LOW, CLICKBTN_PULLUP),
  ClickButton (pattern2button_pin, LOW, CLICKBTN_PULLUP),
  ClickButton (pattern3button_pin, LOW, CLICKBTN_PULLUP),
  ClickButton (pattern4button_pin, LOW, CLICKBTN_PULLUP),
};

// Button pattern_select_buttons[4] = { 
//   Button(21, BUTTON_PULLUP),Button(20, BUTTON_PULLUP),Button(19, BUTTON_PULLUP),Button(18, BUTTON_PULLUP)};

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


