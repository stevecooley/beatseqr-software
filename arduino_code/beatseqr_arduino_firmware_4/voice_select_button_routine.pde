void run_voice_select_button_routine()
{
  // voice mode select button

  int alt_button_delay = 150;

  // int tbutton1 = transport_button_1.uniquePress();
  if(voice_mode_select.uniquePress())
  {
    clear_voice_select_leds();
//     voice_mode_select_LED.toggle();
//     delay(100);
//     voice_mode_select_LED.toggle();
    voice_mode++;
    if(voice_mode > voice_mode_total)
    {
      voice_mode = 1;
    }
    switch(voice_mode)
    {
    case 1: 
      {

        // lcd.print("?f");                   // clear the LCD
        lcd.print("?x14?y0");// move cursor to beginning of line 0
//        lcd.print("v-mode:");
//        lcd.print("?x00?y1");// move cursor to beginning of line 0
//        lcd.print("select   ");
        lcd.print("?6");
        lcd.print("S"); // select

        break;



      }
    case 2:
      {
        //  the_serial_message = "ZPL,1;";
//        lcd.print("?f");                   // clear the LCD
        lcd.print("?x14?y0");// move cursor to beginning of line 0
//        lcd.print("voice mode:");
//        lcd.print("mute ");
        
        lcd.print("?6");
        lcd.print("M"); // mute
        
        run_mute_mode();

        break;
      }
    case 3:
      {
        //  the_serial_message = "ZPL,1;";
//        lcd.print("?f");                   // clear the LCD
        lcd.print("?x14?y0");// move cursor to beginning of line 0
        lcd.print("?6"); // voice mode 
        lcd.print("?7"); // solo

        run_solo_mode();

        break;
      }
    }
  }

  switch(voice_mode)
  {
  case 1: // select
    {
      // light up the last voice selected
      clear_voice_select_leds();

      // experimental:flash voice select LEDs to give visual feedback when each voice is executing 
      // 2010-02-17 steve cooley 
      // {

      for(int i=0; i<=7; i++)
      {
        //step_data[pattern_value][last_voice_selected][i]
        if(step_data[current_pattern][i][current_step] == 1)
        {
          voice_select_leds[i].setValue(255);
        } 
      }

      // } end experimental

      voice_mode_pulse_counter = voice_mode_pulse_counter + pulse_adder;
      if(voice_mode_pulse_counter >= 254 || voice_mode_pulse_counter <= 1)
      {
        pulse_adder = pulse_adder * -1;
      }
      voice_select_leds[last_voice_selected].setValue(voice_mode_pulse_counter);


      int voiceval = voice_select_buttons.getValue();

      if(voiceval > 400)
      {
        total= total - readings[index];         
        // read from the sensor:  
        readings[index] = voiceval;
        // add the reading to the total:
        total= total + readings[index];       
        // advance to the next position in the array:  
        index = index + 1;                    

        // if we're at the end of the array...
        if (index >= numReadings)              
          // ...wrap around to the beginning: 
          index = 0;                           

        // calculate the average:
        voiceval = total / numReadings;


        // debug!

        // Serial.println(voiceval);
      }
      // end debug!

      // Serial.println(voiceval);

      // this is all of the voice select buttons
      for(int v = 0; v<=7; v++)
      {
        if(voiceval >= (vselectval_lowerranges[v]) && voiceval <= (vselectval_upperranges[v]) )
        {
          last_voice_selected = v;
          voice_select_leds[v].setValue(255);
          clear_step_leds();
        }
      }
      break;  
    }
  case 2: // mute
    {

      int voiceval = voice_select_buttons.getValue();

      if(voiceval > 100)
      {



        // debug!

        //    Serial.println(voiceval);
      }
      // end debug!

      // Serial.println(voiceval);



      // this is all of the voice select buttons
      for(int v = 0; v<=7; v++)
      {
        if(voiceval >= (vselectval_lowerranges[v]) && voiceval <= (vselectval_upperranges[v]) ) 
        {

          // get the LED value and set the mute_mode_memory accordingly
          if((voice_select_leds[v].getState() == 0))
          {
            mute_mode_memory[v] = 1;
            voice_select_leds[v].on();
            the_serial_message = "ZMU,";
            the_serial_message += v;
            the_serial_message += ",";
            the_serial_message += "1;";
            serial_printer(the_serial_message);
            delay(alt_button_delay);

          }
          else
          {
            mute_mode_memory[v] = 0;
            voice_select_leds[v].off();                      
            the_serial_message = "ZMU,";
            the_serial_message += v;
            the_serial_message += ",";
            the_serial_message += "0;";
            serial_printer(the_serial_message);
            delay(alt_button_delay);

          }

        }
      }

      last_reading = voiceval;
      break;
    }

  case 3: // solo
    {

      int voiceval = voice_select_buttons.getValue();

      if(voiceval > 100)
      {



        // debug!

        //    Serial.println(voiceval);
      }
      // end debug!

      // Serial.println(voiceval);



      // this is all of the voice select buttons
      for(int v = 0; v<=7; v++)
      {
        if(voiceval >= (vselectval_lowerranges[v]) && voiceval <= (vselectval_upperranges[v]) ) 
        {

          // get the LED value and set the mute_mode_memory accordingly
          if((voice_select_leds[v].getState() == 0))
          {
            solo_mode_memory[v] = 1;
            voice_select_leds[v].on();
            the_serial_message = "ZSO,";
            the_serial_message += v;
            the_serial_message += ",";
            the_serial_message += "1;";
            serial_printer(the_serial_message);
            delay(alt_button_delay);

          }
          else
          {
            solo_mode_memory[v] = 0;
            voice_select_leds[v].off();                      
            the_serial_message = "ZSO,";
            the_serial_message += v;
            the_serial_message += ",";
            the_serial_message += "0;";
            serial_printer(the_serial_message);
            delay(alt_button_delay);

          }

        }
      }

      last_reading = voiceval;
      break;
    }

  }

}


void clear_voice_select_leds()
{

  for(int i=0; i<=7; i++)
  {
    voice_select_leds[i].setValue(0);
  }
  return;
}



void run_mute_mode()
{

  clear_voice_select_leds();
  // recall mute memory and set the LEDs accordingly
  for(int v = 0; v<=7; v++)
  {

    if(mute_mode_memory[v] == 0)
    {
      voice_select_leds[v].off();
    }
    else
    {
      voice_select_leds[v].on();
    }


  }

  return; 
}


void run_solo_mode()
{

  clear_voice_select_leds();
  // recall mute memory and set the LEDs accordingly
  for(int v = 0; v<=7; v++)
  {
    if(solo_mode_memory[v] == 0)
    {
      voice_select_leds[v].off();
    }
    else
    {
      voice_select_leds[v].on();
    }
  }
  return; 
}



