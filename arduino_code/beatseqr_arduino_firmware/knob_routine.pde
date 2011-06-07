

void run_knob_routine()
{





  // int tbutton1 = transport_button_1.uniquePress();


  if(knob_mode_select.uniquePress() && (demoMode == false)){

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
        // lcd.print("?f");                   // clear the LCD
        // lcd.print("?x00?y0");// move cursor to beginning of line 0
        // lcd.print("knob mode:");

        lcd.print("?y1");// move cursor to beginning of line 1        
        lcd.print("?x04");

        /*

         lcd.print("?0");
         lcd.print("t-coarse");
         lcd.print(" swing");
         lcd.print("?1");
         */

        break;
      }
    case 2:
      {
        //  the_serial_message = "ZPL,1;";
        // lcd.print("?f");                   // clear the LCD
        // lcd.print("?x00?y0");// move cursor to beginning of line 0
        // lcd.print("knob mode:");

        lcd.print("?y1");// move cursor to beginning of line 1

        if(this_mapped_tempo_value < 100)
        {
          lcd.print("?x07");
        }
        else
        {
          lcd.print("?x08");
        }

        /*
        lcd.print("?0");
         lcd.print("t-fine");
         lcd.print(" swing");
         lcd.print("?1");
         */

        break;
      }


    }
  }

  // 2010-01-01 steve cooley
  // we need a way to trigger a reset command for midi note numbers.
  // not sure this is the right way.

  //    if(slider_mode_select.isPressed())
  if(knob_mode_select.isPressed())
  {
    slider_reset_counter++;
    if(slider_reset_counter >= 500)
    {
      resetSliders();

    }
  }



  // get the raw data.
  raw_knob_values[1] = swing.getValue();



  // swing
  // debug
  //     Serial.println(raw_knob_values[1]);

  int upper_swing_value;

  if(demoMode == true)
  {
    upper_swing_value = 61;
  }
  else
  {
    upper_swing_value = 201;
  }

  this_swing = map(raw_knob_values[1], 1, 1024, upper_swing_value, -1);

  if((this_swing >= (last_swing+2)) || (this_swing <= (last_swing-2)))
  {     
    the_serial_message = "ZSW,";
    the_serial_message += this_swing;
    the_serial_message += ";";
    serial_printer(the_serial_message);

    last_swing = this_swing;

    //     lcd.print("?f");                   // clear the LCD
    //     lcd.print("?x00?y0");// move cursor to beginning of line 0

    lcd.print("?y1");// move cursor to beginning of line 1
    // lcd.print("bpm:");
    // lcd.print(this_mapped_tempo_value);
    // if(the_mapped_tempo_adjust_value >= 0)
    // {
    //   lcd.print("+");
    // }
    // lcd.print(the_mapped_tempo_adjust_value);

    lcd.print("?x09");
    lcd.print(" Sw:");
    lcd.print(this_swing);    
    if(this_swing < 100)
    {
      lcd.print(" ");
    }

  }



  //
  //
  // TEMPO
  //  and
  // TEMPO ADJUST
  //
  //

  // get the raw data.
  raw_knob_values[0] = tempo.getValue();


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
        //  lcd.print("?f");                   // clear the LCD
        //  lcd.print("?x00?y0");// move cursor to beginning of line 0
        lcd.print("?x00?y1");// move cursor to beginning of line 1
        lcd.print("bpm:");
        lcd.print(this_mapped_tempo_value);
        if(the_mapped_tempo_adjust_value >= 0)
        {
          lcd.print("+");
        }
        lcd.print(the_mapped_tempo_adjust_value);
        lcd.print(" ");

        // return the cursor back to the tempo adjust point to continue indicating that 
        // the knob mode is tempo 

        lcd.print("?x04");


        // lcd.print(" Sw:");
        // lcd.print(this_swing);  
        // if(this_swing < 100)
        // {
        //   lcd.print(" ");
        // }
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



      if((the_tempo_adjust_value >= (last_tempo_adjust_value+10)) || (the_tempo_adjust_value <= (last_tempo_adjust_value-10)))
      {
        // Tempo Adjust
        the_serial_message = "ZTA,";
        the_serial_message += raw_knob_values[0];
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
          // lcd.print("?f");                   // clear the LCD
          // lcd.print("?x00?y0");// move cursor to beginning of line 0
          lcd.print("?x00?y1");// move cursor to beginning of line 1
          lcd.print("bpm:");
          lcd.print(this_mapped_tempo_value);
          if(the_mapped_tempo_adjust_value >= 0)
          {
            lcd.print("+");
          }
          lcd.print(the_mapped_tempo_adjust_value);
          // lcd.print(" Sw:");
          // lcd.print(this_swing);  
          // if(this_swing < 100)
          // {
          lcd.print(" ");
          // }

          // return the cursor back to the tempo adjust point to continue indicating that 
          // the knob mode is tempo adjust
          if(this_mapped_tempo_value < 100)
          {
            lcd.print("?x07");
          }
          else
          {
            lcd.print("?x08");
          }

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











