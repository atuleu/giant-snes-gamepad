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

	VendorOutReport_t recvData;

	VendorInReport_t sendData;
	memset(&sendData,0,sizeof(sendData));
	sendData.type = VI_TYPE_PARAM_RETURN;
	sendData.error = 0;
	sendData.data.params[0].ID = LED_PERIOD;
	sendData.data.params[1].ID = CELL_1;
	sendData.data.params[2].ID = CELL_2;
	sendData.data.params[3].ID = CELL_3;
	sendData.data.params[4].ID = CELL_4;
	sendData.data.params[5].ID = CELL_5;
	sendData.data.params[6].ID = CELL_6;
	sendData.data.params[7].ID = CELL_7;

	static uint8_t needToSend = 0;
	Endpoint_SelectEndpoint(VENDOR_OUT_EPADDR);
	//simply implements a loopback
	if (Endpoint_IsOUTReceived()) {
		Endpoint_Read_Stream_LE(&recvData, VENDOR_OUT_EPSIZE, NULL);
		Endpoint_ClearOUT();
		needToSend = 1;
	}

	if( needToSend == 1 &&
	    recvData.instructionID == INST_READ_PARAMS && 
	    recvData.params[0].ID == LED_PERIOD &&
	    recvData.params[1].ID == CELL_1 &&
	    recvData.params[2].ID == CELL_2 &&
	    recvData.params[3].ID == CELL_3 &&
	    recvData.params[4].ID == CELL_4 &&
	    recvData.params[5].ID == CELL_5 &&
	    recvData.params[6].ID == CELL_6 &&
	    recvData.params[7].ID == CELL_7 ) {
		

		needToSend = 2;
	}


	Endpoint_SelectEndpoint(VENDOR_IN_EPADDR);
	if(needToSend == 2 && Endpoint_IsINReady()) {
		uint8_t error;
		uint8_t trials = 1;
		while( ( error = Endpoint_Write_Stream_LE(&sendData, VENDOR_IN_EPSIZE, NULL) ) == ENDPOINT_RWSTREAM_IncompleteTransfer ) {
			++trials;
		}
		//DisplayValue(trials);
		
		Endpoint_ClearIN();
		needToSend = 0;
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
	res &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR, EP_TYPE_BULK, VENDOR_IN_EPSIZE, 1);
	res &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_BULK, VENDOR_OUT_EPSIZE, 1);

	if ( res == false) {
		ReportError(3);
	}

}

void SetInHIDReport(GamepadInReport_t * in) {
	// todo, sets actual data
	in->buttons = 0;
}


