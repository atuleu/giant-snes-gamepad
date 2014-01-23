#include <avr/io.h>
#include <avr/interrupt.h>

#include "Gamepad.h"

#include "Systime.h"
#include "USB.h"

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

// we have a mean over a number of measurement, the number of measurement is 2^NUM_READ_PWR
#define NUM_READ_PWR 2

const uint8_t ADCMapping[NUM_BUTTONS] = {
	0,
	1,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13 
};


typedef struct {
	uint16_t secondEllapsed;
	Systime_t loopTime;
	uint16_t  buttonStates;


	volatile uint16_t cellUpdated;
	//raw working Cell value
	volatile uint16_t cellValues[NUM_BUTTONS];
	//last available cell value, cleaned
	uint16_t lastCellValues[NUM_BUTTONS];
	uint8_t  cellCount[NUM_BUTTONS];
	Systime_t onDate[NUM_BUTTONS];
	volatile uint8_t error;
} GamepadData_t;

GamepadData_t GData;

void InitGamepad() {
	//set Digital 13 as output
	DDRD |= _BV(7);
	DDRE |= _BV(6);
	GData.error = 0;
	GData.buttonStates = 0;
	GData.secondEllapsed = 0;
	GData.loopTime       = GetSystime();


	//DIDR0 |= _BV(ADC7D) | _BV(ADC6D) | _BV(ADC5D) | _BV(ADC4D) | _BV(ADC1D) | _BV(ADC0D);
	//DIDR2 |= _BV(ADC13D) | _BV(ADC12D) | _BV(ADC11D) | _BV(ADC10D) | _BV(ADC9D) | _BV(ADC8D);
	
	// init the ADC
	// sets 128 prescaler , and Interrupt
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADIE);
	//sets reference as AVCC, with filtering Capacitor on AVREF
	ADMUX |= 1 << 6;
	//right align data
	ADMUX &= ~_BV(ADLAR);
	//sets the first conversion channel
	uint8_t pin = ADCMapping[0];
	if ( pin >> 3 ) {
		ADCSRB |= _BV(MUX5);
	} else {
		ADCSRB &= ~_BV(MUX5);
	}
	ADMUX = (ADMUX & 0xf8 ) | (pin & 0x07);

}

ISR(ADC_vect) {
	static uint8_t numRead       = 0;
	static uint8_t currentButton = 0;
	// we reset here, because we should only modify values that are
	// currentButton
	if(numRead == 0 ) {
		GData.cellValues[currentButton] = 0;
	}
	

	//we should read ADCL first, then ADCH, so lock is done right
	GData.cellValues[currentButton] += ADCL;
	GData.cellValues[currentButton] += (ADCH << 8) ;
	

	++numRead;
	if( numRead == (1 << NUM_READ_PWR) ) {
		++currentButton;
		if( currentButton == NUM_BUTTONS ) {
			currentButton = 0;
		}
		numRead = 0;
		uint8_t pin = ADCMapping[currentButton];
		if (pin >> 3 ) {
			ADCSRB |= _BV(MUX5);
		} else {
			ADCSRB &= ~_BV(MUX5);
		}
		ADMUX = (ADMUX & 0xf8) | (0x07 & pin);
	}  
	ADCSRA |= _BV(ADSC);
}


#define _BV16(i) ( ((uint16_t)1) << i)

void ProcessGamepad() {
	for ( uint8_t i = 0; i < NUM_BUTTONS; ++i ) {
		//disable interrupt
		uint8_t oldSREG = SREG;
		cli();

		uint16_t mask = GData.cellUpdated;
		uint16_t value = GData.cellValues[i];
	
		if ( mask & _BV16(i) ) {
			//restore interrupt state
			SREG = oldSREG;
			continue;
		}
	   
		GData.cellUpdated &= ~_BV16(i);
		value = value >> NUM_READ_PWR;
		GData.lastCellValues[i] = value;
		Systime_t now = GetSystime();
		if ( value > Parameters[CELL_MAX_PARAMS * i  + CELL_THRESHOLD] ) {
			if( (GData.buttonStates & _BV16(i) ) != 0 ){
				GData.cellCount[i] += 1;
			}
			GData.buttonStates |= _BV16(i);
			GData.lastCellValues[i] |= 0xa000;
			GData.onDate[i] = now;

		} else {
			if( (now - GData.onDate[i]) >= Parameters[CELL_MAX_PARAMS * i + CELL_RELEASE] ) {
				GData.buttonStates &= ~_BV16(i);
				GData.lastCellValues[i] &= 0x7fff;
			}
		}

		//restore interrupt state
		SREG = oldSREG;
	}

	if(GData.error) {
		PORTE |= _BV(6);
	} else {
		PORTE &= ~_BV(6);
	}

	//only process these every second
	if(GetSystime() - GData.loopTime < 1000) {
		return;
	}
	GData.secondEllapsed += 1;
	PORTC ^= _BV(7); //flash main led if disconnected from breadboard
	GData.loopTime += 1000;
}


uint16_t * GetCellValues() {
	return GData.lastCellValues;
}


void SetHIDReport(GamepadInReport_t * report) {
	//masks the 4 MSB 
	report->buttons = GData.buttonStates & 0x0fff;
}
