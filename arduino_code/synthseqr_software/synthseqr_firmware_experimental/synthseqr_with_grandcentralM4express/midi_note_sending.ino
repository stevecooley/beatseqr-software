// based on code from:
// Author: Todd Treece <todd@uniontownlabs.org>
// Copyright: (c) 2015 Adafruit Industries
// License: GNU GPLv3

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                         SEQUENCER CALLBACKS                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// called when the step position changes. both the current
// position and last are passed to the callback
void step(int current, int last) {

  // blink on even steps
  if(current % 2 == 0)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);

}

// the callback that will be called by the sequencer when it needs
// to send midi commands. this specific callback is designed to be
// used with a standard midi cable.
//
// the following image will show you how your MIDI cable should
// be wired to the Arduino:
// http://arduino.cc/en/uploads/Tutorial/MIDI_bb.png
void midi(byte channel, byte command, byte arg1, byte arg2) {

  // Beat flash for the play-button LED. This callback fires once per 24-PPQN
  // clock pulse (command 0xF8) while the sequencer is running, in both internal
  // and external clock modes. Counting pulses here keeps the flash locked to
  // the quarter-note beat regardless of clock_div — the LED is on for the first
  // half of each quarter and off for the second half.
  if (command == 0xF8 && playstatus) {
    if (beat_pulse_count < 12) playbutton_LED.on();
    else                       playbutton_LED.off();
    beat_pulse_count++;
    if (beat_pulse_count >= 24) beat_pulse_count = 0;
  }

  if(command < 128) {
    // shift over command
    command <<= 4;
    // add channel to the command
    command |= channel;
  }

  // send MIDI data
  midiEventPacket_t thecommand = {channel, command, arg1, arg2};
  MidiUSB.sendMIDI(thecommand);

}