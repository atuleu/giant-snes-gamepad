#ifndef GSG_USB_H_
#define GSG_USB_H_

#include "LUFAConfig.h"
#include <LUFA/Drivers/USB/USB.h>
#include "Descriptor.h"
#include "GSG.h"

void InitUSBTask();

// Our main Task for the HID 
void RunUSBTask();


void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);

//Sets an in report from app data
void SetInHIDReport(GamepadInReport_t * report);
                     

#endif //GSG_USB_H_
