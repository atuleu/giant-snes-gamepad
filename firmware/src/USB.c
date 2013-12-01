#include "USB.h"
#include "Descriptor.h"


void RunUSBTask() {
	//Wait for configured device
	if(USB_DeviceState != DEVICE_STATE_Configured ) {
		return;
	}

	Endpoint_SelectEndpoint(GAMEPAD_OUT_EPADDR);

	if(Endpoint_IsOUTReceived()) {
		if (Endpoint_IsReadWriteAllowed()) {
			GSGOutReport_t outReport;
			Endpoint_Read_Stream_LE(&outReport,sizeof(GSGOutReport_t),NULL);
			ProcessHIDReport(&outReport);
		}
		Endpoint_ClearOUT();
	}
	
	Endpoint_SelectEndpoint(GAMEPAD_IN_EPADDR);

	if(Endpoint_IsINReady()) {

		GSGInReport_t inReport;
		SetInHIDReport(&inReport);

		Endpoint_Write_Stream_LE(&inReport,sizeof(GSGInReport_t),NULL);

		Endpoint_ClearIN();
		
	}

	//LUFA's Magic happen here, should be last call
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
	switch (USB_ControlRequest.bRequest){
	case HID_REQ_GetReport:
		if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
			GSGInReport_t inReport;
			SetInHIDReport(&inReport);
				
			Endpoint_ClearSETUP();
				
			/* Write the report data to the control endpoint */
			Endpoint_Write_Control_Stream_LE(&inReport, sizeof(GSGInReport_t));
			Endpoint_ClearOUT();
		}
		break;
	case HID_REQ_SetReport:
		if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
			GSGOutReport_t outReport;
				
			Endpoint_ClearSETUP();
			
			/* Read the report data from the control endpoint */
			Endpoint_Read_Control_Stream_LE(&outReport, sizeof(GSGOutReport_t));
			Endpoint_ClearIN();
			
			ProcessHIDReport(&outReport);
		}
		break;
	}
}



void SetInHIDReport(GSGInReport_t * in) {
	// todo, sets actual data
	in->buttons = 0;
	for(unsigned int i = 0; i < 12; ++i) {
		in->cells[i] = 0;
	}
	in->dummy = 0;
}


void ProcessHIDReport(GSGOutReport_t * out) {
	//STUB
}


