#ifndef GSG_FW_DESCRIPTOR_H_
#define GSG_FW_DESCRIPTOR_H_


#include "LUFAConfig.h"
#include <LUFA/Drivers/USB/USB.h>

//some mandatory structure for LUFA
typedef struct {
	USB_Descriptor_Configuration_Header_t Config;

	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t   HID_GamepadHID;
	USB_Descriptor_Endpoint_t  HID_ReportINEndpoint; //report to the host

	/*	USB_Descriptor_Interface_t Vendor_Interface;
	USB_Descriptor_Endpoint_t  Vendor_DataInEndpoint;
	USB_Descriptor_Endpoint_t  Vendor_DataOutEndpoint;*/


} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors {
	ID_GAMEPAD = 0,
	ID_VENDOR  = 1,
};


enum StringDescriptors_t {
	STR_LANGUAGE		= 0,
	STR_MANUFACTURER    = 1,
	STR_PRODUCT			= 2,
};

typedef struct {
	uint16_t buttons; //Mask for the 16 pressed buttons
} GamepadInReport_t;

typedef struct {
	uint16_t paramID;
	uint16_t paramValue;
} GSGParameter_t;

typedef struct { 
	uint8_t statusAndType;	
	union {
		struct {
			uint16_t cells[12];
		} cellValues;
		GSGParameter_t param[6];
	} data;

} VendorInReport_t;


typedef struct {
	uint8_t  instructionID;
	GSGParameter_t params[6];
} VendorOutReport_t;



#define GAMEPAD_IN_EPADDR ( ENDPOINT_DIR_IN  | 1 )
#define VENDOR_IN_EPADDR  ( ENDPOINT_DIR_IN  | 2 )
#define VENDOR_OUT_EPADDR ( ENDPOINT_DIR_OUT | 3 )

#define GAMEPAD_IN_EPSIZE 2
#define VENDOR_IO_EPSIZE  25


//actually defined in LUFA's headers.
/*uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const descriptorAddress) 
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);*/


#endif //GSG_FW_DESCRIPTOR_H_
