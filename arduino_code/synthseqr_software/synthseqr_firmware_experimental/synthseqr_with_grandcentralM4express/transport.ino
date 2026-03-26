void listen_for_delay_tasks() {}

void listen_for_transport_events() {
  if (playbutton_flag == true || midistarted == true || midistopped == true) {
    // now we'll check the playstatus to see if it's false.
    // this means that we're not playing anything.
    if (playbutton_flag == true && playstatus == false) {
      // stand down the transport button pressed flag
      playbutton_flag = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing stuff
      playstatus = true;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0");       // move cursor to beginning of line 0
      Serial.println("?x00?y0");  // move cursor to beginning of line 0
      lcd.print("?0");            // play
      Serial.println("?0");       // play

      // In external clock mode the host is the master; don't send transport.
      if (!external_clock_mode) clockStart();
      // start the sequencer
      seq.start();
      // Synchronise TC4 counter (internal mode only; TC4 is stopped in ext mode).
      if (!external_clock_mode) resetSequencerTimerSync();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    } else if (playbutton_flag == true && playstatus == true) {
      // stand down the transport button pressed flag
      playbutton_flag = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0");       // move cursor to beginning of line 0
      Serial.println("?x00?y0");  // move cursor to beginning of line 0
      lcd.print("?7");            // stop
      Serial.println("?7");       // stop

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();

    } else if (midistarted == true) {
      // stand down the event flag, we got this
      midistarted = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing
      playstatus = true;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0");       // move cursor to beginning of line 0
      Serial.println("?x00?y0");  // move cursor to beginning of line 0
      lcd.print("?0");            // play
      Serial.println("?0");       // play

      if (!external_clock_mode) clockStart();
      // start the sequencer
      seq.start();
      if (!external_clock_mode) resetSequencerTimerSync();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    } else if (midistopped == true) {
      // stand down the event flag, we got this
      midistopped = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0");       // move cursor to beginning of line 0
      Serial.println("?x00?y0");  // move cursor to beginning of line 0
      lcd.print("?7");            // stop
      Serial.println("?7");       // stop

      if (!external_clock_mode) clockStop();
      seq.stop();
      allNotesOff();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    }
  }
}

void run_chase_lights(unsigned int this_step) {
  if (chase_lights_status == 1) {
    if (last_step !=
        this_step)  // clock pulses counted so we can advance to the next step.
    {
      // clear the LEDs back to their data
      read_step_memory(0, pattern_value);
      // if (step_leds[this_step].getState() == 0) {
      step_leds[this_step].toggle();  // chase lights!
      // }
      last_step = this_step;
    }
  } else {
    read_step_memory(0, pattern_value);
  }
}

// allNotesOff()
//
// Sends note-off for every step that is currently sounding, using the exact
// pitch that was sent in the note-on. Safe to call on stop, pattern change,
// or any other situation where notes might be left open.
//
void allNotesOff() {
  for (int i = 0; i < 16; i++) {
    if (sounding_notes[i] >= 0) {
      noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[i], 0);
      sounding_notes[i] = -1;
    }
  }
  MidiUSB.flush();
}

void stepsend(int current_step, int last_step) {
  run_chase_lights(seq.getPosition());

  // Note-off for the previous step using the pitch that was actually played.
  // Guards against out-of-bounds on the first callback after start()
  // where last_step arrives as 255 (byte wrap of -1 sentinel).
  if (last_step >= 0 && last_step < 16 && sounding_notes[last_step] >= 0) {
    noteOff(MIDICHANNEL - 1, (uint8_t)sounding_notes[last_step], 0);
    sounding_notes[last_step] = -1;
  }

  if (step_data[pattern_value][0][current_step] == 1) {
    uint8_t pitch = voice_slider_midinotenum[current_step];
    noteOn(MIDICHANNEL - 1, pitch, 127);
    sounding_notes[current_step] = (int8_t)pitch;
  }
}
