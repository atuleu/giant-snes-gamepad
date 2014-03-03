#include <avr/io.h>
#include <avr/interrupt.h>

#include "Gamepad.h"

#include "Systime.h"
#include "USB.h"


//Leds are in order in PB3 PB1 PD2 PD3

// we have a mean over a number of measurement, the number of measurement is 2^NUM_READ_PWR
#define NUM_READ_PWR 2

const uint8_t ADCMapping[NUM_BUTTONS] = {
	8,
	10,
	11,
	12,
	13,
	9,
	0,
	7,
	1,
	6,
	4,
	5 
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

	//Init LEDs as output
	
	DDRB |= _BV(1) | _BV(3);
	DDRC |= _BV(7);
	DDRD |= _BV(2) | _BV(3);



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


void PrintLSB(uint8_t value) {
	//Leds are in order in PB3 PB1 PD2 PD3
	if ( value & (1 << 0) ) {
		PORTB |= _BV(3);
	} else {
		PORTB &= ~_BV(3);
	}

	if ( value & (1 << 1) ) {
		PORTB |= _BV(1);
	} else {
		PORTB &= ~_BV(1);
	}

	if ( value & (1 << 2) ) {
		PORTD |= _BV(2);
	} else {
		PORTD &= ~_BV(2);
	}

	if ( value & (1 << 3) ) {
		PORTD |= _BV(3);
	} else {
		PORTD &= ~_BV(3);
	}

}

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


	//only process these every second
	if(GetSystime() - GData.loopTime < 250) {
		return;
	}
	GData.secondEllapsed += 1;
	PrintLSB(GData.secondEllapsed);
	GData.loopTime += 250;
}


uint16_t * GetCellValues() {
	return GData.lastCellValues;
}


void SetHIDReport(GamepadInReport_t * report) {
	//masks the 4 MSB 
	report->buttons = GData.buttonStates & 0x0fff;
}
