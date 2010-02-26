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
  lcd.print("?c0");
  lcd.print("?Bff");    // set backlight to ff hex, maximum brightness     
  delay(500);                // pause to allow LCD EEPROM to program

  // lcd.print("?s6");     // set tabs to six spaces
  // delay(1000);               // pause to allow LCD EEPROM to program

  lcd.print("?D00000070404150E04");       // left down arrow
  delay(100);
  lcd.print("?D100001C0404150E04");       // right down arrow
  delay(100);                                  // delay to allow write to EEPROM



  lcd.print("?D7001F1F1F1F1F1F1F");  // 7
  delay(100); 

  lcd.print("?D600001F1F1F1F1F1F");  // 6
  delay(100); 

  lcd.print("?D50000001F1F1F1F1F");  // 5
  delay(100); 

  lcd.print("?D4000000001F1F1F1F");  // 4
  delay(100); 

  lcd.print("?D300000000001F1F1F");  // 3
  delay(100); 

  lcd.print("?D20000000000001F1F");  // 2
  delay(100); 


  // see moderndevice.com for a handy custom char generator (software app)


  lcd.print("?f");                   // clear the LCD
  lcd.print("?x00?y1");// move cursor to beginning of line 1
  lcd.print("beatseqr v3."+build_number);
  lcd.print("?x00?y0");// move cursor to beginning of line 0
  lcd.print("http://3rl.us");
  delay(1000);
  //   pinMode(66, INPUT);  
  //   pinMode(67, INPUT);


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
