void listen_for_navigation_events()
{
    switch (navmode)
    {
    case 100: // default to tempo and swing adjustments
    {
        if (dpad_up.uniquePress())
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
        if (dpad_left.uniquePress())
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
                SWING++;
                seq.increaseShuffle();
                update_line1 = true;
                Serial.println(seq.getShuffle());
            }
        }
        break;
        if(enterbutton.uniquePress())
        {
            enterbutton_LED.toggle();
            update_line1 = true;
            lcdflag = 255;
            last_lcdflag = 255;
        }
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