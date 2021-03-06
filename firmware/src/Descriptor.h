#ifndef GSG_FW_DESCRIPTOR_H_
#define GSG_FW_DESCRIPTOR_H_


#include "LUFAConfig.h"
#include <LUFA/Drivers/USB/USB.h>
#include "../../common/communication.h"

//some mandatory structure for LUFA
typedef struct {
	USB_Descriptor_Configuration_Header_t Config;

	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t   HID_GamepadHID;
	USB_Descriptor_Endpoint_t  HID_ReportINEndpoint; //report to the host

} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors {
	IF_GAMEPAD = 0,
};


enum StringDescriptors_t {
	STR_LANGUAGE		= 0,
	STR_MANUFACTURER    = 1,
	STR_PRODUCT			= 2,
};

typedef struct {
	uint16_t buttons; //Mask for the 16 pressed buttons
} GamepadInReport_t;





#define GAMEPAD_IN_EPADDR ( ENDPOINT_DIR_IN  | 1 )


#define GAMEPAD_IN_EPSIZE 2


//actually defined in LUFA's headers.
/*uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const descriptorAddress) 
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);*/

#endif //GSG_FW_DESCRIPTOR_H_
