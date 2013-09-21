void run_step_button_routine()
{



  read_step_memory(last_voice_selected, pattern_value);

  // step buttons
  for(int i=0; i<=15; i++)
  {
    if(step_buttons[i].uniquePress()){
      step_leds[i].toggle();
      if(step_leds[i].getState() == 1)
      {
        step_value = 1;
      }
      else
      {
        step_value = 0;
      } 

      step_data[pattern_value][last_voice_selected][i] = step_value;
      //Serial << "pattern " << pattern_value << " voice " << last_voice_selected << " step " << i << " value " << step_value << "\r\n";
      /*
      if(pattern_value < 10)
       {
       pattern_padding = "0";
       }
       else
       {
       pattern_padding = "";
       }
       */

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
  }


  if(step_buttons[0].isPressed() && step_buttons[15].isPressed()) // clear the pattern for this voice
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
    }
    delay(250);
  }

  if(step_buttons[0].isPressed() && step_buttons[11].isPressed()) // clear the entire pattern
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
      }
    }
    delay(250);
  }

}

void read_step_memory(int voice, int pattern)
{
  for(int i=0; i<=15; i++)
  {
    int this_step = step_data[pattern][voice][i];

    if(this_step == 1)
    {
      step_leds[i].on();
    }
    else
    {
      step_leds[i].off();
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

  // step_data[pattern_value][last_voice_selected][i] = step_value;
  //Serial << "pattern " << pattern_value << " voice " << last_voice_selected << " step " << i << " value " << step_value << "\r\n";

  /*
 if(pattern_value < 10)
   {
   pattern_padding = "0";
   }
   else
   {
   pattern_padding = "";
   }
   
   if(step < 10)
   {
   step_padding = "0";
   }
   else
   { 
   step_padding = "";
   }
   */

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


