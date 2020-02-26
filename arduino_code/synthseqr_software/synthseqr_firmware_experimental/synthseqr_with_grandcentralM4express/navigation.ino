void listen_for_navigation_events()
{

    switch (navmode)
    {
    case 100: // default to tempo and swing adjustments
    {

        /*
        if (dpad_left.uniquePress())
        {
            //TEMPO++;
            seq.increaseTempo();
            update_line1 = true;
        }
        if (dpad_down.uniquePress())
        {
            //TEMPO--;
            seq.decreaseTempo();
            update_line1 = true;
        }
        
        */
        switch (timing_mode)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            if (SWING >= 1)
            {
                SWING--;
                seq.decreaseShuffle();
                update_line1 = true;
                Serial.println(seq.getShuffle());
            }
        }
            if (dpad_right.uniquePress())
            {
                if (SWING <= 6)
                {
                    if (SWING <= 6)
                    {
                        SWING++;
                        seq.increaseShuffle();
                        update_line1 = true;
                        // Serial.println(seq.getShuffle());
                    }
                }

                if (dpad_down.uniquePress())
                {
                    if (SWING >= 1)
                    {
                        SWING--;
                        seq.decreaseShuffle();
                        update_line1 = true;
                        // Serial.println(seq.getShuffle());
                    }
                }
            }

            if (enterbutton.uniquePress())
            {
                clear_the_lcd = true;
                enterbutton_LED.toggle();
                update_line1 = true;
                lcdflag = 255;
                last_lcdflag = 255;
            }
            break;
        }
    case 110: // adjust the slider values low and high
    {
        if (dpad_left.uniquePress())
        {

            slider_map_low_value = map(voice_sliders[0].getSector(), 0, 255, 0, 128);

            // begin display the high and low values for the sliders
            // Serial.println(slider_map_low_value);

            // parking LCD updates in lcdflag 91!

            // end display the high and low values for the sliders

            midinn_sliderrangelow = slider_map_low_value;

            for (uint8_t i = 0; i < 15; i++)
            {
                if (voice_slider_midinotenum[i] <= midinn_sliderrangelow)
                {
                    voice_slider_midinotenum[i] = midinn_sliderrangelow;
                }
            }
        }
        // high range is set with right d-pad and enter
        if (dpad_right.uniquePress())
        {
            slider_map_high_value = map(voice_sliders[0].getValue(), 0, 255, 0, 128);

            // begin display the high and low values for the sliders

            // parked in lcdflag = 92!

            // end display the high and low values for the sliders

            midinn_sliderrangehigh = slider_map_high_value;

            for (int i = 0; i < 15; i++)
            {
                if (voice_slider_midinotenum[i] != midinn_sliderrangehigh)
                {
                    voice_slider_midinotenum[i] = midinn_sliderrangehigh;
                }
            }
        }
    }
    }
    }

    void set_timing_resolution()
    {
        switch (timing_mode)
        {
        case 1: //
        {
            timing_resolution = 10.0;
            set_lcd_cursor(0, 11);
            break;
        }
        case 2: //
        {
            timing_resolution = 1.0;
            set_lcd_cursor(0, 12);
            break;
        }
        case 3: //
        {
            timing_resolution = 0.1;
            set_lcd_cursor(0, 14);
            break;
        }
        case 4: //
        {
            timing_resolution = 0.01;
            set_lcd_cursor(0, 15);
            break;
        }
        }
    }

    /* park swing code for now
 
*/