void run_pattern_select_routine()
{



  // pattern select
  for(int pattern=0; pattern<=3; pattern++)
  {




    if(pattern_select_buttons[pattern].uniquePress())
    {
      // go_to_pattern(pattern to go to, silent?)
      go_to_pattern(pattern, 0);
    }

    // pattern copy
    if(pattern_select_buttons[pattern].isPressed())
    {
      Serial.print(last_serial);
      pattern_select_button_pressing_counter++;
      if(pattern_select_button_pressing_counter >= 200)
      {
        // lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 1
        lcd.print("Copy pattrn ");
        lcd.print(pattern+1); 
        lcd.print(" to");

        wait_for_copy_command(pattern);
        pattern_select_button_pressing_counter = 0;


      }
    }
  }

  if(pattern_select_buttons[0].isPressed() && pattern_select_buttons[3].isPressed())
  {


    if(extended_step_length_mode == 1)
    {
      // lcd.print("?f");                   // clear the LCD
      lcd.print("?x00?y0");// move cursor to beginning of line 0
      lcd.print("single pattern  ");
      extended_step_length_mode = 0;
      pattern_select_leds[0].on();
      pattern_select_leds[1].on();
      pattern_select_leds[2].on();
      pattern_select_leds[3].on();
      delay(50);
      pattern_select_leds[3].off();
      delay(50);
      pattern_select_leds[2].off();
      delay(50);
      pattern_select_leds[1].off();
      delay(50);
      pattern_select_leds[0].off();
      delay(50);
      go_to_pattern(0,1);
    }
    else
    {
      // lcd.print("?f");                   // clear the LCD
      lcd.print("?x00?y0");// move cursor to beginning of line 0
      lcd.print("chain patterns  ");
      extended_step_length_mode = 1;
      pattern_select_leds[0].off();
      pattern_select_leds[1].off();
      pattern_select_leds[2].off();
      pattern_select_leds[3].off();
      delay(50);
      pattern_select_leds[0].on();
      delay(50);
      pattern_select_leds[1].on();
      delay(50);
      pattern_select_leds[2].on();
      delay(50);
      pattern_select_leds[3].on();
      delay(50);
      go_to_pattern(0,1);


    }

  }

}

void run_auto_pattern_select_routine(int inByte)
{

  if(inByte >= 15)
  {
    current_pattern++;
    if(current_pattern >= 4)
    {
      current_pattern = 0;
    }

    // go_to_pattern(pattern to go to, silent?)
    go_to_pattern(current_pattern,1);
  }

}

void go_to_pattern(int pattern, int silent)
{
  pattern_select_leds[0].off();
  pattern_select_leds[1].off();
  pattern_select_leds[2].off();
  pattern_select_leds[3].off();
  pattern_select_leds[pattern].toggle();
  pattern_value = pattern;
  pattern_select_button_pressing_counter = 0;

  // "P"age / pattern "S"elect 
  the_serial_message = "ZPS,";
  the_serial_message += pattern_value;
  the_serial_message += ";";
  serial_printer(the_serial_message);

  current_pattern = pattern_value;  

  if(silent == 0)
  {
    // lcd.print("?f");                   // clear the LCD
    lcd.print("?x00?y0");// move cursor to beginning of line 1
    lcd.print("pattern ");
    lcd.print(pattern_value+1);
    lcd.print("       ");
  }
  for(int voice=0; voice<=7; voice++)
  {
    for(int step=0; step<=15; step++)
    {
      step_value = step_data[pattern][voice][step];

      if(step_value == 1)
      {
        step_leds[step].on();
      }
      else
      {
        step_leds[step].off();
      }

    }
  }
}

void wait_for_copy_command(int pattern_to_copy_from)
{


  int not_told_which_pattern_to_copy_to = true;
  int ended_on;

  while(not_told_which_pattern_to_copy_to)
  {


    for(int pattern=0; pattern<=3; pattern++)
    {
      if(pattern_select_buttons[pattern].uniquePress())
      {
        /*
        the_serial_message = "ZPS,";
         the_serial_message += pattern;
         the_serial_message += ";";   
         serial_printer(the_serial_message);
         */


        pattern_select_leds[0].off();
        pattern_select_leds[1].off();
        pattern_select_leds[2].off();
        pattern_select_leds[3].off();
        // pattern_select_leds[pattern].blink(250,3);
        pattern_select_leds[pattern].on();
        pattern_value = pattern;
        pattern_select_button_pressing_counter = 0;


        // lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 1
        lcd.print("pattern ");
        lcd.print(pattern_value+1);
        lcd.print("       ");
        
        for(int voice=0; voice<=7; voice++)
        {
          for(int step=0; step<=15; step++)
          {
            step_data[pattern][voice][step] = step_data[pattern_to_copy_from][voice][step];
            // delay(5);
            // copy_step_data(pattern,voice,step,step_data[pattern_to_copy_from][voice][step]);

          }
        }
        not_told_which_pattern_to_copy_to = false;
        ended_on = pattern;    
        the_serial_message = "ZPC,";
        the_serial_message += pattern_to_copy_from;
        the_serial_message += ",";
        the_serial_message += pattern;
        the_serial_message += ";";   
        serial_printer(the_serial_message);
      }

    }
    // "P"age / pattern "S"elect 

  }

}

/*
void wait_for_extended_step_length_command()
 {
 
 int not_told_which_pattern_mode_to_use = true;
 int ended_on;
 
 while(not_told_which_pattern_mode_to_use)
 {
 for(int patterns=0; patterns<=3; patterns++)
 {
 if(pattern_select_buttons[patterns].uniquePress())
 {
 
 
 
 pattern_select_leds[0].off();
 pattern_select_leds[1].off();
 pattern_select_leds[2].off();
 pattern_select_leds[3].off();
 // pattern_select_leds[pattern].blink(250,3);
 pattern_select_leds[patterns].on();
 pattern_select_button_pressing_counter = 0;
 
 
 lcd.print("?f");                   // clear the LCD
 lcd.print("?x00?y0");// move cursor to beginning of line 1
 lcd.print("patterns: ");
 lcd.print(pattern_value+1);
 
 
 not_told_which_pattern_to_copy_to = false;
 extended_step_length_mode = 1;
 patterns_to_play_in_a_row = pattern;
 }
 
 }
 // "P"age / pattern "S"elect 
 
 }
 
 }
 */


