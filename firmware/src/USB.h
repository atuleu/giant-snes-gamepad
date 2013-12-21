#ifndef GSG_USB_H_
#define GSG_USB_H_

#include "LUFAConfig.h"
#include <LUFA/Drivers/USB/USB.h>
#include "Descriptor.h"
#include "../../common/communication.h"

#include "stdint.h"

extern uint16_t Parameters[GSG_NUM_PARAMS];




// Our main Task for the HID 
void InitUSB();
void ProcessUSB();

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void EVENT_USB_Device_StartOfFrame(void);
void EVENT_USB_Device_VendorRequest(uint8_t bRequest,
                                    uint16_t wIndex,
                                    uint16_t wValue);

typedef void(*VendorRequest_fptr)(uint16_t,uint16_t);


#endif //GSG_USB_H_
