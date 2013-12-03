#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <avr/power.h>
#include <avr/wdt.h>

#define val (65535 - 25000)

volatile uint16_t systimeInMs;
volatile uint16_t foo;

ISR(TIMER0_COMPA_vect) {
	++systimeInMs;
}

uint16_t GetMillis() {
	uint16_t curTime;
	uint8_t saveSREG = SREG;
	cli();
	curTime = systimeInMs;
	SREG = saveSREG;
	return curTime;
}


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

int main (void) { 
	MCUSR &= ~_BV(WDRF);
	wdt_disable();
	
	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	//disbale all usb realted interrupt

	


	TIMSK1 = 0;
	TIMSK0 |= _BV(OCIE0A); // Enable overflow interrupt 


	TCNT0 = 0 ; 

	systimeInMs = 0;

	OCR0A = 249;

	TCCR0A |= _BV(WGM01);

	TCCR0B |= ((1 << CS01) | (1 << CS00)); // Set up timer at Fcpu/64 

	//stuff set by Micro's bootloader crashing everything !!!
	UDIEN &= ~(_BV(SUSPE) | _BV(EORSTE) );


	sei();

	DDRC  |= _BV(6);
	DDRE  |= _BV(6);
	DDRB  |= _BV(5);
	DDRB  |= _BV(7);
	DDRC  |= _BV(7);

	


	uint8_t secondEllapsed = 0;
	uint16_t prevTime = GetMillis();

	#define SEC_VALUE 1000

	while(1) {
		uint16_t curTime = GetMillis();
		if(curTime - prevTime < SEC_VALUE ) {
			continue;
		}
		prevTime += SEC_VALUE;
		//_delay_ms(1000);
		secondEllapsed += 1;
		LedDisplayTime(secondEllapsed);
	}
}
 

//ISR(USB_COM_vect){}
//ISR(USB_GEN_vect){}
