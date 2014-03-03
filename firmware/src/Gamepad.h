#ifndef GSG_GAMEPAD_H_
#define GSG_GAMEPAD_H_

#include "Descriptor.h"

//default module init / process

void InitGamepad();
void ProcessGamepad();

// interfaces

typedef enum {
	DEVICE_UNCONNECTED = 0x0f,
	INVALID_ERROR = 0xa,
	INVALID_STATE_FOR_DISPLAY = 0x01,
} DeviceError_e;

// Error and diagneosis report
typedef enum {
	PLAYER_1 = 0,
	PLAYER_2 = 1,
	PLAYER_3 = 2,
	PLAYER_4 = 3,
	WAIT_FOR_PLAYER = 4
} DisplayState_e;

void DisplayState(DisplayState_e s);
void ReportError(DeviceError_e error);
//void DisplayValue(uint8_t value);


uint16_t * GetCellValues();
uint8_t * GetCellCount();


// USB HID 
void SetHIDReport(GamepadInReport_t * r);


#endif //GSG_GAMEPAD_H_
