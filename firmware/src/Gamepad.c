#include <avr/io.h>
#include <avr/interrupt.h>

#include "Gamepad.h"

#include "Systime.h"

void LedDisplayLSB(uint8_t value) {

	if( value & ( 1 << 0 ) ) {
		PORTC |= _BV(6);
	} else {
		PORTC &= ~_BV(6);
	}
	
	if( value & ( 1 << 1 ) ) {
		PORTE |= _BV(6);
	} else {
		PORTE &= ~_BV(6);
	}
	
	if( value & ( 1 << 2 ) ) {
		PORTB |= _BV(5);
	} else {
		PORTB &= ~_BV(5);
	}
	
	if( value & ( 1 << 3 ) ) {
		PORTB |= _BV(7);
	} else {
		PORTB &= ~_BV(7);
	}
	
}

typedef struct {
	uint8_t   secondEllapsed;
	Systime_t loopTime;
} GamepadData_t;

GamepadData_t GData;

void InitGamepad() {
	DDRC  |= _BV(6);
	DDRE  |= _BV(6);
	DDRB  |= _BV(5);
	DDRB  |= _BV(7);
	DDRC  |= _BV(7);	

	GData.secondEllapsed = 0;
	GData.loopTime       = GetSystime();
}




void ProcessGamepad() {
	if(GetSystime() - GData.loopTime < 1000) {
		return;
	}
	PORTC ^= _BV(7); //flash main led if disconnected from breadboard
	GData.loopTime += 1000;
	GData.secondEllapsed += 1;
	LedDisplayLSB(GData.secondEllapsed);
}
