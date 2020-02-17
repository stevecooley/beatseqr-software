int voiceval = 0;
int lowest_reading = 1000;
int highest_reading = 0;
int average_reading = 0;
int diagnostic_total = 0;
int diagnostic_readings[10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int save_high = 0;
int save_low = 0;

void run_diagnostics()
{

  if (playbutton.heldFor(2000) && enterbutton.heldFor(2000))
  {

    Serial.println("running diagnostics...");

    captive_diagnostic_routine();
  }
}

void captive_diagnostic_routine()
{

  if (dpad_up.uniquePress())
  {
    long now = millis();
    while (dpad_up.isPressed())
    {
      Serial.print("dpad up held for ");
      Serial.println(millis() - now);

      if (dpad_up.held())
      {
        Serial.println("up held");
      }
      if (dpad_up.heldFor(2000))
      {
        Serial.println("up held for 2000+");
      }
    }
  }

  if (dpad_right.uniquePress())
  {
    Serial.println("dpad right pressed");
  }

  if (dpad_left.heldFor(2000))
  {
    for (int i = 100; i <= 115; i++)
    {
      // int value = EEPROM.read(i);
      Serial.print("eeprom ");
      Serial.print(i);
      Serial.print(" ");
      // Serial.println(value);
      delay(200);
    }
  }

  // step buttons
  for (int i = 0; i <= 15; i++)
  {
    if (step_buttons[i].uniquePress())
    {
      step_leds[i].toggle();
      Serial.print(i);
      Serial.println(" received");
    }
  }

  // pattern select buttons
  for (int i = 0; i <= 3; i++)
  {
    if (pattern_select_buttons[i].uniquePress())
    {
      pattern_select_leds[i].toggle();
      Serial.print(i);
      Serial.println(" received");
    }
  }

  // get the raw value
  for (int j = 0; j <= 15; j++)
  {
    int sensorValue = map(voice_sliders[j].getSector(), 0, 255, slider_map_low_value, slider_map_high_value);

    /*
    if(sensorValue > 10) {
      Serial.print(j);
      Serial.print(" "); 
      Serial.println(sensorValue);
    }
    */
  }

  if (dpad_right.isPressed() && dpad_left.isPressed())
  {
    Serial.println("exiting diagnostics");
    return;
  }
  else
  {
    captive_diagnostic_routine();
  }
}
