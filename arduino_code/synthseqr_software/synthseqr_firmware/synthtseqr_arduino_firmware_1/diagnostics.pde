  int voiceval = 0;
  int lowest_reading = 1000;
  int highest_reading = 0;
  int average_reading = 0;
  int diagnostic_total = 0;
  int diagnostic_readings[10] = {
    0,0,0,0,0,0,0,0,0,0};
  int save_high = 0;
  int save_low = 0;
  
  
  void run_diagnostics()
  {
  
    if(transport_button_1.isPressed() && param_rec.isPressed())
    {
  
      if(voice_mode_select.isPressed() && transport_button_1.isPressed())
      {
        for(int i=100; i<=115; i++)
        {
          int value = EEPROM.read(i);
          lcd.print("?f");
          lcd.print(i);
          lcd.print(" ");
          lcd.print(value);
          delay(200);
        }
      }
  
      lcd.print("?f");// move cursor to beginning of line 0
      lcd.print("v-select tune up");
      lcd.print("?x00?y1");// move cursor to beginning of line 0
      lcd.print("voice # ?");// move cursor to beginning of line 0
      delay(200);
      voice_select_button_tune_up();
    } 
  }
  
  
  void voice_select_button_tune_up()
  {
  
  
    // step buttons
    for(int i=0; i<=7; i++)
    {
      if(step_buttons[i].uniquePress()){
  
        for(int j=0; j<=7; j++)
        {
          step_leds[j].off();
        }
  
        step_leds[i].on();
        lcd.print("?f");// move cursor to beginning of line 0
        lcd.print("v-select tune up");
        lcd.print("?x00?y1");// move cursor to beginning of line 0
        lcd.print("voice # ? ");// move cursor to beginning of line 0
        // lcd.print("?x8?y1");// move cursor to beginning of line 0
  
        lcd.print(i);
        lcd.print(" ");
        read_voice_select_pin(i);
  
  
      }
    }
  
  
    if(voice_mode_select.uniquePress())
    {
      return;
    } 
    else
    {
      voice_select_button_tune_up();
    }
  }
  
  void read_voice_select_pin(int voice)
  {
  
  
  
  
    for(int i = 1; i<=10; i++)
    {
      int raw_voiceval = voice_select_buttons.getValue();    
      voiceval = raw_voiceval/4;
  
  
      if(voiceval < lowest_reading)
      {
        lowest_reading = voiceval;
      }
  
      if(voiceval > highest_reading)
      {
        highest_reading = voiceval;
      }
  
      // add the reading to the total:
      diagnostic_total= diagnostic_total + (voiceval);       
  
    }
  
  
    // calculate the average:
    average_reading = diagnostic_total / 10;
  
  
  
    if(voiceval > 10)
    {
      lcd.print("?x00?y1");// move cursor to beginning of line 0
  
      lcd.print(voice);
      lcd.print(" ");
  
      lcd.print("h");
      lcd.print(highest_reading);
      lcd.print(" ");
  
      lcd.print("a");
      lcd.print(average_reading);
      lcd.print(" ");
  
      lcd.print("l");
      lcd.print(lowest_reading);
      lcd.print(" ");
  
      save_high = highest_reading;
      save_low = lowest_reading;
  
      // lcd.print(voiceval);
  
      diagnostic_total = 0;    
      highest_reading = 0;
      lowest_reading = 1023;
  
    }
  
    if(transport_button_1.uniquePress())
      // accept the readings, write them to EEPROM and return
    {
  
      int address = 100+voice;
      EEPROM.write(address, save_high);
      delay(50);
      lcd.print("?f");
      int read_high = EEPROM.read(address);
      lcd.print("H ");
      lcd.print(read_high);
      lcd.print(" for V ");
      lcd.print(voice);
      lcd.print(" at ");
      lcd.print(address);
      delay(1000);
  
  
      address = 100+voice+8;
      EEPROM.write(address, save_low);
      delay(50);
      lcd.print("?f");
      int read_low = EEPROM.read(address);
      lcd.print("L ");
      lcd.print(read_low);
      lcd.print(" for V ");
      lcd.print(voice);
      lcd.print(" at ");
      lcd.print(address);
      delay(1000);
  
      return;
    }
    else
    {
      read_voice_select_pin(voice);
    }
  
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  

