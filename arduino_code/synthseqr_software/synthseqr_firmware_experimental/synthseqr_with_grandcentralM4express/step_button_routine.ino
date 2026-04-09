void run_step_button_routine()
{
  // step buttons
  detect_step_button_presses();

  // Use wasPressed() here (not isPressed()) — uniquePress() already called
  // isPressed() for all step buttons in detect_step_button_presses() above.
  // Calling isPressed() a second time on the same button in the same loop
  // iteration can steal the CHANGED flag and cause the next uniquePress() to
  // miss a press. wasPressed() reads the cached state with no side effects.
  if (step_buttons[0].wasPressed() && step_buttons[15].wasPressed()) // clear the pattern for this voice
  {
    clear_pattern_memory_for_voice(0); //synthseqr configuration
  }

  if (step_buttons[0].wasPressed() && step_buttons[11].wasPressed()) // clear the entire pattern
  {
    clear_pattern_memory();
  }
  
}


void detect_step_button_presses()
{
  for (int i = 0; i <= 15; i++)
  {
    if (step_buttons[i].uniquePress())
    {
      // Toggle step_data directly — do not rely on LED state, which the
      // chase light may have inverted for the currently-playing step.
      step_data[pattern_value][0][i] = step_data[pattern_value][0][i] ? 0 : 1;

      if (step_data[pattern_value][0][i]) {
        step_leds[i].on();
      } else {
        step_leds[i].off();
      }
      // nn = String(voice_slider_midinotenum[i], HEX);
      if (step_data[pattern_value][0][i] == 1)
      {
        // ex: void FifteenStep::setNote(byte channel, byte pitch, byte velocity, byte step)
        seq.setNote(MIDICHANNEL-1, voice_slider_midinotenum[i], 127, i);
      }
      else
      {
        seq.setNote(MIDICHANNEL-1, voice_slider_midinotenum[i], 0, i);
      }
    }
  }
  return;
}

void read_step_memory(int voice, int pattern)
{
  for (int i = 0; i <= 15; i++)
  {
    int this_step = step_data[pattern][voice][i];

    if (this_step == 1)
    {
      step_leds[i].on();
    }
    else
    {
      step_leds[i].off();
    }
  }
}

void clear_step_leds()
{
  for (int i = 0; i <= 15; i++)
  {
    step_leds[i].off();
  }
}

void copy_step_data(int pattern_value, int voice, int step, int step_value)
{

  step_data[pattern_value][last_voice_selected][step] = step_value;
  
}

void clear_pattern_memory_for_voice(int voice)
{
  for (int i = 0; i <= 15; i++)
    {

      step_data[pattern_value][voice][i] = 0;
      step_leds[i].off();

    }
  return;
}

void clear_pattern_memory()
{
  for (int p = 0; p < 4; p++)
  {
    for (int v = 0; v < 1; v++) // synthseqr configuration
    {
      for (int i = 0; i <= 15; i++)
      {
        step_data[p][v][i] = 0;
      }
    }
  }
  // Refresh LEDs to reflect the cleared active pattern
  read_step_memory(0, pattern_value);
}