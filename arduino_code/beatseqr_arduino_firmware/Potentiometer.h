/*
||
|| @author Alexander Brevig
|| @version 1.1
||
*/

#ifndef Potentiometer_H
#define Potentiometer_H
 
#include "WProgram.h"

class Potentiometer {
	public:
		Potentiometer(uint8_t potPin);
		Potentiometer(uint8_t potPin, uint16_t sectors);
		uint16_t getValue();
		uint16_t getSector();
		void setSectors(uint16_t sectors);
	private:
		uint8_t pin;
		uint16_t sectors;
};

#endif
