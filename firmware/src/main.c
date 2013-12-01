#include <avr/wdt.h>
#include <avr/power.h> 


#include "USB.h"
#include "GSG.h"


void SetupHardware() {
	//disbale watchdog
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	//no prescaling
	clock_prescale_set(clock_div_1);

}

int main() {
	SetupHardware();
	//Initialize all our application
	Init();
	//starts the USB stack
	USB_Init();


	while(true) {
		//process the usb
		RunUSBTask();
		
		//process the app
		Process();

	}

	return 0;
}



