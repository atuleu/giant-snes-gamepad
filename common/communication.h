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
	CELL_TRESHOLD_UP          = 0,
	CELL_TRESHOLD_DOWN        = 1,
	//		CELL_PARAM_RESERVED_START = 2,
	CELL_MAX_PARAMS           = 2,
} GSGCellParam_e;

// Lists all parameters for a GSG
typedef enum {
	PARAM_NULL_ID  = 0,
	LED_PERIOD     = 1,
	CELL_1         = CELL_MAX_PARAMS * 1,  // 0x02
	CELL_2         = CELL_MAX_PARAMS * 2,  // 0x04
	CELL_3         = CELL_MAX_PARAMS * 3,  // 0x06
	CELL_4         = CELL_MAX_PARAMS * 4,  // 0x08
	CELL_5         = CELL_MAX_PARAMS * 5,  // 0x0a
	CELL_6         = CELL_MAX_PARAMS * 6,  // 0x0c
	CELL_7         = CELL_MAX_PARAMS * 7,  // 0x0e
	CELL_8         = CELL_MAX_PARAMS * 8,  // 0x10
	CELL_9         = CELL_MAX_PARAMS * 9,  // 0x12
	CELL_10        = CELL_MAX_PARAMS * 10, // 0x14
	CELL_11        = CELL_MAX_PARAMS * 11, // 0x16
	CELL_12        = CELL_MAX_PARAMS * 12, // 0x18
	GSG_NUM_PARAMS = CELL_MAX_PARAMS * 13, // 0x1a
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

#ifdef __cplusplus
}
#endif //__cplusplus


#ifdef REQDIR_DEVICETOHOST
#define REQ_VENDOR ( REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE )
#else 
#define REQ_VENDOR ( ( 1<< 7 ) | ( 2 << 5 ) | ( 0 << 0 ) )
#endif 

#endif //GSG_COMMON_COMMUNICATION_H_
