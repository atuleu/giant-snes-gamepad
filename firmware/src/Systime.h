#ifndef GSG_SYSTIME_H_
#define GSG_SYSTIME_H_


#include <avr/io.h>

typedef uint16_t Systime_t;

// Inits the Systime module. Should be called at application
//  startup. Enables Global interrupt.
void InitSystime();

// Return the number of clock tick in ms since initialization.  This
// value can overlap on 0xffff -> 0x0000. Only suitable for comparison
// of less thab 65,635 seconds == 0xffff ms.
Systime_t GetSystime();


#endif //GSG_SYSTIME_H_



