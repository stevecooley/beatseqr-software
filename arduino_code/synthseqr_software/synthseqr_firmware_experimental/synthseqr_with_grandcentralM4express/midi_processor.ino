bool print_message(void *msg)
{
  const char *m = (const char *)msg;
  Serial.print("print_message: ");
  Serial.println(m);
  //  noteOff(0, 48, 64);   // Channel 0, middle C, normal velocity

  return true; // repeat? true
}

void read_midi()
{

  midiEventPacket_t rx;
  do
  {
    rx = MidiUSB.read();
    if (rx.header != 0)
    {

      // midi thru
      // MidiUSB.sendMIDI(rx);
      // MidiUSB.flush();

      /*
        Serial.print("Received: ");
        Serial.print(rx.header, HEX);
        Serial.print("-");
        Serial.print(rx.byte1, HEX);
        Serial.print("-");
        Serial.print(rx.byte2, HEX);
        Serial.print("-");
        Serial.println(rx.byte3, HEX);
      */
      if (rx.byte1 == CLOCKBYTE)
      {
        if (external_clock_mode)
        {
          // Slave mode: let incoming clock drive the sequencer directly.
          // TC4 is stopped, so this is the only clock source.
          seq.hardwareClockPulse();
        }
        else
        {
          // Master mode: forward our own outgoing clock pulse.
          clockPulse();

          clock_pulse_count++;
          if (clock_pulse_count > 5)
          {
            clock_pulse_count = 0;
            current_step++;
            if (current_step > 15)
            {
              current_step = 0;
            }
          }
        }
      }
      else if (rx.byte1 == MIDISTART)
      {
        Serial.println("Midi Start!");
        // this flag hands off to transport.ino
        midistarted = true;
      }
      else if (rx.byte1 == MIDISTOP)
      {
        Serial.println("Midi Stop!");
        // this flag hands off to transport.ino
        midistopped = true;
      }
    }

  } while (rx.header != 0);
}

/*
   MIDIUSB_test.ino

   Created: 4/6/2015 10:47:08 AM
   Author: gurbrinder grewal
   Modified by Arduino LLC (2015)
*/

#include <MIDIUSB.h>

// Modify the noteOn and noteOff functions
void noteOn(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t thenote = {0x08, (uint8_t)(0x90 | channel), pitch, velocity};
    MidiUSB.sendMIDI(thenote);
    MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t thenote = {0x08, (uint8_t)(0x80 | channel), pitch, 0};
    MidiUSB.sendMIDI(thenote);
    MidiUSB.flush();
}

// Update clock functions to use the correct method
void clockStop() {
    midiEventPacket_t stopMsg = {0x0, 0xFC, 0x0, 0x0}; // Stop byte
    MidiUSB.sendMIDI(stopMsg);
    MidiUSB.flush();
}

void clockStart() {
    midiEventPacket_t startMsg = {0x0, 0xFA, 0x0, 0x0}; // Start byte
    MidiUSB.sendMIDI(startMsg);
    MidiUSB.flush();
}

void clockPulse() {
    midiEventPacket_t pulseMsg = {0x0, 0xF8, 0x0, 0x0}; // Clock byte
    MidiUSB.sendMIDI(pulseMsg);
    MidiUSB.flush();
}