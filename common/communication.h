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

	// List of parameters flags
	typedef enum {
		NOT_A_PARAM    = 0,
		// param is readable by host
		PARAM_READ       = (1 << 0),
		// param is writable by host
		PARAM_WRITE      = (1 << 1),
		// param is persistent to reboot
		PARAM_PERSISTENT = (1 << 2),
		// param is a signed value
		PARAM_SIGNED     = (1 << 3), 
	} GSGParamFlags_e;

#define PARAM_RWP (PARAM_READ | PARAM_WRITE | PARAM_PERSISTENT)

	// Parameter for a load cell
	typedef enum {
		CELL_GAIN                 = 0,
		CELL_TRESHOLD_UP          = 1,
		CELL_TRESHOLD_DOWN        = 2,
		CELL_PARAM_RESERVED_START = 3,
		CELL_MAX_PARAMS           = 4,
	} GSGCellParam_e;

	// Lists all parameters for a GSG
	typedef enum {
		PARAM_NULL_ID  = 0,
		// 1 - 3 reserved
		CELL_1         = CELL_MAX_PARAMS * 1,  // 0x04
		CELL_2         = CELL_MAX_PARAMS * 2,  // 0x08
		CELL_3         = CELL_MAX_PARAMS * 3,  // 0x0c
		CELL_4         = CELL_MAX_PARAMS * 4,  // 0x10
		CELL_5         = CELL_MAX_PARAMS * 5,  // 0x14
		CELL_6         = CELL_MAX_PARAMS * 6,  // 0x18
		CELL_7         = CELL_MAX_PARAMS * 7,  // 0x1c
		CELL_8         = CELL_MAX_PARAMS * 8,  // 0x20
		CELL_9         = CELL_MAX_PARAMS * 9,  // 0x24
		CELL_10        = CELL_MAX_PARAMS * 10, // 0x28
		CELL_11        = CELL_MAX_PARAMS * 11, // 0x2c
		CELL_12        = CELL_MAX_PARAMS * 12, // 0x30
		GSG_NUM_PARAMS = CELL_MAX_PARAMS * 13,
	} GSGParam_e;
	
#define CELL_PARAM(nbCell,param) ( CELL_NUM_PARAMS * (nbCell) + (param))
	
	uint8_t GSGParamFlags[GSG_NUM_PARAMS] = { NOT_A_PARAM,NOT_A_PARAM,NOT_A_PARAM,NOT_A_PARAM, // 0-3 are invalid
	                                          // CELL_1
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_2
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_3
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_4
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_5
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_6
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_7
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_8
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_9
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_10
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_11
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM,
	                                          // CELL_12
	                                          // GAIN  ,TRESH_UP,THRESH_D  , RESERVERD
	                                          PARAM_RWP,PARAM_RWP,PARAM_RWP,NOT_A_PARAM};

	typedef enum {
		INST_READ_PARAMS       = 1,
		INST_SET_PARAMS        = 2,
		INST_FETCH_CELL_VALUES = 3,,
	} GSGHostInstruction_t;


	typedef struct { 
		uint8_t type;
		uint8_t error;
		union {
			struct {
				uint16_t cells[12];
				} cellValues;
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
