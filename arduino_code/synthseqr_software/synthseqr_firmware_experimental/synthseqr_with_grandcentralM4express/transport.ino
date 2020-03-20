void listen_for_delay_tasks() {}

void listen_for_transport_events()
{
  if (playbutton.uniquePress())
  {
    transport_button_pressed = true;
  }

  if (transport_button_pressed == true || midistarted == true ||
      midistopped == true)
  {

    // now we'll check the playstatus to see if it's false.
    // this means that we're not playing anything.
    if (transport_button_pressed == true && playstatus == false)
    {
      // stand down the transport button pressed flag
      transport_button_pressed = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing stuff
      playstatus = true;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?0");      // play

      // start the outbound midi clock

      clockStart();
      // start the sequencer
      seq.start();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    }
    else if (transport_button_pressed == true && playstatus == true)
    {
      // stand down the transport button pressed flag
      transport_button_pressed = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?7");      // stop

      // stop the outbound midi clock
      clockStop();

      // flush
      MidiUSB.flush();

      // stop the internal sequencer
      seq.stop();
    }
    else if (midistarted == true)
    {
      // stand down the event flag, we got this
      midistarted = false;

      // turn the play button LED on
      playbutton_LED.on();

      // start playing
      playstatus = true;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?0");      // play

      // start the outbound midi clock
      clockStart();
      // start the sequencer
      seq.start();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    }
    else if (midistopped == true)
    {
      // stand down the event flag, we got this
      midistopped = false;

      // turn the play button LED off
      playbutton_LED.off();

      // stop playing
      playstatus = false;

      // update the play/stop character on the LCD
      // one time for yo mind
      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?7");      // stop

      // stop the outbound midi clock
      clockStop();
      // stop the sequencer
      seq.stop();

      // turn on the chase lights, I guess? I mean there might be times you
      // wouldn't want this to always happen. *sigh*
      chase_lights_status = true;
    }
  }
}

void run_chase_lights(unsigned int this_step)
{

  if (chase_lights_status == 1)
  {

    if (last_step !=
        this_step) // clock pulses counted so we can advance to the next step.
    {

      // clear the LEDs back to their data
      read_step_memory(0, pattern_value);
      if (step_leds[this_step].getState() == 0)
      {
        step_leds[this_step].toggle(); // chase lights!
      }
      last_step = this_step;
    }
  }
  else
  {
    read_step_memory(0, pattern_value);
  }
}

void stepsend(int current_step, int last_step)
{

  // current_step = seq.getPosition();

  run_chase_lights(seq.getPosition());

  if (step_data[pattern_value][0][last_step] == 1)
  {
    noteOff(MIDICHANNEL - 1, voice_slider_midinotenum[last_step], 0);
  }

  if (step_data[pattern_value][0][current_step] == 1)
  {
    noteOn(MIDICHANNEL - 1, voice_slider_midinotenum[current_step], 127);
  }
  return;
}
