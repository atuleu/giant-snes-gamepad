#include "USB.h"
#include "Descriptor.h"


void InitUSB() {
	USB_Init();
}

void ProcessUSB() {
	//Wait for configured device
	if ( USB_DeviceState != DEVICE_STATE_Configured ) {
		return;
	}
	
	Endpoint_SelectEndpoint(GAMEPAD_IN_EPADDR);

	if(Endpoint_IsINReady()) {

		GamepadInReport_t inReport;
		SetInHIDReport(&inReport);

		Endpoint_Write_Stream_LE(&inReport,sizeof(GamepadInReport_t),NULL);

		Endpoint_ClearIN();
		
	}

	//LUFA's Magic happen here
	USB_USBTask();

	VendorOutReport_t recvData;
	Endpoint_SelectEndpoint(VENDOR_OUT_EPADDR);


	//simply implements a loopback
	if (Endpoint_IsOUTReceived()) {
		Endpoint_Read_Stream_LE(&recvData, VENDOR_IO_EPSIZE, NULL);
		Endpoint_ClearOUT();


		Endpoint_SelectEndpoint(VENDOR_IN_EPADDR);
		Endpoint_Write_Stream_LE(&recvData, VENDOR_IO_EPSIZE, NULL);
		Endpoint_ClearIN();		

	}

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
		if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | 
		                                         REQTYPE_CLASS | 
		                                         REQREC_INTERFACE)) {
			GamepadInReport_t inReport;
			SetInHIDReport(&inReport);
			
			Endpoint_ClearSETUP();
			
			/* Write the report data to the control endpoint */
			Endpoint_Write_Control_Stream_LE(&inReport, sizeof(GamepadInReport_t));
			Endpoint_ClearOUT();
		}
		break;
	}
}


void EVENT_USB_Device_ConfigurationChanged(void) {
	bool res = true;

	res &= Endpoint_ConfigureEndpoint(GAMEPAD_IN_EPADDR, EP_TYPE_INTERRUPT, GAMEPAD_IN_EPSIZE, 1);
	res &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
	res &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);

	// todo report error appropriately

}

void SetInHIDReport(GamepadInReport_t * in) {
	// todo, sets actual data
	in->buttons = 0;
}


