#ifndef GSG_COMMON_COMMUNICATION_H_
#define GSG_COMMON_COMMUNICATION_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


// All errors type return by the GSG
typedef enum {
	GSG_ERR_NO_ERROR = 0,
	GSG_ERROR_MAX
} VendorInError_e;


// Parameter for a load cell
typedef enum {
	CELL_THRESHOLD   = 0,
	CELL_RELEASE    = 1,
	CELL_MAX_PARAMS = 2
} GSGCellParam_e;

// Lists all parameters for a GSG
typedef enum {
	LED_PERIOD     = 0,
	CELL_1         = 1,
	CELL_2         = CELL_1  + CELL_MAX_PARAMS, // 0x03
	CELL_3         = CELL_2  + CELL_MAX_PARAMS, // 0x05
	CELL_4         = CELL_3  + CELL_MAX_PARAMS, // 0x07
	CELL_5         = CELL_4  + CELL_MAX_PARAMS, // 0x09
	CELL_6         = CELL_5  + CELL_MAX_PARAMS, // 0x0b
	CELL_7         = CELL_6  + CELL_MAX_PARAMS, // 0x0d
	CELL_8         = CELL_7  + CELL_MAX_PARAMS, // 0x0f
	CELL_9         = CELL_8  + CELL_MAX_PARAMS, // 0x11
	CELL_10        = CELL_9  + CELL_MAX_PARAMS, // 0x13
	CELL_11        = CELL_10 + CELL_MAX_PARAMS, // 0x15
	CELL_12        = CELL_11 + CELL_MAX_PARAMS, // 0x17
	GSG_NUM_PARAMS = CELL_12 + CELL_MAX_PARAMS, // 0x19
} GSGParam_e;

	
#define CELL_PARAM(nbCell,param) ( CELL_NUM_PARAMS * (nbCell) + (param))
	
typedef enum {
	INST_READ_ALL_PARAMS   = 0,
	INST_SET_PARAM         = 1,
	INST_SAVE_IN_EEPROM    = 2,
	INST_FETCH_CELL_VALUES = 3,
	INST_NUMBER_OF_INSTRUCTION
} GSGHostInstruction_e;



typedef struct {
	const uint8_t  bmRequestType;
	const uint16_t wLength;
	void *         userData;
} GSGInstructionMetadata_t;

extern GSGInstructionMetadata_t IMetaData[INST_NUMBER_OF_INSTRUCTION];


typedef enum  {
	UP            = 0,
	DOWN          = 1,
	LEFT          = 2,
	RIGHT         = 3,
	A             = 4,
	B             = 5,
	X             = 6,
	Y             = 7,
	START         = 8,
	SELECT        = 9,
	TRIGGER_LEFT  = 10,
	TRIGGER_RIGHT = 11,
	NUM_BUTTONS   = 12
} GSGButton_e;

extern GSGParam_e CellByButton[NUM_BUTTONS];

#ifdef __cplusplus
}
#endif //__cplusplus


#ifdef REQDIR_DEVICETOHOST
#define REQ_VENDOR_IN ( REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE )
#define REQ_VENDOR_OUT ( REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE )
#else 
#define REQ_VENDOR_IN ( ( 1 << 7 ) | ( 2 << 5 ) | ( 0 << 0 ) )
#define REQ_VENDOR_OUT ( (  0 << 7 ) | ( 2 << 5 ) | ( 0 << 0 ) )
#endif 

#define IS_REQ_VENDOR(req) ( ( (req) & 0x7f ) == REQ_VENDOR_OUT )

#endif //GSG_COMMON_COMMUNICATION_H_
