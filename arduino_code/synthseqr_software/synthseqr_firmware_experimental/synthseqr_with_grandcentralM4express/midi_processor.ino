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

        /*
          Serial.print("clock pulse count = " );
          Serial.print(clock_pulse_count);
          Serial.println();
        */
      }
      else if (rx.byte1 == MIDISTART)
      {
        Serial.println("Midi Start!");
        playstatus = true;
      }
      else if (rx.byte1 == MIDISTOP)
      {
        Serial.println("Midi Stop!");
        playstatus = false;
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

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t thenote = {0x08, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(thenote);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t thenote = {0x08, 0x90 | channel, pitch, 0};
  MidiUSB.sendMIDI(thenote);
  MidiUSB.flush();
}

void clockStop()
{
  midiEventPacket_t clockstop = {0x08, 0xFC};
  MidiUSB.sendMIDI(clockstop);
  // MidiUSB.flush();
}

void clockStart()
{
  midiEventPacket_t clockstart = {0x08, 0xFA};
  MidiUSB.sendMIDI(clockstart);
  // MidiUSB.flush();

  // current_step = 0;
}