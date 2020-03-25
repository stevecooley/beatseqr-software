void serial_printer(const char* message) {
  // version 4.2 was a continuous serial data stream.
  //   Serial.print(message);

  // version 4.3 and above is experimentally going back to this format for
  // easier parsing in max/msp... trying to kill roxor, and bring all of the
  // hardware messaging into steppa.
  Serial.println(message);

  // last_serial = message;

  return;
}

void process_incoming_serial() {
  /*
  if (Serial.available() > 0)
  {
    // get incoming byte:
    inByte = Serial.read();
    lcd.print("?x00?y0"); // move cursor to beginning of line 0
Serial.println("?x00?y0"); // move cursor to beginning of line 0
    lcd.print(inByte);
Serial.println(inByte);
    current_step = inByte;

    // if(step_data[pattern_value][last_voice_selected][inByte] == 0)
    // if(step_leds[inByte].getState() != true)
    // {
    // step_leds[lastInByte].toggle();
    step_leds[inByte].toggle();
    lastInByte = inByte;
    delay(50);

    // this handles how many patterns to run in a row.
    if (extended_step_length_mode == 1)
    {
      run_auto_pattern_select_routine(inByte);
    }
  }
  */
}
