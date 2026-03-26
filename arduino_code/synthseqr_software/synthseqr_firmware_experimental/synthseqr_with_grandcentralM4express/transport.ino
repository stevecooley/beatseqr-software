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

      // flush
      MidiUSB.flush();

      // stop the internal sequencer
      seq.stop();

      // experimental to see if this can help prevent the last note from being held open. Midi.flush should have done it??
      if (step_data[pattern_value][0][last_step] == 1) {
        noteOff(MIDICHANNEL - 1, voice_slider_midinotenum[last_step], 0);
      }

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
      // stop the sequencer
      seq.stop();

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

void stepsend(int current_step, int last_step) {
  // current_step = seq.getPosition();

  run_chase_lights(seq.getPosition());

  // Guard against out-of-bounds on the first callback after start(),
  // where last_step arrives as 255 (byte wrap of -1 sentinel).
  if (last_step >= 0 && last_step < 16 && step_data[pattern_value][0][last_step] == 1) {
    noteOff(MIDICHANNEL - 1, voice_slider_midinotenum[last_step], 0);
  }

  if (step_data[pattern_value][0][current_step] == 1) {
    noteOn(MIDICHANNEL - 1, voice_slider_midinotenum[current_step], 127);
  }
  return;
}
