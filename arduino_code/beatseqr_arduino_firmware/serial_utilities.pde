void serial_printer(const char* message)
{
  Serial.print(message);
  // last_serial = message;

  return;
}


void run_LCD_setup_routine()
{

  pinMode(lcdTxPin, OUTPUT);
  lcd.begin(9600);      // 9600 baud is chip comm speed
  //   lcd.print("?c0");
  lcd.print("?c1"); // blinky

  lcd.print("?Bff");    // set backlight to ff hex, maximum brightness     
  delay(500);                // pause to allow LCD EEPROM to program

  // lcd.print("?s6");     // set tabs to six spaces
  // delay(1000);               // pause to allow LCD EEPROM to program

  lcd.print("?D010181C1E1E1C1810");       // play
  delay(100);
  lcd.print("?D1001B1B1B1B1B1B00");       // stop
  delay(100);                             // delay to allow write to EEPROM

  lcd.print("?D21214181004040407");  // VL
  delay(100); 

  lcd.print("?D31C10101C07040407");  // CC
  delay(100); 

  lcd.print("?D4121A1612090D0B09");  // NN
  delay(100); 

  lcd.print("?D504040E0404111B15");  // slider mode
  delay(100); 

  
  lcd.print("?D61111110A04111B15");  // voice select button aka "VM", voice mode
   delay(100); 
   
   
   lcd.print("?D71F19171B1D1D131F");  // Solo... we'll use S for "select" and "M" for mute
   delay(100); 
  






  // see moderndevice.com for a handy custom char generator (software app)


  lcd.print("?f");                   // clear the LCD
  lcd.print("?x00?y1");// move cursor to beginning of line 1
  lcd.print("beatseqr v3");

  lcd.print("?x00?y0");// move cursor to beginning of line 0
  lcd.print("firmware v");
  lcd.print(build_number);
  delay(1500);
  //   pinMode(66, INPUT);  
  //   pinMode(67, INPUT);

  lcd.print("?f");                   // clear the LCD
}

void process_incoming_serial()
{
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // lcd.print("?x00?y0");// move cursor to beginning of line 0    
    // lcd.print(inByte);
    current_step = inByte;

    // if(step_data[pattern_value][last_voice_selected][inByte] == 0)
    // if(step_leds[inByte].getState() != true)
    {
      // step_leds[lastInByte].toggle();
      step_leds[inByte].toggle();
      lastInByte = inByte;
      delay(50);

      // this handles how many patterns to run in a row.
      if(extended_step_length_mode == 1)
      {
        run_auto_pattern_select_routine(inByte);
      }

    }
  }
}

