#ifndef GSG_GAMEPAD_H_
#define GSG_GAMEPAD_H_

#include "Descriptor.h"

//default module init / process

void InitGamepad();
void ProcessGamepad();

// interfaces

// Error and diagnosis report
//void ReportError(uint8_t error);
//void DisplayValue(uint8_t value);


uint16_t * GetCellValues();
uint8_t * GetCellCount();

// USB HID 
void SetHIDReport(GamepadInReport_t * r);


#endif //GSG_GAMEPAD_H_
