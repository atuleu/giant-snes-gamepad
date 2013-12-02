#include "Descriptor.h"



const USB_Descriptor_HIDReport_Datatype_t PROGMEM GamepadReport[] =
{
	// \todo understand and set
};


const USB_Descriptor_Device_t PROGMEM DeviceDescriptor ={
	.Header						= {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification			= VERSION_BCD(1,1,0),
	.Class						= USB_CSCP_NoDeviceClass,
	.SubClass					= USB_CSCP_NoDeviceSubclass,
	.Protocol					= USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size				= FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID					= 0x03EB, // LUFA's VID
	.ProductID					= 0x2040, // LUFA Test PID / VID
	.ReleaseNumber				= VERSION_BCD(0x69,0x04,0x02), //Unique version number to test if its ours device
	

	.ManufacturerStrIndex		= STR_MANUFACTURER,
	.ProductStrIndex			= STR_PRODUCT,
	.SerialNumStrIndex			= USE_INTERNAL_SERIAL,

	.NumberOfConfigurations		= FIXED_NUM_CONFIGURATIONS
};


const USB_Descriptor_String_t PROGMEM LanguageString = {
	.Header				= {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},
	.UnicodeString		= {LANGUAGE_ID_ENG}
};

const USB_Descriptor_String_t PROGMEM ManufacturerString = {
	.Header				= {.Size = USB_STRING_LEN(12), .Type = DTYPE_String},
	.UnicodeString		= L"Bar Satellite"
};

const USB_Descriptor_String_t PROGMEM ProductString	= {
	.Header				= {.Size = USB_STRING_LEN(18), .Type = DTYPE_String},
	.UnicodeString		= L"Giant SNES Gamepad"
};



const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
	.Config = {
		.Header					= {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },
		
		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces		= 2, //change here if other interfaces
		
		.ConfigurationNumber	= 1,
		.ConfigurationStrIndex	= NO_DESCRIPTOR,

		.ConfigAttributes		= (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED ),
		
		.MaxPowerConsumption	= USB_CONFIG_POWER_MA(100)
	},

	//add another one like size if more is needed
	.HID_Interface = {
		.Header					= {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
		
		.InterfaceNumber		= ID_GAMEPAD,
		.AlternateSetting		= 0x00,

		.TotalEndpoints			= 1,


		.Class					= HID_CSCP_HIDClass,
		.SubClass				= HID_CSCP_NonBootSubclass,
		.Protocol				= HID_CSCP_NonBootProtocol,

		.InterfaceStrIndex		= NO_DESCRIPTOR
	},


	.HID_GamepadHID = {
		.Header					= {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
		
		.HIDSpec				= VERSION_BCD(1,1,1),
		.CountryCode			= 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType			= HID_DTYPE_Report,
		.HIDReportLength		= sizeof(GamepadReport)
	},

	.HID_ReportINEndpoint = {
		.Header					= {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
		
		.EndpointAddress		= GAMEPAD_IN_EPADDR,
		.Attributes				= (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize			= GAMEPAD_IN_EPSIZE,
		.PollingIntervalMS		= 0x05 // \todo : reconsider increasing this

	},

	.Vendor_Interface = {
		.Header					= {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
		.InterfaceNumber		= ID_VENDOR,
		.AlternateSetting		= 0,
		.TotalEndpoints			= 2,

		.Class					= 0xFF, // vendor specific =)
		.SubClass				= 0xFF, // vendor specific =)
		.Protocol				= 0xFF, // vendor specific =)

		.InterfaceStrIndex		= NO_DESCRIPTOR

	},

	.Vendor_DataInEndpoint = {
		.Header					= { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },

		.EndpointAddress		= VENDOR_IN_EPADDR,
		.Attributes				= (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize			= VENDOR_IO_EPSIZE,
		.PollingIntervalMS		= 0x05, //\todo consider change this
	},

	.Vendor_DataOutEndpoint = {
		.Header					= { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },

		.EndpointAddress		= VENDOR_OUT_EPADDR,
		.Attributes				= (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize			= VENDOR_IO_EPSIZE,
		.PollingIntervalMS		= 0x05, //\todo consider change this
	}


};



uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const descriptorAddress) {

	const uint8_t descriptorType = (wValue >> 8);
	const uint8_t descriptorNumber = (wValue & 0xff);
	
	const void * address = NULL;
	uint16_t size = NO_DESCRIPTOR;


	switch(descriptorType) {
	case DTYPE_Device :
		address = &DeviceDescriptor;
		size = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		address = &ConfigurationDescriptor;
		size    = sizeof(USB_Descriptor_Configuration_t);
			break;
	case DTYPE_String:
		switch(descriptorNumber) {
		case STR_LANGUAGE:
			address = &LanguageString;
			size    = pgm_read_byte(&LanguageString.Header.Size);
			break;
		case STR_MANUFACTURER:
			address = &ManufacturerString;
			size    = pgm_read_byte(&ManufacturerString.Header.Size);
			break;
		case STR_PRODUCT:
			address = &ProductString;
			size    = pgm_read_byte(&ProductString.Header.Size);
			break;
		}

		break;
	case HID_DTYPE_HID:
		address = &ConfigurationDescriptor.HID_GamepadHID;
		size    = sizeof(USB_HID_Descriptor_HID_t);
		break;
	case HID_DTYPE_Report:
		address = &GamepadReport;
		size    = sizeof(GamepadReport);
		break;
	}

	*descriptorAddress = address;
	return size;
}
