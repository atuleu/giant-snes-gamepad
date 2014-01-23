#include <avr/io.h> 
#include <avr/power.h>
#include <avr/wdt.h>


#include "Systime.h"
#include "Gamepad.h"
#include "USB.h"

void SetupHardware() {
	// Bootloader workaround testing. 
	// 
	// Datasheet says that to power off the USB interface we have to do 'some' of: 
	//       Detach USB interface 
	//      Disable USB interface 
	//      Disable PLL 
	//      Disable USB pad regulator 

	// Disable the USB interface 
	USBCON &= ~(1 << USBE); 
    
	// Disable the VBUS transition enable bit 
	USBCON &= ~(1 << VBUSTE); 
    
	// Disable the VUSB pad 
	USBCON &= ~(1 << OTGPADE); 
    
	// Freeze the USB clock 
	USBCON &= ~(1 << FRZCLK); 
    
	// Disable USB pad regulator 
	UHWCON &= ~(1 << UVREGE); 
    
	// Clear the IVBUS Transition Interrupt flag 
	USBINT &= ~(1 << VBUSTI); 
    
	// Physically detact USB (by disconnecting internal pull-ups on D+ and D-) 
	UDCON |= (1 << DETACH); 

	//Arduino's micro bootloader let the USB interrupt on, but if we
	//are not using USB, well it blows our face because ISR routine
	//are not there. We disable the interrupt. Maybe a USB clock
	//freeze will be great too. This is teh first step because we do
	//not want our USB host to wake and crash us.

	//	UDIEN &= ~(_BV(SUSPE) | _BV(EORSTE) );

	//this is not stricly needed as set by Arduino Micro's
	//bootloader. But just to be absolutely certain :

	MCUSR &= ~_BV(WDRF); // clear watchdog flag
	wdt_disable(); //disbale it
	
	//no prescale of F_CPU
	clock_prescale_set(clock_div_1);

	//rest of the init here

	
	InitSystime();

	InitGamepad();

	InitUSB();


	//we need to start ADC here !
	ADCSRA |= _BV(ADEN);
	ADCSRA |= _BV(ADSC);
	
}



#define LOOP_IN_MS 1000

int main (void) { 
	SetupHardware();

	while(1) {
		//highest priority to USB
		ProcessUSB();

		ProcessGamepad();

	}
}
 

//ISR(USB_COM_vect){}
//ISR(USB_GEN_vect){}
