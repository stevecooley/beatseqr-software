void run_transport_button_routine()
{

  // transport buttons

  // int tbutton1 = transport_button_1.uniquePress();
  if(transport_button_1.uniquePress()){
    transport_led_1.toggle();



    if(transport_led_1.getState() == true)
    { 
      the_serial_message = "ZPL,1;";
      serial_printer(the_serial_message);

      lcd.print("?x00?y0");// move cursor to beginning of line 0
      lcd.print("?0");
      lcd.print(" ");

      // lcd.print("?x00?y1");// move cursor to beginning of line 1
      // lcd.print(now);
      last_step_time=now;
      chase_lights_status = 1;

    }
    else
    {// pause
      chase_lights_status = 0;

      if( extended_step_length_mode == 1)
      {
        current_pattern = 0;
        go_to_pattern(0,1);      
      }

      if(voice_mode == 1)
      {
        for(int i=0; i<=7; i++)
        {
           voice_select_leds[i].setValue(0);
        }
      }
      the_serial_message = "ZPL,0;";
      serial_printer(the_serial_message);

      lcd.print("?x00?y0");// move cursor to beginning of line 0
      lcd.print("O");
      lcd.print(" ");
    }
  }




}



void run_chase_lights(unsigned long now)
{



  if(chase_lights_status == 1)
  {
    /*
    if(now > (last_step_time + millis_per_step))
     {
     lcd.print("?x00?y0");// move cursor to beginning of line 0
     lcd.print(step_counter);
     step_counter++;
     
     if(step_counter > 16){
     step_counter=1;
     }
     
     last_step_time = now;  
     }
     
     */



    // debug!
    /*
    lcd.print("?f");                   // clear the LCD
     lcd.print("?x00?y0");// move cursor to beginning of line 0
     lcd.print(sixty);
     lcd.print(" ");
     
     lcd.print(this_mapped_tempo_value);
     lcd.print(" ");
     
     lcd.print(nice_millis_per_beat);
     lcd.print(" ");
     lcd.print(nice_millis_per_step);
     */


    /*
    if(millis_diff >= play_started_at + millis_per_beat)
     {
     play_started_at = this_millis;
     step_counter++;
     if(step_counter >= 16)
     {
     step_counter = 1;
     }
     
     
     
     }
     */

    // delay(100);

    // example
    // 60 seconds / 120 bpm = .5 seconds per beat = 500 millis per beat / 4 steps per beat = 125 millis per step


    // step_leds[mod_millis].toggle();




    //   this_mapped_tempo_value
  } 


}

