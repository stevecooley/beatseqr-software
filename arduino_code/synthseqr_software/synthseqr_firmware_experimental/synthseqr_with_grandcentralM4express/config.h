#include <stdlib.h>  // because dtostrf()
#include "Button.h"
#include "FifteenStep.h"
#include "LED.h"
#include "MIDIUSB.h"
#include "PString.h"
#include "Potentiometer.h"

const char* firmware_version_number = "2.0x";
const char* hardware_version_number = "1.0";

uint8_t last_voice_selected = 0;

////////////////////////////////
//
// sliders
//
////////////////////////////////

Potentiometer voice_sliders[16] = {
    Potentiometer(A15, 255), Potentiometer(A14, 255),
    Potentiometer(A13, 255), Potentiometer(A12, 255),

    Potentiometer(A11, 255), Potentiometer(A10, 255),
    Potentiometer(A9, 255),  Potentiometer(A8, 255),

    Potentiometer(A7, 255),  Potentiometer(A6, 255),
    Potentiometer(A5, 255),  Potentiometer(A4, 255),

    Potentiometer(A2, 255),  Potentiometer(A3, 255),
    Potentiometer(A1, 255),  Potentiometer(A0, 255)};

int voice_slider_values[16];

int raw_voice_slider_values[16];

int CC_cleared_to_update_values[16];
int NN_cleared_to_update_values[16];
int VL_cleared_to_update_values[16];
int MC_cleared_to_update_values[16];

int voice_slider_midivelocity[16];
int voice_slider_midicc[16];
int voice_slider_midinotenum[16] = {36, 37, 38, 39, 40, 41, 42, 43,
                                    44, 45, 46, 47, 48, 49, 50, 51};
int voice_slider_midichannel[16] = {1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1};

int last_voice_slider_values[16];
uint8_t slider_mode = 1;
uint8_t slider_mode_total = 4;
uint8_t slider_reset_counter = 0;
const char* slider_message_header = "NN";
uint8_t slider_map_low_value = 36;
uint8_t slider_map_high_value = 52;
uint8_t slider_step_value = 2;

uint8_t midinn_sliderrangelow = -1;
uint8_t midinn_sliderrangehigh = -1;

///////////////////////////////////
//
// Step buttons and LEDs
//
///////////////////////////////////

LED step_leds[16] = {LED(22), LED(24), LED(26), LED(28), LED(30), LED(32),
                     LED(34), LED(36), LED(38), LED(40), LED(42), LED(44),
                     LED(46), LED(48), LED(50), LED(52)};

Button step_buttons[16] = {
    Button(23, PULLUP), Button(25, PULLUP), Button(27, PULLUP),
    Button(29, PULLUP), Button(31, PULLUP), Button(33, PULLUP),
    Button(35, PULLUP), Button(37, PULLUP), Button(39, PULLUP),
    Button(41, PULLUP), Button(43, PULLUP), Button(45, PULLUP),
    Button(47, PULLUP), Button(49, PULLUP), Button(51, PULLUP),
    Button(53, PULLUP)};

// step_data = 4 patterns, 1 voice, 16 steps
int step_data[4][1][16];
uint8_t pattern_value;
uint8_t step_value;

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

Button playbutton = Button(21, PULLUP);
LED playbutton_LED = LED(2);
bool transport_button_pressed = false;

uint8_t isRecordingArmed = 0;

Button dpad_up = Button(19, PULLUP);
Button dpad_right = Button(16, PULLUP);
Button dpad_down = Button(18, PULLUP);
Button dpad_left = Button(17, PULLUP);

uint8_t navmode = 100;  // default to tempo and swing adjust

Button enterbutton = Button(20);
LED enterbutton_LED = LED(3);

LED pattern_select_leds[4] = {

    LED(4), LED(5), LED(7), LED(9)};

uint8_t pattern_select_button_pressing_counter = 0;

Button pattern_select_buttons[4] = {

    Button(15, PULLUP), Button(14, PULLUP), Button(6, PULLUP),
    Button(8, PULLUP)};

uint8_t extended_step_length_mode = 0;
uint8_t current_pattern = 0;
uint8_t patterns_to_play_in_a_row = 1;

bool not_told_which_pattern_to_copy_to = true;
uint8_t copy_pattern_to;

bool not_told_which_pattern_mode_to_use = true;
uint8_t pattern_mode;

char* pattern_padding;
char* step_padding;

//////////////////////////////////
//
// LCD
//
//////////////////////////////////

#define rxPin \
  255  // rxPin is immaterial - not used - just make this an unused Arduino pin
       // number

#define lcdTxPin 1 // hardware version 1.0, hack version 2.0

#define lcd Serial1

uint8_t lcdflag = 255;
uint8_t last_lcdflag = 255;
char lcd_line1[100];
char lcd_line2[100];
bool update_line1 = true;
char lcd_lastline2[100];
char voicemodechar[10] = "?6";
bool clear_the_lcd = false;

// lcd:
// for LCD command string
char the_string[32];

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

// for usbmidi
#define CLOCKBYTE 0xf8
uint8_t clock_pulse_count;

#define MIDISTART 0xfa
#define MIDISTOP 0xfc

bool playstatus = false;
bool midistarted = false;
bool midistopped = false;

uint8_t play_started_at;
uint8_t chase_lights_status = 0;
uint8_t step_counter = 1;
byte last_pitch;

/////////////////////////////////
// sequencer
/////////////////////////////////

// sequencer init
FifteenStep seq = FifteenStep(1024);
float TEMPO = 120.0;
float timing_resolution = 1.0;
uint8_t timing_mode = 1;
uint8_t SWING = 0;
uint8_t MIDICHANNEL = 2;
String nn;

/////////////////////////////////
// outgoing serial data
/////////////////////////////////
char buffer[30];
char last_buffer[30];
char copy_pattern_return_message[30];
PString the_serial_message(buffer, sizeof(buffer));
PString last_serial(last_buffer, sizeof(last_buffer));
const char* last_last_message;

// incoming serial data
uint8_t inByte;
uint8_t lastInByte;
uint8_t current_step;
uint8_t last_step;

//////////////////////////////////
//
// voice modes.. um.. not sure what we're doing here yet
//
//////////////////////////////////

uint8_t voice_mode = 1;
uint8_t voice_mode_total = 3;

uint8_t voice_mode_pulse_counter = 1;
uint8_t pulse_adder = 1;
