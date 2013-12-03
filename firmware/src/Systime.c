#include <avr/interrupt.h>
#include <avr/io.h>

#include "Systime.h"


volatile uint16_t s_Systime;

ISR(TIMER0_COMPA_vect) {
	s_Systime += 1;
}

void InitSystime() {
	//enables interrupt
	sei();


	
	TIMSK0 |= _BV(OCIE0A); // enable interrupt Output compare enable

	TCCR0A |= _BV(WGM01); // CTC mode, reset TCNT0 on OCR0A value

	OCR0A = 249; /// TODO right value according to F_CPU
	
	TCNT0 = 0;
	
	//set systme to 0
	s_Systime = 0;

	//start timer with a F_CPU / 64 clock
	TCCR0B |= ( _BV(CS01) | _BV(CS00) );


}



Systime_t GetSystime() {
	Systime_t curSystime;
	//disbale interrupt to avoid race condition
	uint8_t savedSREG = SREG;
	cli();

	curSystime = s_Systime;
	
	//restores global interrupt state
	SREG = savedSREG;

	return curSystime;
}
	
