void run_LCD_setup_routine()
{

    pinMode(lcdTxPin, OUTPUT);

    // I bought a bunch of LCD backpacks for the LCD screens, and the controller chip runs not at 9600 baud, Nooooo
    // they run at 9850!

    lcd.begin(9850); // 9850 baud is chip comm speed
    // lcd.begin(9600); // 9600 baud is chip comm speed

    lcd.print("?c0");
    lcd.print("?c1"); // blinky

    lcd.print("?B70"); // set backlight to (70, but) ff hex, maximum brightness
    delay(500);        // pause to allow lcd EEPROM to program

    lcd.print("?s6"); // set tabs to six spaces
    delay(1000);      // pause to allow lcd EEPROM to program

    lcd.print("?D010181C1E1E1C1810"); // play
    delay(100);

    lcd.print("?D1110A04000E11110E"); // voice
    delay(100);

    lcd.print("?D21214181004040407"); // VL
    delay(100);

    lcd.print("?D31C10101C07040407"); // CC
    delay(100);

    lcd.print("?D4121A1612090D0B09"); // NN
    delay(100);

    lcd.print("?D504040E0404111B15"); // slider mode
    delay(100);

    lcd.print("?D61111110A04111B15"); // voice select button aka "VM", voice mode
    delay(100);

    lcd.print("?D7001F1F1F1F1F1F00"); // STOP ...
    //  this used to be "solo", but instead of explicitly calling it "solo", I'm going to go with "toggle" instead, and just use a "T" to  represent that
    //  Doing that will let me change the "O" I was using to represent "stop" and replace it with a solid square instead.

    delay(100);

    // see moderndevice.com for a handy custom char generator (software app)

    lcd.print("?f");      // clear the LCD
    lcd.print("?x00?y0"); // move cursor to beginning of line 1
    lcd.print("synthseqr v");
    lcd.print(hardware_version_number);

    lcd.print("?x00?y1"); // move cursor to beginning of line 0
    lcd.print("firmware v");
    lcd.print(firmware_version_number);
    delay(1500);
    // pinMode(66, INPUT); // ???
    // pinMode(67, INPUT); // ???

    lcd.print("?f"); // clear the LCD
}

void run_LCD_update()
{
    // lcd.print(lcdflag);
    // lcd.print(" ");
    // lcd.print(seq.getWhole());
    // maybe we only periodically clear the screen
    if (clear_the_lcd == true) // something changed
    {
        // stand down
        clear_the_lcd = false;
        lcd.print("?f"); // clear the lcd
    }

    switch (lcdflag)
    {
    case 90: // parked from run_voice_slider_routine for NN
    {
        // lcd.print("?f");      // clear the LCD
        lcd.print("?x02?y0"); // move cursor to beginning of line 0
        lcd.print("v");
        lcd.print(1);

        if (voice_slider_values[0] < 100)
        {
            lcd.print(" ");
        }

        lcd.print(" ?4");
        lcd.print(":");
        lcd.print(voice_slider_values[0]);
        if (voice_slider_values[0] < 100)
        {
            lcd.print(" ");
        }
        lcd.print("?x00?y1"); // move cursor to beginning of line 1
        last_lcdflag = 90;
        break;
    }
    case 91: // parked from run_voice_slider_routine for changing high range values
    {
        if (slider_map_low_value < 100)
        {
            lcd.print(" ");
        }
        lcd.print("?x09?y1"); // move cursor to line 2, char 10
        lcd.println("hi:");
        lcd.print("?x12?y1"); // move cursor to line 2, char 13
        lcd.print(slider_map_high_value);
        if (slider_map_high_value < 100)
        {
            //            lcd.print(" ");
        }
        last_lcdflag = 91;
        break;
    }
    case 92: // changing the voice slider high range
    {
        lcd.print("?x0?y1");  // move cursor to line 2
        lcd.print("?x00?y1"); // move cursor to line 2, char 1
        lcd.print("?5");
        lcd.print(" lo:");
        lcd.print("?x05?y1"); // move cursor to line 2, char 6
        lcd.print(slider_map_low_value);
        if (slider_map_low_value < 100)
        {
            lcd.print(" ");
        }
        lcd.print("?x09?y1"); // move cursor to line 2, char 10
        lcd.println("hi:");
        lcd.print("?x12?y1"); // move cursor to line 2, char 13
        lcd.print(slider_map_high_value);
        if (slider_map_high_value < 100)
        {
            lcd.print(" ");
        }
        break;
        last_lcdflag = 92;
    }
    case 93: // reset voice_slider_values
    {
        lcd.print("?f");      // clear the LCD
        lcd.print("?x00?y0"); // move cursor to beginning of line 1
        lcd.print("notenum reset");
        last_lcdflag = 93;
    }
    case 100: // pattern copy
    {
        // lcd.print("?f");      // clear the lcd
        lcd.print("?x00?y0"); // move cursor to beginning of line 1
        lcd.print("Copy ");
        lcd.print(current_pattern + 1);
        lcd.print("->");
        delay(100);
        last_lcdflag = 100;
        break;
    }
    case 101: // pattern copy to N
    {
        // lcd.print("?f"); // clear the lcd
        lcd.print("?x00?y0"); // move cursor to beginning of line 1
        lcd.print("Copy ");
        lcd.print(current_pattern + 1);
        lcd.print("->       ");
        last_lcdflag = 101;
        break;
    }
    case 200: // pattern chain single
    {
        // lcd.print("?f");      // clear the lcd
        lcd.print("?x00?y0"); // move cursor to beginning of line 1
        lcd.print("single");
        last_lcdflag = 200;
        break;
    }
    case 201: // pattern chain 4
    {
        // lcd.print("?f");      // clear the lcd
        lcd.print("?x00?y0"); // move cursor to beginning of line 1
        lcd.print("chain 4 ");
        last_lcdflag = 201;
        break;
    }
    case 255:
    default:
    {

        if (update_line1 == true)
        {

            // stand down the update line 1 flag
            update_line1 = false;

            // prep the string
            sprintf(lcd_line1, " C%02d %s%u T%.2f", MIDICHANNEL, voicemodechar, voice_mode, seq.getTempo());

            // Serial.println(lcd_line1);
            lcd.print("?x00?y0"); // move cursor to beginning of line 0

            // 01: play or stop
            if (playstatus == true)
            {
                lcd.print("?0");
            }
            else
            {
                lcd.print("?7");
            }

            lcd.print(lcd_line1);

            lcd.print("?x00?y1"); // move cursor to beginning of line 0

            // 15 swing
            lcd.print("s");
            // 16 swing value
            lcd.print(seq.getShuffle());

            lcd.print("?x00?y1"); // move cursor to beginning of line 0
        }
        else
        {
            // Serial.println("no update to line 1");
        }

        last_lcdflag = 255;
        break;
    }
    }
}

void set_lcd_cursor(int line, int position)
{
    sprintf(the_string, "?x%02d?y%d", position, line);
    /*
    the_string += "?x";
    the_string += position;
    the_string += "?y";
    the_string += line;
*/
    // Serial.println(the_string);
    lcd.print(the_string); // move cursor
    sprintf(the_string, "");
    // the_string = "";
    // Serial.println(the_string);
}

void clear_lcd()
{
    lcd.print("?f"); // clear the LCD
}
