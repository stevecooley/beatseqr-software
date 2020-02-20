void run_transport_button_routine()
{
  // transport buttons
  if (playbutton.uniquePress() || playstatus == true)
  {
    playbutton_LED.toggle();

    if (playbutton_LED.getState() == true)
    {
      if (enterbutton.isPressed() == true)
      {
        isRecordingArmed = 1;
      }

      clockStart();
      // MidiUSB.flush();
      seq.start();
      chase_lights_status = 1;

      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?0");      //play
      lcd.print(" ");
      /*
      Serial.print("chase lights armed! now = ");
      Serial.print(last_step_time);
      Serial.println();
      */
    }
    else
    { // pause, but we're going to stop.

      // midi stop
      // MidiUSB.flush();
      clockStop();
      // MidiUSB.flush();
      seq.stop();
      chase_lights_status = 0;
      Serial.println("chase lights off");

      if (extended_step_length_mode == 1)
      {
        current_pattern = 0;
        // go_to_pattern(0, 1);
      }

      // the_serial_message = "ZPL,0;";
      //serial_printer(the_serial_message);

      if (isRecordingArmed == 1)
      {
        // the_serial_message = "ZRE,0;";
        // serial_printer(the_serial_message);
        isRecordingArmed = 0;
      }

      lcd.print("?x00?y0"); // move cursor to beginning of line 0
      lcd.print("?7");
      lcd.print(" ");
    }
  }
}

void run_chase_lights(unsigned int this_step)
{

  if (chase_lights_status == 1)
  {

    if (last_step != this_step) // clock pulses counted so we can advance to the next step.
    {

      /*
      // Serial.println(this_step,HEX);
      lcd.print("?x03?y0"); // move cursor to beginning of line 1
      if(this_step < 10)
      {
        lcd.print(" ");

      }
      lcd.print(this_step);
      */
     
      // HAHA don't call it here, too! Derp! It's a call-back already.
      // stepsend(this_step, 16);

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
  if (step_data[pattern_value][0][current_step] == 1)
  {
    noteOn(1, voice_slider_midinotenum[current_step], 70);
    delay(50);
    noteOff(1, voice_slider_midinotenum[current_step], 0);
  }
  return;
}
