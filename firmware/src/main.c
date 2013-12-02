/*
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/power.h> 
#include <avr/io.h>
#include <util/delay.h>



//#include "USB.h"
//#include "GSG.h"


volatile uint8_t systimeInMs;

ISR(TIMER0_OVF_vect) {
	PORTC ^= _BV(7);
	//TCCR0B |= _BV(CS01) ;
	//systimeInMs += 1;
}


/*uint16_t GetMillis() {
	//we are on a 8 bit platform. Accessing a 16 byte value is never
	//atomic !!! We should disable interrupt.
	
	uint16_t currentSystime;
	
	//disable Overflow interrupt
	TIMSK1 &= ~_BV(OCIE1A);
	
	currentSystime = systimeInMs;

	//enable interrupt
	TIMSK1 |= _BV(OCIE1A);

	return currentSystime;
	}

void SetupHardware() {
	sei();
	//disbale watchdog
	//	MCUSR &= ~(1 << WDRF);
	//wdt_disable();

	//no prescaling
	//clock_prescale_set(clock_div_1);
   
	DDRC = _BV(7); //Sets PortC Pin 7 as Digital Output.

	//TCCR1A = 0x0; // no output compare std waveform
	//TCCR0A |= _BV(WGM01); //CTC mode compare on OCR0A

	//OCR0A = 249; // 16e6 /( 64 * (249 + 1) = 1e3 

	//	systimeInMs = 0;

	//this line with start the timer
	TCCR0B |= _BV(CS01) | _BV(CS00) ;

	TIMSK0 |= _BV(TOIE0); // Enables Timer Overflow Interrupt 1
   
   
	PORTC |= _BV(7);
}

int main() {
	SetupHardware();
	//Initialize all our application
	//	Init();
	//starts the USB stack
	//USB_Init();
	

	//uint16_t prevTime = GetMillis();


	while(1) {
		//		_delay_ms(500);
		// PORTC ^= _BV(7);
		//process the usb
		//RunUSBTask();
		
		//process the app
		//Process();

		/*		int16_t curTime = GetMillis();
		if(curTime - prevTime > 500 ) { 
			prevTime += 500; //overflow will be keeped :-)

			PORTC ^= _BV(7); //togle PORTC 7
			}
	}

	return 0;
}

*/


#include <avr/io.h> 
#include <avr/interrupt.h>

#define val (65535 - 25000)

int main (void) { 
	DDRC |= (1 << 7); // Set LED as output 

	TIMSK1 |= (1 << TOIE1); // Enable overflow interrupt 
	sei(); // Enable global interrupts 

	TCNT1 = val; // Preload timer with precalculated value 

	TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set up timer at Fcpu/64 

	PORTC ^= (1 << 7);

	for (;;){
		
	} 
}
 
ISR(TIMER1_OVF_vect) { 
	PORTC ^= (1 << 7); // Toggle the LED 
	TCNT1  = val; // Reload timer with precalculated value 
}
 
