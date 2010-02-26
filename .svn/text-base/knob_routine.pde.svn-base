void run_knob_routine()
{


  // int tbutton1 = transport_button_1.uniquePress();


  if(knob_mode_select.uniquePress()){

    // invalidate that the raw data is what should be transmitting.

    knobs_cleared_to_update[0]=0;
    knobs_cleared_to_update[1]=0;

    /*
     lcd.print("?f");                   // clear the LCD
     lcd.print("?x00?y0");// move cursor to beginning of line 0
     lcd.print("knobs not cleared");
     delay(500);
     */

    knob_mode_select_LED.toggle();
    delay(100);
    knob_mode_select_LED.toggle();
    knob_mode++;
    if(knob_mode > knob_mode_total)
    {
      knob_mode = 1;
    }

    switch(knob_mode)
    {
    case 1: 
      {
        // velocity
        //  the_serial_message = "ZPL,1;";
        lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("knob mode:");
        lcd.print("?x00?y1");// move cursor to beginning of line 1
        lcd.print("?0");
        lcd.print("t-coarse");
        lcd.print(" swing");
        lcd.print("?1");
        break;
      }
    case 2:
      {
        //  the_serial_message = "ZPL,1;";
        lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("knob mode:");
        lcd.print("?x00?y1");// move cursor to beginning of line 1
        lcd.print("?0");
        lcd.print("t-fine");
        lcd.print(" swing");
        lcd.print("?1");
        break;
      }
 

    }
  }



  // get the raw data.
  raw_knob_values[1] = swing.getValue();
  raw_knob_values[0] = tempo.getValue();


  // swing
  // debug
  //     Serial.println(raw_knob_values[1]);

  this_swing = map(raw_knob_values[1], 1, 1024, 200, -1);

  if((this_swing >= (last_swing+2)) || (this_swing <= (last_swing-2)))
  {     
    the_serial_message = "ZSW,";
    the_serial_message += this_swing;
    the_serial_message += ";";
    serial_printer(the_serial_message);

    last_swing = this_swing;

    lcd.print("?f");                   // clear the LCD
    lcd.print("?x00?y0");// move cursor to beginning of line 0
    lcd.print("Tempo: ");
    lcd.print(this_mapped_tempo_value);
    lcd.print(" + ");
    lcd.print(the_mapped_tempo_adjust_value);
    lcd.print("?x00?y1");// move cursor to beginning of line 1
    lcd.print("Swing: ");
    lcd.print(this_swing);    


  }




  //
  //
  // TEMPO
  //  and
  // TEMPO ADJUST
  //
  //

  if(knob_mode == 1) // Tempo
  {

    this_mapped_tempo_value = map(raw_knob_values[0], 1, 1024, upper_BPM_number, lower_BPM_number);

    if(this_mapped_tempo_value == last_mapped_tempo_value)
    {
      knobs_cleared_to_update[0] = 1;

    }


    if(knobs_cleared_to_update[0] == 1)
    {

      this_mapped_tempo_value = map(raw_knob_values[0], 1, 1024, upper_BPM_number, lower_BPM_number);



      if(last_mapped_tempo_value != this_mapped_tempo_value)
      {

        the_serial_message = "ZTM,";
        the_serial_message += this_mapped_tempo_value;
        the_serial_message += ";";
        serial_printer(the_serial_message);

        last_raw_tempo_value = this_raw_tempo_value;

        last_mapped_tempo_value = this_mapped_tempo_value;
        this_BPM_is_new = 1;
      }

      // debug...
      // Serial << "this_tempo_value " << this_tempo_value << " the_tempo_value " << the_tempo_value << " last_tempo_value " << last_tempo_
      // alue << "\r\n";

      // if(the_serial_message != last_serial)
      // {     


      if(this_BPM_is_new == 1)
      {
        lcd.print("?f");                   // clear the LCD
        lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("Tempo: ");
        lcd.print(this_mapped_tempo_value);
        lcd.print(" + ");
        lcd.print(the_mapped_tempo_adjust_value);
        lcd.print("?x00?y1");// move cursor to beginning of line 1
        lcd.print("Swing: ");
        lcd.print(this_swing);  


        // calculate the new milliseconds per step so we know how long the chase lights should take for each step button.

        float millis_per_beat = (float)sixty/(this_mapped_tempo_value + the_mapped_tempo_adjust_value);    
        int nice_millis_per_beat = int(millis_per_beat * 1000);

        float millis_per_step = (float)fifteen/(this_mapped_tempo_value + the_mapped_tempo_adjust_value);    
        int nice_millis_per_step = int(millis_per_step * 1000);

        //  lcd.print(" ");
        //  lcd.print(nice_millis_per_step);


        this_BPM_is_new = 0;

      }
    }

  }


  if(knob_mode == 2) // Tempo adjust
  {

    this_tempo_adjust_value = map(raw_knob_values[0], 1, 1024, 512, -512);

    if(this_tempo_adjust_value == last_tempo_adjust_value)
    {
      knobs_cleared_to_update[0] = 1;
    }


    if(knobs_cleared_to_update[0] == 1)
    {
      the_tempo_adjust_value = this_tempo_adjust_value;
      the_tempo_adjust_float_value = (the_tempo_adjust_value/100);


      // float send_tempo = map(this_tempo,1,1024,-10.00, 10.00);



      if((the_tempo_adjust_value >= (last_tempo_adjust_value+5)) || (the_tempo_adjust_value <= (last_tempo_adjust_value-5)))
      {
        // Tempo Adjust
        the_serial_message = "ZTA,";
        the_serial_message += the_tempo_adjust_float_value;
        the_serial_message += ";";
        serial_printer(the_serial_message);

        the_mapped_tempo_adjust_value = map(the_tempo_adjust_value, -500, 500, -5, 5);

        if(last_mapped_tempo_adjust_value != the_mapped_tempo_adjust_value)
        {
          last_tempo_adjust_value = the_tempo_adjust_value;
          last_mapped_tempo_adjust_value = the_mapped_tempo_adjust_value;
          this_BPM_is_new = 1;
        }

        if(this_BPM_is_new == 1)
        {
          lcd.print("?f");                   // clear the LCD
          lcd.print("?x00?y0");// move cursor to beginning of line 0
          lcd.print("Tempo: ");
          lcd.print(this_mapped_tempo_value);
          lcd.print(" + ");
          lcd.print(the_mapped_tempo_adjust_value);
          lcd.print("?x00?y1");// move cursor to beginning of line 1
          lcd.print("Swing: ");
          lcd.print(this_swing);  



          // calculate the new milliseconds per step so we know how long the chase lights should take for each step button.

          float millis_per_beat = (float)sixty/(this_mapped_tempo_value + the_mapped_tempo_adjust_value);    
          int nice_millis_per_beat = int(millis_per_beat * 1000);

          float millis_per_step = (float)fifteen/(this_mapped_tempo_value + the_mapped_tempo_adjust_value);    
          int nice_millis_per_step = int(millis_per_step * 1000);

          millis_per_step = nice_millis_per_step;

          // lcd.print(" ");
          // lcd.print(nice_millis_per_step);


          this_BPM_is_new = 0;        
        }
      }
    } 
  }
}


