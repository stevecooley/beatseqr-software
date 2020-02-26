void run_step_button_routine()
{
  // step buttons
  detect_step_button_presses();

  if (step_buttons[0].isPressed() && step_buttons[15].isPressed()) // clear the pattern for this voice
  {
    clear_pattern_memory_for_voice(0); //synthseqr configuration
  }

  if (step_buttons[0].isPressed() && step_buttons[11].isPressed()) // clear the entire pattern
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
      step_leds[i].toggle();

      step_data[pattern_value][0][i] = step_leds[i].getState();
      // nn = String(voice_slider_midinotenum[i], HEX);
      if (step_data[pattern_value][0][i] == 1)
      {
        // ex: void FifteenStep::setNote(byte channel, byte pitch, byte velocity, byte step)
        seq.setNote(MIDICHANNEL, voice_slider_midinotenum[i], 0xFF, i);
      }
      else
      {
        seq.setNote(MIDICHANNEL, voice_slider_midinotenum[i], 0x00, i);
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
  for (int v = 0; v < 1; v++) // synthseqr configuration
    {
      for (int i = 0; i <= 15; i++)
      {
        step_data[pattern_value][v][i] = 0;
        step_leds[i].off();

      }
    }
    return;
}