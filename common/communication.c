#include "communication.h"

#ifndef NULL 
#define NULL ( (void*) 0 )
#endif

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

GSGInstructionMetadata_t IMetaData[INST_NUMBER_OF_INSTRUCTION] =
	{ { REQ_VENDOR_IN , 2 * GSG_NUM_PARAMS , NULL }, // READ_ALL_PARAMS
	  { REQ_VENDOR_OUT , 0 , NULL }, // SET_PARAM
	  { REQ_VENDOR_OUT , 0 , NULL }, // SAVE_IN_EEPROM
	  { REQ_VENDOR_IN , 2 * 12 , NULL } // FETCH_CELL_VALUES
	};


#ifdef __cplusplus
}
#endif //__cplusplus
