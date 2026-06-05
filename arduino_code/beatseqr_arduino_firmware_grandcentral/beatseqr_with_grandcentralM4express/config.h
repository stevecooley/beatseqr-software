#include <SdFat.h>   // SD card storage (Adafruit Fork, replaces standard SD.h)
#include <stdlib.h>  // dtostrf()

#include "Button.h"
#include "FifteenStep.h"
#include "LED.h"
#include "MIDIUSB.h"
#include "PString.h"
#include "Potentiometer.h"

const char* firmware_version_number = "2.0";
const char* hardware_version_number = "4.51";

////////////////////////////////
//
// Voice select resistor ladder
//
// Eight buttons share one analog pin (A10). Each button pulls the pin to a
// different voltage via a resistor divider. ADC thresholds are 12-bit (0–4095).
// Idle (no button pressed) reads near 0. Button 0 = highest voltage.
// *** CALIBRATION IN PROGRESS — placeholder thresholds, do not use ***
//
////////////////////////////////

const int VOICE_COUNT = 8;

// Lower and upper ADC thresholds for each voice button (12-bit, 0–4095).
// Idle (no button) reads ~10. Button 0 = highest voltage, button 7 = lowest.
// Calibrated with ladder: 2.2k+2.2k+3k+3.9k+4.7k+5.6k+6.8k+15k series, 15k pull-down.
int vselectval_lowerranges[VOICE_COUNT] = {4047, 3660, 3016, 2471, 2016, 1675, 1305,  500};
int vselectval_upperranges[VOICE_COUNT] = {4095, 4046, 3659, 3015, 2470, 2015, 1674, 1304};

// Currently selected voice (0–7). Changed by pressing a voice select button.
// All step button presses, slider reads, and LED updates operate on this voice.
uint8_t current_voice = 0;
uint8_t last_voice_selected = 0;

////////////////////////////////
//
// Voice select LEDs
//
// One LED per voice on pins 2–9. The active voice LED stays lit; others off.
//
////////////////////////////////

LED voice_select_leds[VOICE_COUNT] = {
    LED(2), LED(3), LED(4), LED(5),
    LED(6), LED(7), LED(8), LED(9)};

////////////////////////////////
//
// Voice sliders (8 total, one per voice)
//
// In NN mode: sets the MIDI note number for that voice.
// In VL mode: sets velocity (acts as a live volume fader — 0 = silent/muted).
// In GT mode: sets gate length (1–8 steps).
// In CC mode: sets CC value for that voice.
//
////////////////////////////////

Potentiometer voice_sliders[VOICE_COUNT] = {
    Potentiometer(A7, 255), Potentiometer(A6, 255),
    Potentiometer(A5, 255), Potentiometer(A4, 255),
    Potentiometer(A3, 255), Potentiometer(A2, 255),
    Potentiometer(A1, 255), Potentiometer(A0, 255)};

int voice_slider_values[VOICE_COUNT];
int raw_voice_slider_values[VOICE_COUNT];
int last_voice_slider_values[VOICE_COUNT];

// Pickup guard: when true for a voice, that slider must physically reach the
// stored value before it takes control. Set on voice switch and mode switch.
bool slider_needs_pickup[VOICE_COUNT];

// Slider takeover behaviour (global), set via config menu "Takeover:" item.
//   0 = Catch    — slider must cross the stored value before it writes (pickup)
//   1 = Jump     — slider engages on first movement, value jumps to it
//   2 = Relative — value tracks the delta from the slider's physical position
// LV-style live modes do not exist on Beatseqr, so this applies to all 5 modes.
uint8_t slider_takeover = 0;

// Swing knob (A9) function during normal play (global), set via config menu
// "Swing knob:" item. The knob acts as a relative jog wheel (CW = +, CCW = -).
//   0 = Swing   — adjust SWING 0–5
//   1 = Tempo   — nudge TEMPO ±1 BPM
//   2 = Pattern — jog to next/previous pattern
//   3 = Voice   — jog to next/previous voice
uint8_t swing_knob_function = 0;
#define SWING_KNOB_FN_COUNT 4

// Last raw 12-bit ADC reading per slider; used by Jump movement detection and
// Relative deltas. Seeded from the physical position on every mode/pattern/voice
// switch via set_slider_mode().
uint16_t slider_last_raw[VOICE_COUNT];

uint8_t slider_mode       = 1;  // 1=NN  2=VL  3=GT  4=CC  5=PR
uint8_t slider_mode_total = 5;
uint8_t slider_reset_counter = 0;

