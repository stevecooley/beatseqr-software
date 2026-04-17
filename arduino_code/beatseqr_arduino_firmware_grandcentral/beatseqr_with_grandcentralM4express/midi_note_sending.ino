// Based on code from:
// Author: Todd Treece <todd@uniontownlabs.org>
// Copyright: (c) 2015 Adafruit Industries
// License: GNU GPLv3

// Low-level MIDI helpers — all MIDI output goes through these so there is one
// place to swap the transport (USB-MIDI today, TRS tomorrow).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, (byte)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, (byte)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, (byte)(0xB0 | channel), control, value};
  MidiUSB.sendMIDI(event);
}

// Send MIDI real-time messages (clock, start, stop).
// These are single-byte messages; MIDIUSB wraps them in a 4-byte packet.
void sendRealTime(byte rtByte) {
  midiEventPacket_t rt = {0x0F, rtByte, 0, 0};
  MidiUSB.sendMIDI(rt);
}

void clockStart() { sendRealTime(0xFA); MidiUSB.flush(); }
void clockStop()  { sendRealTime(0xFC); MidiUSB.flush(); }
void clockPulse() { sendRealTime(0xF8); MidiUSB.flush(); }

// midi() — FifteenStep MIDI clock callback.
//
// Called by the sequencer library when it needs to send a MIDI byte (used for
// clock output in internal mode). In hardware-timer mode the library calls this
// for 0xF8 clock pulses only; transport start/stop is handled by clockStart()
// and clockStop() above.
void midi(byte channel, byte command, byte arg1, byte arg2) {
  if (command < 128) {
    command <<= 4;
    command |= channel;
  }
  midiEventPacket_t thecommand = {channel, command, arg1, arg2};
  MidiUSB.sendMIDI(thecommand);
}

// step() — optional LED13 heartbeat (blinks on even steps for scope debugging).
void step(int current, int last) {
  if (current % 2 == 0)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
}
