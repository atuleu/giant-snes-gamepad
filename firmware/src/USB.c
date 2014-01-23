#include "USB.h"
#include "Descriptor.h"
#include "Gamepad.h"
#include <avr/eeprom.h>

#include "Systime.h"

uint16_t Parameters[GSG_NUM_PARAMS];

void eeprom_update_word(uint16_t * address, uint16_t value) {
	uint16_t curVal = eeprom_read_word(address);
	if ( curVal == value ) {
		return;
	}
	eeprom_write_word(address,value);
}


void ReadAllCallback(uint16_t index, uint16_t value) {
	Endpoint_ClearSETUP();

	/* Write the report data to the control endpoint */
	Endpoint_Write_Control_Stream_LE(&Parameters, sizeof(Parameters));
	//Clear IN called above
	//acknowledge the transaction
	Endpoint_ClearOUT();
}

void SetParamCallback(uint16_t index, uint16_t value) {
	if(index >= GSG_NUM_PARAMS ) {
		return;
	}

	Endpoint_ClearSETUP();
	Parameters[index] = value;
	//acknowledge the transaction
	Endpoint_ClearStatusStage();
	
}
void SaveInEEPROMCallback(uint16_t index, uint16_t value) {
	Endpoint_ClearSETUP();
	for ( unsigned int i = 0; i < GSG_NUM_PARAMS; ++i ) {
		eeprom_update_word( (uint16_t *) ( 2 * i + 2 ) , Parameters[i] );
	}
	//Clear IN called above
	//acknowledge the transaction
	Endpoint_ClearStatusStage();
	
}

void FetchCellValueCallback(uint16_t index, uint16_t value) {
	uint16_t val[NUM_BUTTONS];
	for(uint8_t i =0 ; i < NUM_BUTTONS; ++i ) {
		val[i] = GetSystime() >> 8;
	}
	Endpoint_ClearSETUP();
	/* Write the report data to the control endpoint */
	Endpoint_Write_Control_Stream_LE(GetCellValues(), sizeof(uint16_t) * NUM_BUTTONS);
	// ClearIN called by above

	// acknowledge the status transaction
	Endpoint_ClearOUT();
}


void InitUSB() {
	USB_Init();


	// Loads the param value from the EEPROM
	for ( uint8_t i = 0; i < GSG_NUM_PARAMS; ++i ) {
		uint16_t * address = (uint16_t *) (2 * i + 2);
		Parameters[i] = eeprom_read_word( address );
		
		//sets
		if (Parameters[i] != 0xffff ) {
			// parameter is not uninitialized
			continue;
		}

		switch ( ( i - CELL_1  ) % CELL_MAX_PARAMS) {
		case CELL_THRESHOLD :
			Parameters[i] = 1 << 10;
			break;
		case CELL_RELEASE :
			Parameters[i] = 0;
			break;
		default :
			continue;
		}

		eeprom_update_word( address , Parameters[i] ); 

	}

	// Loads all Callback
	IMetaData[INST_READ_ALL_PARAMS].userData = &ReadAllCallback;
	IMetaData[INST_SET_PARAM].userData = &SetParamCallback;
	IMetaData[INST_SAVE_IN_EEPROM].userData = &SaveInEEPROMCallback;
	IMetaData[INST_FETCH_CELL_VALUES].userData = &FetchCellValueCallback;
	

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

	if ( IS_REQ_VENDOR(USB_ControlRequest.bmRequestType) ) {
		EVENT_USB_Device_VendorRequest(USB_ControlRequest.bRequest,
		                               USB_ControlRequest.wIndex,
		                               USB_ControlRequest.wValue);
		return;
	}

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
	}
}



void EVENT_USB_Device_ConfigurationChanged(void) {
	bool res = true;
	res &= Endpoint_ConfigureEndpoint(GAMEPAD_IN_EPADDR, EP_TYPE_INTERRUPT, GAMEPAD_IN_EPSIZE, 1);
}

void SetInHIDReport(GamepadInReport_t * in) {
	// todo, sets actual data
	in->buttons = 0;
}



void EVENT_USB_Device_VendorRequest(uint8_t bRequest, 
                                    uint16_t wIndex,
                                    uint16_t wValue) {
	if (bRequest >= INST_NUMBER_OF_INSTRUCTION) {
		return;
	} 
	(*(VendorRequest_fptr)IMetaData[bRequest].userData)(wIndex,
	                                                    wValue);
}




