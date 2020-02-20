void run_pattern_select_routine()
{

  listen_for_copy_command();

  // pattern select
  for (int pattern = 0; pattern < 4; pattern++)
  {

    if (pattern_select_buttons[pattern].uniquePress())
    {
      // go_to_pattern(pattern to go to, silent?)
      go_to_pattern(pattern, 0);
      read_step_memory(0, pattern);
    }

    // pattern copy
    if (pattern_select_buttons[pattern].heldFor(2000))
    {
      // Serial.print(last_serial);
      not_told_which_pattern_to_copy_to = false; // this is us being told to copy the pattern
      lcdflag = 100; // pattern copy

      // pattern_select_button_pressing_counter = 0;
    }
  }

  if (pattern_select_buttons[0].isPressed() && pattern_select_buttons[3].isPressed())
  {

    if (extended_step_length_mode == 1)
    {
      lcdflag = 200; // pattern chain single
      extended_step_length_mode = 0;
      pattern_select_leds[0].on();
      pattern_select_leds[1].on();
      pattern_select_leds[2].on();
      pattern_select_leds[3].on();
      delay(10);
      pattern_select_leds[3].off();
      pattern_select_leds[2].off();
      pattern_select_leds[1].off();
      pattern_select_leds[0].off();
      go_to_pattern(0, 1);
    }
    else
    {
      lcdflag = 201; //pattern chain 4
      extended_step_length_mode = 1;
      pattern_select_leds[0].off();
      pattern_select_leds[1].off();
      pattern_select_leds[2].off();
      pattern_select_leds[3].off();
      delay(10);
      pattern_select_leds[0].on();
      pattern_select_leds[1].on();
      pattern_select_leds[2].on();
      pattern_select_leds[3].on();
      go_to_pattern(0, 1);
    }
  }
}

void run_auto_pattern_select_routine()
{

  if (seq.getPosition() >= 15) // seems wrong
  {
    current_pattern++;
    if (current_pattern >= 3)
    {
      current_pattern = 0;
    }

    // go_to_pattern(pattern to go to, silent?)
    go_to_pattern(current_pattern, 1);
  }
}

void go_to_pattern(int pattern, int silent)
{
  pattern_select_leds[0].off();
  pattern_select_leds[1].off();
  pattern_select_leds[2].off();
  pattern_select_leds[3].off();
  pattern_select_leds[pattern].toggle();
  pattern_value = pattern;
  // pattern_select_button_pressing_counter = 0;

  // "P"age / pattern "S"elect
  /*
  the_serial_message = "ZPS,";
  the_serial_message += pattern_value;
  the_serial_message += ";";
  serial_printer(the_serial_message);
*/
  current_pattern = pattern_value;

  for (int voice = 0; voice < 1; voice++) //synthseqr configuration
  {
    for (int step = 0; step <= 15; step++)
    {
      step_value = step_data[pattern][voice][step];

      if (step_value == 1)
      {
        step_leds[step].on();
      }
      else
      {
        step_leds[step].off();
      }
    }
  }
}

void listen_for_copy_command()
{
  // return; // let's just not, right now.

  int ended_on;

  if (not_told_which_pattern_to_copy_to == false) // we were told to copy the current pattern to another pattern
  {
    lcdflag = 101; // pattern copy to N
    for (int i = 0; i <= 3; i++)
    {
      if (pattern_select_buttons[i].uniquePress())
      {
        copy_pattern_to = i; // for lcdflag 101

        pattern_select_leds[0].off();
        pattern_select_leds[1].off();
        pattern_select_leds[2].off();
        pattern_select_leds[3].off();

        pattern_select_leds[i].on();

        for (int voice = 0; voice < 1; voice++) //synthseqr configuration
        {
          for (int step = 0; step <= 15; step++)
          {
            step_data[copy_pattern_to][voice][step] = step_data[current_pattern][voice][step];
          }
        }
        not_told_which_pattern_to_copy_to = true;
        ended_on = copy_pattern_to;
      }
    }
  }
}

void listen_for_extended_step_length_command()
{

  int ended_on;

  if (not_told_which_pattern_mode_to_use == false) // we were told to use a different pattern mode
  {
    for (int pattern = 0; pattern <= 3; pattern++)
    {
      if (pattern_select_buttons[pattern].uniquePress())
      {

        pattern_select_leds[0].off();
        pattern_select_leds[1].off();
        pattern_select_leds[2].off();
        pattern_select_leds[3].off();
        // pattern_select_leds[pattern].blink(250,3);
        pattern_select_leds[pattern].on();
        // pattern_select_button_pressing_counter = 0;

        not_told_which_pattern_mode_to_use = true;
        extended_step_length_mode = 1;
        patterns_to_play_in_a_row = pattern;
      }
    }
    // "P"age / pattern "S"elect
  }
}
