#include <avr/io.h>
#include <avr/interrupt.h>

#include "Gamepad.h"

#include "Systime.h"


/*
 * List of IO
 * led 1 : D7
 * led 2 : B4
 * led 3 : B6
 * led 4 : C6
 * led 5 : E6
 * led 6 : B5
 * led 7 : B7
 * alive led : C7
 * button 1 : D4
 * button 2 : D1
 */

void LedDisplayLSB(uint8_t value) {

	if( value & ( 1 << 0 ) ) {
		PORTB |= _BV(6);
	} else {
		PORTB &= ~_BV(6);
	}
	
	if( value & ( 1 << 1 ) ) {
		PORTC |= _BV(6);
	} else {
		PORTC &= ~_BV(6);
	}
	
	if( value & ( 1 << 2 ) ) {
		PORTE |= _BV(6);
	} else {
		PORTE &= ~_BV(6);
	}
	
	if( value & ( 1 << 3 ) ) {
		PORTB |= _BV(5);
	} else {
		PORTB &= ~_BV(5);
	}

	if( value & ( 1 << 4 ) ) {
		PORTB |= _BV(7);
	} else {
		PORTB &= ~_BV(7);
	}

	
}


typedef struct {
	uint8_t   secondEllapsed;
	Systime_t loopTime;
	uint8_t   error;
	uint8_t   value;
	uint16_t  buttonStates;
} GamepadData_t;

GamepadData_t GData;

void InitGamepad() {
	DDRB  |= _BV(4) | _BV(5) | _BV(6) | _BV(7);
	DDRC  |= _BV(6) | _BV(7);
	DDRD  |= _BV(7);
	DDRE  |= _BV(6);

	//sets pin as input with pull up
	PORTD |= _BV(1) | _BV(4);
	//should not be done, but who knows with this nasty bootloader.
	DDRD &= ~(_BV(1) | _BV(4));

	GData.error = 0;
	GData.value = 0;
	GData.buttonStates = 0;
	GData.secondEllapsed = 0;
	GData.loopTime       = GetSystime();
}


void ReportError(uint8_t error) {
	uint8_t savedSREG = SREG;
	cli();
	GData.error |= _BV(error);
	SREG = savedSREG;
}


void DisplayValue(uint8_t value) {
	uint8_t savedSREG = SREG;
	cli();
	GData.error = 0xff;
	GData.value = value;
	SREG = savedSREG;
}

void ProcessGamepad() {
	if(GData.error == 0xff ) {
		//report we display value
		PORTD |= _BV(7);
		PORTB |= _BV(4);

		if(GData.secondEllapsed & 0x01) {
			//display MSB
			LedDisplayLSB( 0x10 | (GData.value >> 4) );
		} else {
			//display LSB
			LedDisplayLSB(GData.value & 0x0f);
		}

	} else if (GData.error) {
		PORTD |= _BV(7);
		PORTB &= ~_BV(4);
		LedDisplayLSB(GData.error);
	} else  {
		PORTD &= ~_BV(7);
		PORTB |= _BV(4);
		LedDisplayLSB(0);
	}

	if(PIND & _BV(4)) {
		GData.buttonStates &= ~_BV(0);
		PORTB &= ~_BV(6);
	} else {
		GData.buttonStates |= _BV(0);
		PORTB |= _BV(6);
	}

	if( PIND & _BV(1) ) {
		GData.buttonStates &= ~_BV(1);
		PORTC &= ~_BV(6);
	} else {
		GData.buttonStates |= _BV(1);
		PORTC |= _BV(6);
	}

	//only process these every second
	if(GetSystime() - GData.loopTime < 1000) {
		return;
	}
	GData.secondEllapsed += 1;
	PORTC ^= _BV(7); //flash main led if disconnected from breadboard
	GData.loopTime += 1000;
}



void SetHIDReport(GamepadInReport_t * report) {
	//masks the 4 MSB 
	report->buttons = GData.buttonStates & 0x0fff;
}
