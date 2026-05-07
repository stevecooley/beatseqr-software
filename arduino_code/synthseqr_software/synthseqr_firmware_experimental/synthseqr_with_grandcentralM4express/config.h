// Set to 1 for original PCB (sliders 12/13 wired A2/A3 swapped),
// set to 2 for revised PCB (sliders 12/13 in correct A3/A2 order).
#define PCB_VERSION 1

#include <SdFat.h>   // SD card storage (Adafruit Fork, replaces standard SD.h)
#include <stdlib.h>  // because dtostrf()

#include "Button.h"
#include "FifteenStep.h"
#include "LED.h"
#include "MIDIUSB.h"
#include "PString.h"
#include "Potentiometer.h"

const char* firmware_version_number = "3.0";
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

#if PCB_VERSION == 1
    Potentiometer(A2, 255),  Potentiometer(A3, 255),
#else
    Potentiometer(A3, 255),  Potentiometer(A2, 255),
#endif
    Potentiometer(A1, 255),  Potentiometer(A0, 255)};

int voice_slider_values[16];

int raw_voice_slider_values[16];

uint8_t voice_slider_midivelocity[16] = {127, 127, 127, 127, 127, 127,
                                         127, 127, 127, 127, 127, 127,
                                         127, 127, 127, 127};

// Per-pattern saved velocities. Updated when a slider moves in VL mode.
// Restored when switching patterns so each pattern remembers its own
// velocities.
uint8_t pattern_step_velocities[16][16] = {
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127},
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
     127}};

// Per-pattern gate lengths (1-16 steps). Sliders map 0-255 -> 1-16 in GT mode.
uint8_t step_gate[16][16] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                             {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
// CC sequencing data — per-pattern, per-step
uint8_t cc_step_values[16][16];   // CC value (0–127) per pattern per step;
                                  // defaults to 0
uint8_t cc_step_enabled[16][16];  // 1=fire CC on this step, 0=skip; independent
                                  // from notes
uint8_t cc_number[16] =
    {  // CC controller number per pattern (1–119, safe range)
        1, 1, 1, 1,
        1, 1, 1, 1,  // default: CC 1 (Mod Wheel) for all 16 patterns
        1, 1, 1, 1,
        1, 1, 1, 1};

// Per-step fire probability (0–100; 100 = always fires, 0 = never).
// Applied in stepsend() before sending note-on.
uint8_t step_probability[16][16] = {
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100}};
int voice_slider_midinotenum[16] = {36, 37, 38, 39, 40, 41, 42, 43,
                                    44, 45, 46, 47, 48, 49, 50, 51};

// Per-pattern saved pitches. Updated whenever a slider moves (after pickup).
// Restored when switching patterns so each pattern remembers its own tuning.
uint8_t pattern_step_pitches[16][16] = {
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}};

// When true for a step, that slider must physically reach the stored pitch
// before it takes control. Set on pattern switch; cleared per-slider on pickup.
bool slider_needs_pickup[16];
int voice_slider_midichannel[16] = {1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1};

int last_voice_slider_values[16];
uint8_t slider_mode = 1;  // 1=NN  2=VL  3=GT  4=CC  5=PR
uint8_t slider_mode_total = 5;
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

// step_data = 16 patterns, 1 voice, 16 steps
int step_data[16][1][16];
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
bool playbutton_flag = false;

uint8_t isRecordingArmed = 0;

Button dpad_up = Button(19, PULLUP);
bool dpad_up_flag = false;
Button dpad_right = Button(16, PULLUP);
bool dpad_right_flag = false;
Button dpad_down = Button(18, PULLUP);
bool dpad_down_flag = false;
Button dpad_left = Button(17, PULLUP);
bool dpad_left_flag = false;
Button enterbutton = Button(20);
bool enterbutton_flag = false;
LED enterbutton_LED = LED(3);

uint8_t navmode = 100;  // default to tempo and swing adjust

LED pattern_select_leds[4] = {

    LED(4), LED(5), LED(7), LED(9)};

uint8_t pattern_select_button_pressing_counter = 0;

Button pattern_select_buttons[4] = {Button(15, PULLUP), Button(14, PULLUP),
                                    Button(6, PULLUP), Button(8, PULLUP)};

bool pattern_select_button_flags[4] = {false, false, false, false};

uint8_t extended_step_length_mode = 0;
uint8_t current_pattern = 0;
uint8_t patterns_to_play_in_a_row = 1;

