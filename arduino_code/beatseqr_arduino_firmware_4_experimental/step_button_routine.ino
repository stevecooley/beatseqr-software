void run_step_button_routine()
{



 for (int i=0; i<number_of_step_buttons; i++)
  {
    // Update state of all buitton
    step_buttons[i].Update();
  }


  read_step_memory(last_voice_selected, pattern_value);

  // step buttons
  for(int i=0; i<=15; i++)
  {
    if(step_buttons[i].clicks == 1)
    {
      step_leds[i].toggle();
      if(step_leds[i].getState() == 1)
      {
        step_value = 1;
        flam_value = 0;
      }
      else
      {
        step_value = 0;
        flam_value = 0;
      } 

      step_data[pattern_value][last_voice_selected][i] = step_value;
      flam_data[pattern_value][last_voice_selected][i] = flam_value;
     

      if(i < 10)
      {
        step_padding = "0";
      }
      else
      { 
        step_padding = "";
      }


      // SD = "S"tep "D"ata
      the_serial_message = "ZSD,";
      // the_serial_message += pattern_padding;
      the_serial_message += pattern_value;
      the_serial_message += ",";
      the_serial_message += last_voice_selected;
      the_serial_message +=",";
      the_serial_message += step_padding;
      the_serial_message += i;
      the_serial_message += ",";
      the_serial_message += step_value;
      the_serial_message += ";";   
      serial_printer(the_serial_message);

    }

// flam?

if(step_buttons[i].clicks <= -1)
    {
      if(((millis()/300)%2) == 1)
      {
        step_leds[i].toggle();
      }

      if(flam_data[pattern_value][last_voice_selected][i] == 0)
      {
        flam_value = 1;
      }
      else
      {
        flam_value = 0;
      } 

      flam_data[pattern_value][last_voice_selected][i] = flam_value;
     

      if(i < 10)
      {
        step_padding = "0";
      }
      else
      { 
        step_padding = "";
      }


      // SD = "S"tep "D"ata
      the_serial_message = "ZSF,";
      // the_serial_message += pattern_padding;
      the_serial_message += pattern_value;
      the_serial_message += ",";
      the_serial_message += last_voice_selected;
      the_serial_message +=",";
      the_serial_message += step_padding;
      the_serial_message += i;
      the_serial_message += ",";
      the_serial_message += flam_value;
      the_serial_message += ";";   
      serial_printer(the_serial_message);

    }
// end flam!
    
  }


  if((step_buttons[0].depressed) && (step_buttons[15].depressed)) // clear the pattern for this voice
  {
    for(int i=0; i<=15; i++)
    {

      if(i < 10)
      {
        step_padding = "0";
      }
      else
      { 
        step_padding = "";
      }

      step_data[pattern_value][last_voice_selected][i] = 0;
      flam_data[pattern_value][last_voice_selected][i] = 0;
      step_leds[i].off();

      // SD = "S"tep "D"ata
      the_serial_message = "ZSD,";
      // the_serial_message += pattern_padding;
      the_serial_message += pattern_value;
      the_serial_message += ",";
      the_serial_message += last_voice_selected;
      the_serial_message +=",";
      the_serial_message += step_padding;
      the_serial_message += i;
      the_serial_message += ",";
      the_serial_message += 0;
      the_serial_message += ";";   
      serial_printer(the_serial_message);   
      
      
      // SD = "S"tep "F"lam
      the_serial_message = "ZSF,";
      // the_serial_message += pattern_padding;
      the_serial_message += pattern_value;
      the_serial_message += ",";
      the_serial_message += last_voice_selected;
      the_serial_message +=",";
      the_serial_message += step_padding;
      the_serial_message += i;
      the_serial_message += ",";
      the_serial_message += 0;
      the_serial_message += ";";   
      serial_printer(the_serial_message);   
    }
    // delay(250);
  }

  if((step_buttons[0].depressed) && (step_buttons[11].depressed)) // clear the entire pattern
  {
    for(int v=0; v<=7; v++)
    {
      for(int i=0; i<=15; i++)
      {

        if(i < 10)
        {
          step_padding = "0";
        }
        else
        { 
          step_padding = "";
        }

        step_data[pattern_value][v][i] = 0;
        flam_data[pattern_value][v][i] = 0;
        
        step_leds[i].off();

        // SD = "S"tep "D"ata
        the_serial_message = "ZSD,";
        // the_serial_message += pattern_padding;
        the_serial_message += pattern_value;
        the_serial_message += ",";
        the_serial_message += v;
        the_serial_message +=",";
        the_serial_message += step_padding;
        the_serial_message += i;
        the_serial_message += ",";
        the_serial_message += 0;
        the_serial_message += ";";   
        serial_printer(the_serial_message);
        
        // SD = "S"tep "F"lam
        the_serial_message = "ZSF,";
        // the_serial_message += pattern_padding;
        the_serial_message += pattern_value;
        the_serial_message += ",";
        the_serial_message += v;
        the_serial_message +=",";
        the_serial_message += step_padding;
        the_serial_message += i;
        the_serial_message += ",";
        the_serial_message += 0;
        the_serial_message += ";";   
        serial_printer(the_serial_message);   
        
      }
    }
   //  delay(250);
  }

}

void read_step_memory(int voice, int pattern)
{
  for(int i=0; i<=15; i++)
  {
    int this_step = step_data[pattern][voice][i];
    int this_flam = flam_data[pattern][voice][i];

    if(this_step == 1)
    {
      step_leds[i].on();
    }
    else
    {
      step_leds[i].off();
    } 
    
    if(this_flam == 1)
    {
      if(((millis()/200)%2) == 1)
      step_leds[i].toggle();
    }
  
  }
}

void clear_step_leds()
{
  for(int i=0; i<=15; i++)
  {
    step_leds[i].off();
  }
}

void copy_step_data(int pattern_value, int voice, int step, int step_value)
{

  // SD = "S"tep "D"ata
  the_serial_message = "ZSD,";
  // the_serial_message += pattern_padding;
  the_serial_message += pattern_value;
  the_serial_message += ",";
  the_serial_message += voice;
  the_serial_message +=",";
  // the_serial_message += step_padding;
  the_serial_message += step;
  the_serial_message += ",";
  the_serial_message += step_value;
  the_serial_message += ";";   
  serial_printer(the_serial_message);


}


