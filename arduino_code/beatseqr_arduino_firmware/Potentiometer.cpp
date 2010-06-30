/*
||
|| @author Alexander Brevig
|| @version 1.1
||
*/

#include "Potentiometer.h"

Potentiometer::Potentiometer(uint8_t potPin){
	pin=potPin;
	setSectors(6);
}

Potentiometer::Potentiometer(uint8_t potPin, uint16_t sectors){
	pin=potPin;
	setSectors(sectors);
}

uint16_t Potentiometer::getValue(){
	return analogRead(pin);
}

uint16_t Potentiometer::getSector(){
	return analogRead(pin)/(1024/sectors);
}

void Potentiometer::setSectors(uint16_t newSectors){
	if (newSectors<1024 && newSectors>0){
		sectors=newSectors;
	}else if (newSectors<0){
		sectors=0;
	}else{
		sectors=1023;
	}
}