// Chain range — used in both simple and advanced mode.
// Simple mode: chain toggles between single pattern and 0→3 (old behaviour).
// Advanced mode: button 2 sets arbitrary start/end within 0–15.
uint8_t chain_start = 0;
uint8_t chain_end = 3;

// Advanced mode pattern-copy state.
// Phase 1 (adv_copy_waiting_source): waiting for a step button tap to select
//   the source pattern to copy FROM; d-pad left cancels.
// Phase 2 (adv_copy_armed): source is current_pattern; waiting for a step
//   button tap to select the destination pattern; d-pad left cancels.
bool adv_copy_waiting_source = false;
bool adv_copy_armed = false;

// Advanced mode pattern-nav mode — toggled by a single click of pattern
// button 1. While active, step buttons select/chain patterns instead of editing
// steps.
bool adv_pat_nav_active = false;

// Which step button is currently held for chain selection (-1 = none).
// Set when a step is first pressed in nav mode; cleared on release.
int8_t adv_chain_hold_step = -1;

// Blink state for the currently-playing pattern's step LED in nav mode.
unsigned long adv_blink_last_ms = 0;
bool adv_blink_state = false;

// Timestamp of the most recent Pattern Button 0 press in advanced mode.
// Kept as a global (not static local) so it can be reset to 0 whenever
// advanced_mode is enabled — prevents a stale press from instantly
// triggering the 400 ms single-click timeout on first entry to adv mode.
unsigned long adv_pat0_press_ms = 0;

bool told_which_pattern_to_copy_to = false;
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

#define lcdTxPin 1  // hardware version 1.0, hack version 2.0

#define lcd Serial1

// LCD field cursor positions — update these if the display format changes.
// Line 1: P{pat:02u} >{step:02d} {tempo:05.1f} [?5][mode]  (16 chars total)
//   col:  0         4           8              14
#define LCD_L1_X_PATTERN 1   // first (tens) digit of pattern in "P%02u" (col 1)
#define LCD_L1_X_STEP 5      // first (tens) digit of step in ">%02d" (col 5)
#define LCD_L1_X_TEMPO_10 9  // tempo tens digit (±10 BPM, col 9)
#define LCD_L1_X_TEMPO_1 10  // tempo units digit (±1 BPM, col 10)
#define LCD_L1_X_TEMPO_01 \
  12  // tempo tenths digit, after decimal at col 11 (±0.1 BPM)
#define LCD_L1_X_SLIDERMODE \
  14  // first char of 2-char slider-mode indicator (?5 + mode)
// Line 2: ?4PPP ?2VVV G#?3CCC  — note, velocity, gate, CC value (16 chars
// total) Swing, clock, and MIDI channel are now in the config menu.

uint8_t lcdflag = 255;
uint8_t next_lcdflag = 255;
char lcd_line1[100];
char lcd_line2[100];
bool update_line1 = true;
bool update_line2 = true;
char lcd_lastline2[100];
char voicemodechar[10] = "?6";
bool clear_the_lcd = false;

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
bool cursor_flag = true;

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
uint8_t timing_mode = 2;  // default: ±10 BPM (mode 2 in new visual order)
uint8_t SWING = 0;
uint8_t pitch_drift = 0;  // semitones of random pitch wander at send time (0=off, 1–7)
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
uint8_t last_step = 15;

// Tracks the MIDI pitch actually sent for each step so note-off always
// uses the exact pitch from the note-on, regardless of slider changes.
// -1 means the step is not currently sounding.
int8_t sounding_notes[16] = {-1, -1, -1, -1, -1, -1, -1, -1,
                             -1, -1, -1, -1, -1, -1, -1, -1};

// Which step number triggers the note-off for each sounding note.
// (current_step + gate) % 16. -1 means not scheduled.
int8_t sounding_note_end_step[16] = {-1, -1, -1, -1, -1, -1, -1, -1,
                                     -1, -1, -1, -1, -1, -1, -1, -1};

// The last step that actually fired a note-on (for LCD line 2 feedback).
// -1 = no step has fired yet this session.
int8_t last_triggered_step = -1;
// The actual MIDI pitch sent for that step (after octave/note shift + scale quantization).
uint8_t last_triggered_pitch = 0;

//////////////////////////////////
//
// voice modes.. um.. not sure what we're doing here yet
//
//////////////////////////////////

