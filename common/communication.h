#ifndef GSG_COMMON_COMMUNICATION_H_
#define GSG_COMMON_COMMUNICATION_H_



#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


	// types of packet that can be send back from the GSG
	typedef enum {
		VI_TYPE_INVALID   = 0,
		VI_TYPE_CONTROL_RETURN = 1,
		VI_TYPE_CELL_VALUES    =2,
	} VendorInReportType_e;

	

	// All errors type return by the GSG
	typedef enum {
		VI_ERR_NO_ERROR = 0,
		

	} VendorInError_e;

	// A Packeted parameter, concatenation of An ID and a 16 bit value
	typedef struct {
		uint8_t paramID;
		uint16_t paramValue;
	} GSGParameter_t;

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
		// 1 - 3 reserved
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
		INST_READ_PARAMS       = 1,
		INST_SET_PARAMS        = 2,
		INST_SAVE_IN_EEPROM    = 3,
		INST_FETCH_CELL_VALUES = 4,
	} GSGHostInstruction_t;


	typedef struct { 
		uint8_t type;
		uint8_t error;
		union {
			uint16_t cells[12];
			GSGParameter_t param[8];
		} data;
	} VendorInReport_t;
	
	
	typedef struct {
		uint16_t  instructionID;		
		GSGParameter_t params[8];
	} VendorOutReport_t;
	
	
	
#ifdef __cplusplus
}
#endif //__cplusplus



#endif //GSG_COMMON_COMMUNICATION_H_
