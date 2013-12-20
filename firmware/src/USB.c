#include "USB.h"
#include "Descriptor.h"
#include "Gamepad.h"

void InitUSB() {
	USB_Init();
}

void ProcessUSB() {
	static int once = 1;
	//Wait for configured device
	if ( USB_DeviceState != DEVICE_STATE_Configured ) {
		return;
	}

	Endpoint_SelectEndpoint(GAMEPAD_IN_EPADDR);

	if(Endpoint_IsINReady()) {

		GamepadInReport_t inReport;
		SetHIDReport(&inReport);
		Endpoint_Write_Stream_LE(&inReport,sizeof(GamepadInReport_t),NULL);

		Endpoint_ClearIN();
		
	}
	//LUFA's Magic happen here
	USB_USBTask();

}

void EVENT_USB_Device_Connect(void) {
	//STUB whatever you want
}

void EVENT_USB_Device_Disconnect(void) {
	//STUB whatever you want
}

//Adapted form LUFA's
void EVENT_USB_Device_ControlRequest(void) {
	/* Handle HID Class specific requests */
	uint8_t req = USB_ControlRequest.bRequest;
	switch (req){
	case HID_REQ_GetReport:
		if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | 
		                                         REQTYPE_CLASS | 
		                                         REQREC_INTERFACE)) {
			GamepadInReport_t inReport;
			SetHIDReport(&inReport);
			
			Endpoint_ClearSETUP();
			
			/* Write the report data to the control endpoint */
			Endpoint_Write_Control_Stream_LE(&inReport, sizeof(GamepadInReport_t));
			Endpoint_ClearOUT();
		}
		break;
	case REQ_GetDescriptor :
	case REQ_GetConfiguration :
	case REQ_SetConfiguration :
	case REQ_SetAddress :
	case 0x0a :
		break;
	default :
		DisplayValue(req);
		break;
		//default :
		//ReportError(4);
	}
}


void EVENT_USB_Device_ConfigurationChanged(void) {
	bool res = true;
	res &= Endpoint_ConfigureEndpoint(GAMEPAD_IN_EPADDR, EP_TYPE_INTERRUPT, GAMEPAD_IN_EPSIZE, 1);

	if ( res == false) {
		ReportError(3);
	}

}

void SetInHIDReport(GamepadInReport_t * in) {
	// todo, sets actual data
	in->buttons = 0;
}


