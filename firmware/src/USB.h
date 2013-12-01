#ifndef GSG_USB_H_
#define GSG_USB_H_

#include "LUFAConfig.h"
#include <LUFA/Drivers/USB/USB.h>
#include "Descriptor.h"
#include "GSG.h"

typedef struct {
	uint16_t buttons; //Mask for the 16 pressed buttons
	uint8_t cells[12]; //raw 12 load cells value reading
	uint16_t dummy; //to make it 16 bytes
} GSGInReport_t;

typedef struct {
	uint8_t  data[GAMEPAD_OUT_EPSIZE];
} GSGOutReport_t;

void InitUSBTask();

// Our main Task for the HID 
void RunUSBTask();


void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

//Sets an in report from app data
void SetInHIDReport(GSGInReport_t * report);

//Sets an out report from app data
void ProcessHIDReport(GSGOutReport_t * report);
                     

#endif //GSG_USB_H_
