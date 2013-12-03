#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h> 
#include <avr/power.h>
#include <avr/wdt.h>


#include "Systime.h"
#include "Gamepad.h"


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

   
	InitSystime();
	InitGamepad();
}

#define LOOP_IN_MS 1000

int main (void) { 
	SetupHardware();

	while(1) {

		ProcessGamepad();

	}
}
 

//ISR(USB_COM_vect){}
//ISR(USB_GEN_vect){}
