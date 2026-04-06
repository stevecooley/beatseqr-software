
void run_diagnostics()
{
  if (dpad_left.heldFor(1000) && dpad_right.heldFor(1000))
  {
    Serial.println("running diagnostics...");

    // Wait for both buttons to be released before entering — otherwise the
    // exit check (same combo) inside captive_diagnostic_routine fires immediately.
    while (dpad_left.isPressed() || dpad_right.isPressed()) {}

    captive_diagnostic_routine();
  }
}

void captive_diagnostic_routine()
{
  // D-pad up: print hold duration while held
  if (dpad_up.uniquePress())
  {
    long now = millis();
    while (dpad_up.isPressed())
    {
      Serial.print("dpad up held for ");
      Serial.println(millis() - now);
    }
  }

  if (dpad_down.uniquePress())
  {
    Serial.println("dpad down pressed");
  }

  if (dpad_left.uniquePress())
  {
    Serial.println("dpad left pressed");
  }

  if (dpad_right.uniquePress())
  {
    Serial.println("dpad right pressed");
  }

  if (enterbutton.uniquePress())
  {
    Serial.println("enter pressed");
  }

  // Step buttons
  for (int i = 0; i <= 15; i++)
  {
    if (step_buttons[i].uniquePress())
    {
      step_leds[i].toggle();
      Serial.print("step ");
      Serial.print(i);
      Serial.println(" pressed");
    }
  }

  // Pattern select buttons
  for (int i = 0; i <= 3; i++)
  {
    if (pattern_select_buttons[i].uniquePress())
    {
      pattern_select_leds[i].toggle();
      Serial.print("pattern ");
      Serial.print(i);
      Serial.println(" pressed");
    }
  }

  // Voice sliders: only print when value changes
  static int last_diag_slider[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  for (int j = 0; j <= 15; j++)
  {
    int val = map(voice_sliders[j].getSector(), 0, 255, slider_map_low_value, slider_map_high_value);
    if (val != last_diag_slider[j])
    {
      last_diag_slider[j] = val;
      Serial.print("slider ");
      Serial.print(j);
      Serial.print(" ");
      Serial.println(val);
    }
  }

  // Exit: hold left + right simultaneously (same combo as entry)
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
