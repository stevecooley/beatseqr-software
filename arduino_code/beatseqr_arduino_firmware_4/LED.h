/*
||
|| @file LED.h
|| @version 1.1
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Provide an easy way of making/using LEDs
|| #
||
|| @license
|| | Copyright (c) 2009 Alexander Brevig. All rights reserved.
|| | This code is subject to AlphaLicence.txt
|| | alphabeta.alexanderbrevig.com/AlphaLicense.txt
|| #
||
*/

#ifndef LED_H
#define LED_H

#include "Arduino.h" 

class LED{
  public:
    LED(uint8_t ledPin);
	bool getState();
    void on();
	void off();
	void toggle();
	void blink(unsigned int time, byte times=1);
	void setValue(byte val);
	void fadeIn(unsigned int time);
	void fadeOut(unsigned int time);
  private:
	bool status;
	uint8_t pin;
};

extern LED DEBUG_LED;

#endif

/*
|| @changelog
|| | 1.1 2009-05-07 - Alexander Brevig : Added blink(uint,byte), requested by: Josiah Ritchie - josiah@josiahritchie.com
|| | 1.1 2009-04-07 - Alexander Brevig : Altered API
|| | 1.0 2009-04-17 - Alexander Brevig : Initial Release
|| #
*/