uint8_t slider_map_low_value  = 36;   // default low MIDI note (kick drum range)
uint8_t slider_map_high_value = 51;   // default high MIDI note

////////////////////////////////
//
// Voice data — per-pattern, per-voice
//
// All sequencer data is indexed [pattern][voice] rather than [pattern][step].
// Each voice has a fixed pitch/velocity/gate that applies to all active steps.
//
////////////////////////////////

// MIDI note number for each voice (rows = patterns, cols = voices).
// Default: GM drum map starting at note 36 (bass drum).
uint8_t voice_pitch[16][VOICE_COUNT] = {
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39},
    {36, 38, 42, 46, 49, 51, 37, 39}};

// MIDI velocity for each voice. In VL mode the slider acts as a live fader:
// sliding to 0 silences the voice. Default 100 gives headroom to push louder.
uint8_t voice_velocity[16][VOICE_COUNT] = {
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100},
    {100, 100, 100, 100, 100, 100, 100, 100}};

// Gate length in steps (1–8) for each voice. Default 1 = staccato.
uint8_t voice_gate[16][VOICE_COUNT] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}};

// CC value (0–127) for each voice per pattern. Used in CC slider mode.
uint8_t voice_cc_value[16][VOICE_COUNT];   // zero-initialised = 0

// Fire probability (0–100%) for each voice per pattern. Default 100 = always fires.
// Controlled by slider mode 5 (PR). Checked in stepsend() before each note-on.
uint8_t voice_probability[16][VOICE_COUNT] = {
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100},
    {100,100,100,100,100,100,100,100}};

// Whether each voice fires a CC message when its step is active.
// Per-voice global (not per-pattern). Toggle in CC mode via step buttons or
// voice_mode_select if that function is wired up.
uint8_t voice_cc_enabled[VOICE_COUNT];     // zero-initialised = off

