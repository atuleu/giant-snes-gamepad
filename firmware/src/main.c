#include <LUFA/Drivers/USB/USB.h>

enum InterfaceIDs {
	Joystick = 0;
};

static uint8_t PrevGamepadHIDReportBuffer[sizeof(USB_GamepadReport_Data_t)];

USB_ClassInfo_HID_Device_t Gamepad_HID_Interface = {
	{
		.Config =
		{ 
			.InterfaceNumber = 0,
			.ReportINEndpoint = 
			{
				.Address = (ENDPOINT_DIR_IN | 1),
				.Size    = 12,
				.Banks   = 1,
			}
			.PrevReportInBuffer = 

}

int main() {


	while(true) {
		HID_Device_USBTask();
		USB_USBTask();

	}

	return 0;
}
