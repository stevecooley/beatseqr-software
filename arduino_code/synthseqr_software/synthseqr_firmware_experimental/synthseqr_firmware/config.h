  
#include "Button.h"
#include "LED.h"
#include "Potentiometer.h"
#include "PString.h"

const char* firmware_version_number = "1.0x";
const char* hardware_version_number = "2.0";

int last_voice_selected = 0;


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

int midinn_sliderrangelow = -1;
int midinn_sliderrangehigh = -1;

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
// transport, dpad, and Pattern select buttons and LEDS
//
//////////////////////////////////
  // patter 1 15, led 4
  // pattern 2 14, led 5
  // pattern 3 6, led 7
  // pattern 4 8, led 9

  // right 16
  // left 17 button 17, 
  // down 18
  // up 19
  

Button playbutton = Button(21,PULLUP);
LED playbutton_LED = LED(2);

int isRecordingArmed = 0;

Button dpad_up = Button(19,PULLUP);
Button dpad_right = Button(16,PULLUP);
Button dpad_down = Button(18,PULLUP);
Button dpad_left = Button(17,PULLUP);


Button enterbutton = Button(20,PULLUP);
LED enterbutton_LED = LED(3);



LED pattern_select_leds[4] = {


 LED(4),
 LED(5),
 LED(7),
 LED(9)
 };
 
int pattern_select_button_pressing_counter = 0;

Button pattern_select_buttons[4] = { 

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


//////////////////////////////////
//
// LCD
//
//////////////////////////////////

#define rxPin 255  // rxPin is immaterial - not used - just make this an unused Arduino pin number
#define lcdTxPin 10 // analog in - pin 15
SoftwareSerial lcd =  SoftwareSerial(rxPin, lcdTxPin);

char N;
int I;
int ByteVar;

int NN;
int Remainder;
int Num_5;


//////////////////////////////////
//
// Serial data setup
//
//////////////////////////////////

// keepin' track of time
unsigned long now;
unsigned long last_step_time;
unsigned long next_step_time;

int play_started_at;
int chase_lights_status = 0;
int step_counter = 1;

// outgoing serial data
char buffer[30];
char last_buffer[30];
char copy_patter_return_message[30];
PString the_serial_message(buffer, sizeof(buffer));
PString last_serial(last_buffer, sizeof(last_buffer));
const char* last_last_message;

// incoming serial data
int inByte;
int lastInByte;
int current_step;


//////////////////////////////////
//
// voice modes.. um.. not sure what we're doing here yet
//
//////////////////////////////////

int voice_mode = 1;
int voice_mode_total = 3;

int voice_mode_pulse_counter = 1;
int pulse_adder = 1;
