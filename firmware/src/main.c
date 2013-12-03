#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <avr/power.h>
#include <avr/wdt.h>


#include "Systime.h"

void LedDisplayTime(uint8_t sec) {
	uint8_t saveSREG = SREG;
	cli();

	if( sec & ( 1 << 0 ) ) {
		PORTC |= _BV(6);
	} else {
		PORTC &= ~_BV(6);
	}
	
	if( sec & ( 1 << 1 ) ) {
		PORTE |= _BV(6);
	} else {
		PORTE &= ~_BV(6);
	}
	
	if( sec & ( 1 << 2 ) ) {
			PORTB |= _BV(5);
	} else {
		PORTB &= ~_BV(5);
	}
	
	if( sec & ( 1 << 3 ) ) {
		PORTB |= _BV(7);
	} else {
		PORTB &= ~_BV(7);
	}

	SREG = saveSREG;
}


void SetupHardware() {
	//Arduino's micro bootloader let the USB interrupt on, but if we
	//are not using USB, well it blows our face because ISR routine
	//are not there. We disable the interrupt. Maybe a USB clock
	//freeze will be great too. This is teh first step because we do
	//not want our USB host to wake and crash us.
	UDIEN &= ~(_BV(SUSPE) | _BV(EORSTE) );

	//this is not stricly needed as set by Arduino Micro's
	//bootloader. But just to be absolutely certain :

	MCUSR &= ~_BV(WDRF); // clear watchdog flag
	wdt_disable(); //disbale it
	
	//no prescale of F_CPU
	clock_prescale_set(clock_div_1);

	//rest of the init here

	DDRC  |= _BV(6);
	DDRE  |= _BV(6);
	DDRB  |= _BV(5);
	DDRB  |= _BV(7);
	DDRC  |= _BV(7);

	InitSystime();

}

#define LOOP_IN_MS 1000

int main (void) { 
	SetupHardware();
	uint8_t secondEllapsed = 0;
	Systime_t prevTime = GetSystime();

	while(1) {
		Systime_t curTime = GetSystime();
		if(curTime - prevTime < LOOP_IN_MS ) {
			continue;
		}
		prevTime += LOOP_IN_MS;
		secondEllapsed += 1;
		LedDisplayTime(secondEllapsed);
	}
}
 

//ISR(USB_COM_vect){}
//ISR(USB_GEN_vect){}