// CC controller number per pattern (shared across all voices in that pattern).
uint8_t cc_number[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Per-voice MIDI channel. Default: channel 10 (GM drum standard).
// Currently all voices use the global MIDICHANNEL, but this array is reserved
// for future per-voice channel routing.
int voice_slider_midichannel[VOICE_COUNT] = {10, 10, 10, 10, 10, 10, 10, 10};

////////////////////////////////
//
// Step buttons and LEDs
//
// Identical pin assignments to Synthseqr: LEDs 22–52 even, buttons 23–53 odd.
// Step LEDs reflect step_data for the current_voice in the current pattern.
//
////////////////////////////////

LED step_leds[16] = {
    LED(22), LED(24), LED(26), LED(28), LED(30), LED(32),
    LED(34), LED(36), LED(38), LED(40), LED(42), LED(44),
    LED(46), LED(48), LED(50), LED(52)};

Button step_buttons[16] = {
    Button(23, PULLUP), Button(25, PULLUP), Button(27, PULLUP),
    Button(29, PULLUP), Button(31, PULLUP), Button(33, PULLUP),
    Button(35, PULLUP), Button(37, PULLUP), Button(39, PULLUP),
    Button(41, PULLUP), Button(43, PULLUP), Button(45, PULLUP),
    Button(47, PULLUP), Button(49, PULLUP), Button(51, PULLUP),
    Button(53, PULLUP)};

// step_data[pattern][voice][step] — 1 = step active, 0 = step off
int step_data[16][VOICE_COUNT][16];

uint8_t pattern_value = 0;
uint8_t step_value    = 0;

////////////////////////////////
//
// Transport (play button + LED)
//
// Play button is on A11 (pin 65) with a hardware interrupt on falling edge.
// The ISR sets play_button_isr_fired; the main loop processes it.
// Play LED is on pin 10.
//
////////////////////////////////

Button playbutton    = Button(A11, PULLUP);
LED    playbutton_LED = LED(10);
bool   playbutton_flag = false;

////////////////////////////////
//
// Knobs — tempo and swing
//
// Dedicated hardware potentiometers replace the D-pad tempo/swing controls.
// BPM range: lower_BPM_number to upper_BPM_number.
// Swing range: 0–5 (same as Synthseqr).
//
////////////////////////////////

const int lower_BPM_number = 30;
const int upper_BPM_number = 250;

Potentiometer tempo_knob = Potentiometer(A8, 255);
Potentiometer swing_knob = Potentiometer(A9, 8);   // 8 sectors → 0–7, clamped to 0–5

// Jog-wheel state for config menu navigation (see knob_routine.ino).
int menu_knob_tempo_last = 0;   // saved knob position on menu entry
int menu_knob_swing_last = 0;

////////////////////////////////
//
// Mode select buttons
//
// slider_mode_select: single press cycles NN → VL → GT → CC → NN.
//                     Double-press is reserved for future use.
// voice_mode_select:  reserved (TODO: assign function).
// knob_mode_select:   double-click enters/exits config menu.
// param_rec (D11):    Enter / confirm in config menu.
//
////////////////////////////////

Button slider_mode_select = Button(A13, PULLUP);
Button voice_mode_select  = Button(A12, PULLUP);
Button knob_mode_select   = Button(A14, PULLUP);
Button param_rec          = Button(11, PULLUP);   // D11 — "Enter" button

bool slider_mode_flag     = false;
bool voice_mode_flag      = false;
bool knob_mode_flag       = false;
bool param_rec_flag       = false;
// Set by a knob_mode_select single-tap while the config menu is open; consumed
// by run_config_menu() to back out one level (cancel edit → cancel confirm →
// close submenu → exit menu). Replaces the old swing-knob-CCW cancel gesture.
bool knob_mode_back_flag  = false;

////////////////////////////////
//
// Pattern select buttons and LEDs
//
// Pins match original Beatseqr hardware (different from Synthseqr).
// Buttons: 18–21, LEDs: 14–17.
//
////////////////////////////////

LED pattern_select_leds[4] = {LED(17), LED(16), LED(15), LED(14)};

Button pattern_select_buttons[4] = {
    Button(21, PULLUP), Button(20, PULLUP),
    Button(19, PULLUP), Button(18, PULLUP)};

bool pattern_select_button_flags[4] = {false, false, false, false};

uint8_t pattern_select_button_pressing_counter = 0;
uint8_t extended_step_length_mode = 0;
uint8_t current_pattern = 0;
uint8_t patterns_to_play_in_a_row = 1;

uint8_t chain_start = 0;
uint8_t chain_end   = 3;

bool adv_copy_waiting_source = false;
bool adv_copy_armed          = false;
bool adv_pat_nav_active      = false;
int8_t adv_chain_hold_step   = -1;

unsigned long adv_blink_last_ms = 0;
bool adv_blink_state = false;

bool told_which_pattern_to_copy_to = false;
uint8_t copy_pattern_to;

char* pattern_padding;
char* step_padding;

////////////////////////////////
//
// LCD
//
// Hardware UART Serial1 (TX = pin 1) at 9850 baud, same protocol as Synthseqr.
// Line 1 format: P01 >03 V3 N 120   (16 chars)
//   col: 0       4   7  8  10 12
// Line 2 format: ♪036 ♩VVV G# ♩CCC  (16 chars, same as Synthseqr line 2)
//
////////////////////////////////

#define rxPin   255   // unused RX — just an invalid pin number
#define lcdTxPin  69   // Serial1 TX
#define lcd Serial1

// Line 1 cursor positions.
#define LCD_L1_X_PATTERN    1   // first digit of pattern number in "P%02u"
#define LCD_L1_X_STEP       5   // first digit of step counter in ">%02d"
#define LCD_L1_X_VOICE      9   // voice digit in "V%1u"
#define LCD_L1_X_SLIDERMODE 11  // slider mode char (N / V / G / C)
#define LCD_L1_X_TEMPO      13  // first digit of 3-digit integer BPM

uint8_t lcdflag      = 255;
uint8_t next_lcdflag = 255;
char lcd_line1[100];
char lcd_line2[100];
bool update_line1 = true;
bool update_line2 = true;
char lcd_lastline2[100];
bool clear_the_lcd = false;

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
bool cursor_flag = false;   // no D-pad cursor in Beatseqr

// LCD scratch vars
char the_string[32];
char N;
int I;
int ByteVar;
int NN;
int Remainder;
int Num_5;

////////////////////////////////
//
// MIDI / sequencer state
//
////////////////////////////////

unsigned long now;
unsigned long last_step_time;
unsigned long next_step_time;

#define CLOCKBYTE 0xf8
uint8_t clock_pulse_count;
#define MIDISTART 0xfa
#define MIDISTOP  0xfc

bool playstatus   = false;
bool midistarted  = false;
bool midistopped  = false;

uint8_t play_started_at;
uint8_t chase_lights_status = 0;
uint8_t step_counter = 1;
byte    last_pitch;

FifteenStep seq = FifteenStep(1024);
float   TEMPO        = 120.0;
uint8_t SWING        = 0;
uint8_t MIDICHANNEL  = 10;   // GM drum channel

// Sounding notes indexed by voice (not step — each voice sounds at most one note).
// -1 = voice not currently sounding.
int8_t sounding_notes[VOICE_COUNT]        = {-1, -1, -1, -1, -1, -1, -1, -1};
int8_t sounding_note_end_step[VOICE_COUNT]= {-1, -1, -1, -1, -1, -1, -1, -1};

// Most-recently-triggered step (for LCD line 1 step counter). -1 = none yet.
int8_t last_triggered_step = -1;

/////////////////////////////////
// outgoing serial data
/////////////////////////////////
char buffer[30];
char last_buffer[30];
char copy_pattern_return_message[30];
PString the_serial_message(buffer, sizeof(buffer));
PString last_serial(last_buffer, sizeof(last_buffer));
const char* last_last_message;

uint8_t inByte;
uint8_t lastInByte;
uint8_t current_step;
uint8_t last_step = 15;

/////////////////////////////////
// clock source
/////////////////////////////////

bool external_clock_mode = false;

/////////////////////////////////
// operating mode (Simple / Advanced)
/////////////////////////////////

bool advanced_mode = false;

/////////////////////////////////
// config menu
/////////////////////////////////

bool config_menu_active      = false;
uint8_t config_menu_item     = 0;
bool config_confirm_pending  = false;
bool config_editing_value    = false;
uint8_t config_note_range_phase = 0;
uint8_t config_scale_phase      = 0;

/////////////////////////////////
// runtime feature flags (Features submenu)
//
// Each flag enables an optional capability. When a flag is off, its config
// menu item(s) are skipped during scrolling and, for slider-mode features,
// the slider-mode cycle hops over that mode. Persisted to EEPROM + SD.
/////////////////////////////////

bool ft_advanced_mode       = true;   // Mode item + advanced pattern layout
bool ft_cc_mode             = true;   // slider mode 4 (CC) + CC num item
bool ft_probability         = true;   // slider mode 5 (PR) + Voice prob item
bool ft_gate_mode           = true;   // slider mode 3 (GT)
bool ft_velocity_mode       = true;   // slider mode 2 (VL)
bool ft_scale_quantization  = true;   // Note scales item
bool ft_pattern_direction   = true;   // Pat dir item
bool ft_variable_pat_length = true;   // Pat length item
bool ft_external_clock      = true;   // Clock item
bool ft_octave_note_shift   = true;   // Octave shift + Note shift items
bool ft_diagnostics         = true;   // Diagnostics item + hardware test mode

// Features submenu navigation state.
bool    config_features_active = false;
uint8_t config_feature_item    = 0;

// Diagnostics submenu navigation state (Input test / LED test).
bool    config_diag_active = false;
uint8_t config_diag_item   = 0;
#define DIAG_SUBMENU_ITEM_COUNT 2

// Save-file viewer: number of top-level JSON fields shown in diagnostics.
// Explicit prototype avoids Arduino auto-prototype issues with 2D-array params.
#define SD_DIAG_FIELD_COUNT 19
void sd_diag_load_fields(char l1[][17], char l2[][17]);

/////////////////////////////////
// octave / note shift
/////////////////////////////////

int8_t octave_shift = 0;   // ±5 octaves, applied at MIDI send time
int8_t note_shift   = 0;   // ±12 semitones, applied at MIDI send time

/////////////////////////////////
// note scales
/////////////////////////////////

#define SCALE_COUNT 9
uint8_t scale_type  = 0;   // 0=Chromatic … 8=Blues
uint8_t scale_root  = 0;   // 0=C … 11=B

uint8_t scale_note_pool[128];
uint8_t scale_note_count = 0;

extern const char* SCALE_NAMES[SCALE_COUNT];
extern const char* ROOT_NAMES[12];

/////////////////////////////////
// Pattern playback settings
/////////////////////////////////

#define PATTERN_DIRECTION_COUNT 8
uint8_t pattern_length    = 16;
uint8_t pattern_direction = 0;

bool    ping_pong_going_forward = true;
uint8_t ping_pong_step          = 0;

uint8_t shuffle_order[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t shuffle_pos = 0;

/////////////////////////////////
// External clock swing state
/////////////////////////////////

uint8_t ext_clk_pulse_count         = 0;
unsigned long ext_clk_last_pulse_us = 0;
unsigned long ext_clk_avg_interval_us = 20833;
bool ext_swing_pulse_pending          = false;
unsigned long ext_swing_pulse_fire_us = 0;
bool ext_clock_start_pending          = false;

/////////////////////////////////
// Forward declarations (defined in their respective .ino files)
/////////////////////////////////

extern volatile bool play_button_isr_fired;
extern bool diag_mode;
