void run_voice_slider_routine()
{
  // slider mode select button

  // int tbutton1 = transport_button_1.uniquePress();
  if(slider_mode_select.uniquePress()){

    for(int i=0; i<=7; i++)
    {
      VL_cleared_to_update_values[i]=false;
      CC_cleared_to_update_values[i]=false;
      NN_cleared_to_update_values[i]=false;
    }

    slider_mode_select_LED.toggle();
    delay(100);
    slider_mode_select_LED.toggle();
    slider_mode++;
    if(slider_mode > slider_mode_total)
    {
      slider_mode = 1;
    }

    switch(slider_mode)
    {
    case 1: 
      {
        // velocity
        //  the_serial_message = "ZPL,1;";
        // lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("faders:");
        lcd.print("velocity ");
        delay(300);
        slider_message_header = "VL";
        // 6,144
        slider_map_low_value = -2;
        slider_map_high_value = 127;
        slider_step_value = 2;

        break;
      }
    case 2:
      {
        //  the_serial_message = "ZPL,1;";
        // lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("fader mode:");
        lcd.print("CC   ");
        delay(300);     




        slider_message_header = "CC";
        slider_map_low_value = 0;
        slider_map_high_value = 127;
        slider_step_value = 5;

        break;
      }
    case 3:
      {
        //  the_serial_message = "ZPL,1;";
        //  lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("faders:");
        lcd.print("notenum  ");
        delay(300);




        slider_message_header = "NN";
        slider_map_low_value = 35;
        slider_map_high_value = 61;
        //        slider_map_low_value = 0;
        //        slider_map_high_value = 127;
        slider_step_value = 1;
        break;
      } 
    }
  }

  // 2010-01-01 steve cooley
  // we need a way to trigger a reset command for midi note numbers.
  // not sure this is the right way.

  if(slider_mode_select.isPressed())
  {
    slider_reset_counter++;
    if(slider_reset_counter >= 500)
    {
      resetSliders();

    }
  }

  // voice select sliders

    for(int j=0; j<=7; j++)
  {

    // get the raw value
    raw_voice_slider_values[j] = map(voice_sliders[j].getValue(), 0,1023,slider_map_low_value,slider_map_high_value);

    int this_slider_graphically = map(raw_voice_slider_values[j], slider_map_low_value, slider_map_high_value, 2, 7);



    if(slider_message_header == "VL")     
    {
      if((raw_voice_slider_values[j] == voice_slider_midivelocity[j]))
      {
        VL_cleared_to_update_values[j]=true;
      }
    }

    if(slider_message_header == "CC")     
    {
      if((raw_voice_slider_values[j] == voice_slider_midicc[j]))
      {
        CC_cleared_to_update_values[j]=true;
      }
    }

    if(slider_message_header == "NN")     
    {
      if((raw_voice_slider_values[j] == voice_slider_midinotenum[j]))
      {
        NN_cleared_to_update_values[j]=true;
      }
    }



    if(VL_cleared_to_update_values[j] == true)
    {
      voice_slider_midivelocity[j] = raw_voice_slider_values[j];
      voice_slider_values[j] = raw_voice_slider_values[j];
    }

    if(CC_cleared_to_update_values[j] == true)
    {
      voice_slider_midicc[j] = raw_voice_slider_values[j];
      voice_slider_values[j] = raw_voice_slider_values[j];
    }

    if(NN_cleared_to_update_values[j] == true)
    {
      voice_slider_midinotenum[j] = raw_voice_slider_values[j];
      voice_slider_values[j] = raw_voice_slider_values[j];
    }


    if(
    (voice_slider_values[j] >= last_voice_slider_values[j]+slider_step_value) 
      or 
      (voice_slider_values[j] <= last_voice_slider_values[j]-slider_step_value)
      )
    {

      // assemble the serial messages... 

      slider_serial_message_factory(slider_message_header, j);


      last_voice_slider_values[j] = voice_slider_values[j];

      // start commenting if this starts to affect performance

      if(slider_message_header == "VL")
      {
        //lcd.print("?f");                   // clear the LCD      
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("voice ");
        lcd.print(j+1);
        lcd.print(" VL : ");
        lcd.print(voice_slider_values[j]);
        if(voice_slider_values[j] < 100)
        {
          lcd.print(" ");
        }
        //        lcd.print("?x00?y1");// move cursor to beginning of line 1

        //        lcd.print("?");      
        //        lcd.print(this_slider_graphically);
      }

      if(slider_message_header == "CC")
      {
        //lcd.print("?f");                   // clear the LCD      
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("voice ");
        lcd.print(j+1);
        lcd.print(" CC : ");
        lcd.print(voice_slider_values[j]);
        if(voice_slider_values[j] < 100)
        {
          lcd.print(" ");
        }
        //        lcd.print("?x00?y1");// move cursor to beginning of line 1

        //        lcd.print("?");      
        //        lcd.print(this_slider_graphically);
      }

      // end commenting here if that was affecting performance

      if(slider_message_header == "NN")
      {
        //lcd.print("?f");                   // clear the LCD      
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("voice ");
        lcd.print(j+1);
        lcd.print(" NN : ");
        lcd.print(voice_slider_values[j]);
        if(voice_slider_values[j] < 100)
        {
          lcd.print(" ");
        }
        //        lcd.print("?x00?y1");// move cursor to beginning of line 1

        //        lcd.print("?");      
        //        lcd.print(this_slider_graphically);
      }
    }
  } 
}


void resetSliders()
{
  // lcd.print("?f");                   // clear the LCD
  lcd.print("?x00?y0");// move cursor to beginning of line 0
  lcd.print("notenum reset   ");
  slider_reset_counter = 0;   

  int notenum_count_up_from = 36;

  for(int i=0; i<=7; i++)
  {
    voice_slider_midinotenum[i] = notenum_count_up_from + i;
    slider_serial_message_factory("NN",i);
  }

}

void slider_serial_message_factory(const char* slider_message_header, int j)
{
  the_serial_message = "Z";
  the_serial_message += slider_message_header;
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


  the_serial_message += ";";   
  serial_printer(the_serial_message);      

}

