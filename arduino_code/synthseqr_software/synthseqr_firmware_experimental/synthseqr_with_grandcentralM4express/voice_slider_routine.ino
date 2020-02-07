void run_voice_slider_routine()
{

  // voice select sliders

  for (int j = 0; j <= 15; j++)
  {

    // get the raw value
    raw_voice_slider_values[j] = map(voice_sliders[j].getValue(), 0, 1023, slider_map_low_value, slider_map_high_value);
    /*
    lcd.print("raw vs["); // voice slider
    lcd.print(j);
    lcd.print("] = ");
    lcd.println(raw_voice_slider_values[j]);
*/
    if (raw_voice_slider_values[j] != voice_slider_midinotenum[j])
    {
      voice_slider_midinotenum[j] = raw_voice_slider_values[j];
      voice_slider_values[j] = raw_voice_slider_values[j];
    }

    last_voice_slider_values[j] = voice_slider_values[j];

    if (
        (voice_slider_values[j] >= last_voice_slider_values[j] + slider_step_value) or
        (voice_slider_values[j] <= last_voice_slider_values[j] - slider_step_value))
    {

      if (slider_message_header == "NN")
      {

        // lcd.print("?f");      // clear the LCD
        lcd.print("?x02?y0"); // move cursor to beginning of line 0
        lcd.print("v");
        lcd.print(j + 1);

        if (voice_slider_values[j] < 100)
        {
          lcd.print(" ");
        }

        lcd.print(" ?4");
        lcd.print(":");
        lcd.print(voice_slider_values[j]);
        if (voice_slider_values[j] < 100)
        {
          lcd.print(" ");
        }
        lcd.print("?x00?y1"); // move cursor to beginning of line 1

        // lcd.print("?");
        // lcd.print(this_slider_graphically);

        // Ok, adjust the low and high range values with these combo moves:
        // low range is set with left d-pad and enter
        while (dpad_left.isPressed())
        {

          slider_map_low_value = map(voice_sliders[0].getValue(), 0, 1023, 0, 128);

          // begin display the high and low values for the sliders
          // Serial.println(slider_map_low_value);

          if (slider_map_low_value < 100)
          {
            lcd.print(" ");
          }
          lcd.print("?x09?y1"); // move cursor to line 2, char 10
          lcd.println("hi:");
          lcd.print("?x12?y1"); // move cursor to line 2, char 13
          lcd.print(slider_map_high_value);
          if (slider_map_high_value < 100)
          {
            //            lcd.print(" ");
          }

          // end display the high and low values for the sliders

          midinn_sliderrangelow = slider_map_low_value;

          for (int i = 0; i < 15; i++)
          {
            if (voice_slider_midinotenum[i] <= midinn_sliderrangelow)
            {
              voice_slider_midinotenum[i] = midinn_sliderrangelow;
            }
          }
        }

        // high range is set with right d-pad and enter
        while (dpad_right.isPressed())
        {
          slider_map_high_value = map(voice_sliders[0].getValue(), 0, 1023, 0, 128);

          // begin display the high and low values for the sliders

          lcd.print("?x0?y1");  // move cursor to line 2
          lcd.print("?x00?y1"); // move cursor to line 2, char 1
          lcd.print("?5");
          lcd.print(" lo:");
          lcd.print("?x05?y1"); // move cursor to line 2, char 6
          lcd.print(slider_map_low_value);
          if (slider_map_low_value < 100)
          {
            lcd.print(" ");
          }
          lcd.print("?x09?y1"); // move cursor to line 2, char 10
          lcd.println("hi:");
          lcd.print("?x12?y1"); // move cursor to line 2, char 13
          lcd.print(slider_map_high_value);
          if (slider_map_high_value < 100)
          {
            lcd.print(" ");
          }

          // end display the high and low values for the sliders

          midinn_sliderrangehigh = slider_map_high_value;

          for (int i = 0; i < 15; i++)
          {
            if (voice_slider_midinotenum[i] != midinn_sliderrangehigh)
            {
              voice_slider_midinotenum[i] = midinn_sliderrangehigh;
            }
          }
        }
      }

      lcd.print("?x03?y0"); // move cursor to fader mode display position
    }
  }
}

void resetSliders()
{
  lcd.print("?f");      // clear the LCD
  lcd.print("?x00?y0"); // move cursor to beginning of line 0
  lcd.print("notenum reset   ");
  slider_reset_counter = 0;

  int notenum_count_up_from = 36;

  for (int i = 0; i <= 15; i++)
  {
    voice_slider_midinotenum[i] = notenum_count_up_from + i;
    slider_serial_message_factory("NN", i);
    slider_serial_message_factory("CC", i);
    slider_serial_message_factory("VL", i);
  }
}

void slider_serial_message_factory(const char *slider_message_header, int j)
{

  /*

  the_serial_message = "Z";

  if(enterbutton.isPressed() && (slider_message_header == "VL"))
  {
    the_serial_message += "RV";
  }
  else if(enterbutton.isPressed() && (slider_message_header == "CC"))
  {
    the_serial_message += "RC";
  }
  else if(enterbutton.isPressed() && (slider_message_header == "NN"))
  {
    the_serial_message += "RN";


  }
  else 
  {
    the_serial_message += slider_message_header;
  }

  the_serial_message += ",";
  the_serial_message += j;
  the_serial_message += ",";


  if(slider_message_header == "VL")     
  {
    the_serial_message += voice_slider_midivelocity[j];
  }
  else if(slider_message_header == "CC")
  {
    the_serial_message += voice_slider_midicc[j];
  }
  else if(slider_message_header == "NN")
  {
    the_serial_message += voice_slider_midinotenum[j];
  } 
  else if(slider_message_header == "MC")
  {
    the_serial_message += voice_slider_midichannel[j];
  } 


  the_serial_message += ";";   
  serial_printer(the_serial_message);  
  */
}