uint8_t voice_mode = 1;
uint8_t voice_mode_total = 3;

uint8_t voice_mode_pulse_counter = 1;
uint8_t pulse_adder = 1;

/////////////////////////////////
// clock source
/////////////////////////////////

// false = internal TC4 hardware timer (default)
// true  = follow incoming USB-MIDI clock (0xF8 / 0xFA / 0xFC)
bool external_clock_mode = false;

/////////////////////////////////
// operating mode
/////////////////////////////////

// false = Simple mode (4 pattern buttons, current behaviour)
// true  = Advanced mode (pattern buttons as function keys, 16 patterns, etc)
bool advanced_mode = false;

/////////////////////////////////
// config menu
/////////////////////////////////

bool config_menu_active = false;
uint8_t config_menu_item = 0;  // persists between opens; restored on re-entry
bool config_confirm_pending = false;
bool config_editing_value =
    false;  // true while adjusting a value (e.g. octave shift)
uint8_t config_note_range_phase =
    0;  // 0 = editing low, 1 = editing high (used by NOTE_RANGE item)
uint8_t config_scale_phase =
    0;  // 0 = editing scale type, 1 = editing root (used by NOTE_SCALES item)

/////////////////////////////////
// octave shift
/////////////////////////////////

// Applied at MIDI send time. Each unit = 12 semitones. Range -5 to +5.
// Stored separately from per-step pitches so tuning is not affected.
int8_t octave_shift = 0;

// Semitone offset applied at MIDI send time on top of octave_shift.
// Range -12 to +12. Stored separately from per-step pitches.
int8_t note_shift = 0;

/////////////////////////////////
// note scales
/////////////////////////////////

// 0=Chromatic 1=Major 2=NatMinor 3=PentMaj 4=PentMin
// 5=Dorian 6=Mixolydian 7=HarmMinor 8=Blues
#define SCALE_COUNT 9
#define SD_DIAG_FIELD_COUNT 16
uint8_t scale_type = 0;

// 0=C 1=C# 2=D 3=D# 4=E 5=F 6=F# 7=G 8=G# 9=A 10=A# 11=B
uint8_t scale_root = 0;

// In-scale MIDI notes within the current note range; populated by
// build_scale_notes().
uint8_t scale_note_pool[128];
uint8_t scale_note_count = 0;

// Declared in scales.ino; forward-declared here so config_menu.ino can use
// them. (Arduino merges .ino files alphabetically; config_menu.ino precedes
// scales.ino.)
extern const char* SCALE_NAMES[SCALE_COUNT];
extern const char* ROOT_NAMES[12];

// Pattern playback settings — global, applies to all patterns.
// pattern_direction: 0=Fwd 1=Rev 2=Pong 3=Rand 4=Shuf 5=E/O 6=In 7=Quad
#define PATTERN_DIRECTION_COUNT 8
uint8_t pattern_length = 16;          // 1–16 steps before looping
uint8_t pattern_direction = 0;        // see above
bool ping_pong_going_forward = true;  // ping-pong direction state
uint8_t ping_pong_step = 0;           // ping-pong virtual play position
// Shuffle (permutation) state — init_shuffle() fills this before use.
uint8_t shuffle_order[16] = {0, 1, 2,  3,  4,  5,  6,  7,
                             8, 9, 10, 11, 12, 13, 14, 15};
uint8_t shuffle_pos = 0;

// External clock swing state.
// When SWING > 0 in external clock mode, odd-step transitions are deferred
// by avg_pulse_interval * SWING µs to replicate the internal-clock swing feel.
uint8_t ext_clk_pulse_count = 0;          // 0–5 within current step
unsigned long ext_clk_last_pulse_us = 0;  // micros() of last 0xF8 pulse
unsigned long ext_clk_avg_interval_us =
    20833;                                  // running avg (default: 120 BPM)
bool ext_swing_pulse_pending = false;       // deferred 6th-pulse queued
unsigned long ext_swing_pulse_fire_us = 0;  // micros() value to fire it at
bool ext_clock_start_pending =
    false;  // play pressed; waiting for next beat boundary to start

// Declared in transport.ino; forward-declared here so the main sketch can
// read the flag set by the play button hardware interrupt.
extern volatile bool play_button_isr_fired;

// Declared in diagnostics.ino; forward-declared here so loop() and LCD.ino
// can suppress normal subsystems while diagnostics mode is active.
extern bool diag_mode;
