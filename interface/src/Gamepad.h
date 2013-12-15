#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include <libusb.h>

#include "../../common/communication.h"
#include <mutex>

class Gamepad {
public :
	typedef std::shared_ptr<Gamepad>    Ptr;
	typedef std::vector<Gamepad::Ptr>   List;
	typedef std::vector<GSGParameter_t> ListOfParameter;
	typedef std::vector<GSGParam_e>     ListOfParameterID;
	typedef std::vector<uint16_t>       LoadCellValues;
	// LUFA's VID
	const static uint16_t ID_VENDOR   = 0x03eb;
	// LUFA's Test PID
	const static uint16_t ID_PRODUCT  = 0x2040;
	// CUSTOM Fixed BCD VERSION
	const static uint16_t BCD_VERSION = 0x6942;
	// Vendor Specific Class

	const static uint8_t SUBCLASS_NON_BOOT        = 0x00;
	const static uint8_t PROTOCOL_NON_BOOT        = 0x00;
	const static uint8_t CLASS_VENDOR_SPECIFIC    = 0xff;
	const static uint8_t SUBCLASS_VENDOR_SPECIFIC = 0xff;
	const static uint8_t PROTOCOL_VENDOR_SPECIFIC = 0xff;

	static List ListAll();
	
	~Gamepad();

	void Open();
	void Close();

	unsigned int MaxRetries() const;
	void SetMaxRetries(unsigned int max); 

	void SetParameter(GSGParam_e parameter, uint16_t value);
	uint16_t GetParameter(GSGParam_e parameter);
	void GetParameters(const ListOfParameterID & ids, ListOfParameter & result);

	const LoadCellValues & GetCells();
	
	void SaveParamInEEPROM();

private :
	typedef std::shared_ptr<libusb_device>        DevicePtr;
	typedef std::shared_ptr<libusb_device_handle> HandlePtr;
	
	
	Gamepad(libusb_device * dev);
	void AssertOpened() const;
	void Init();


	// Bulk All
	void BulkAll(bool in, uint8_t * data , size_t size) const;

	VendorInReport_t SendInstruction(GSGHostInstruction_e inst,const ListOfParameter & params);

	void CheckResponse(const VendorInReport_t & report,
	                   VendorInReportType_e expected, 
	                   const std::string & context);
	void GetParametersPrivate(const ListOfParameterID::const_iterator & begin, 
	                          const ListOfParameterID::const_iterator & end,
	                          ListOfParameter & result);
	

	DevicePtr d_device;
	HandlePtr d_handle;

	uint8_t d_bulkInEP,d_bulkOutEP;
	int     d_vendorInterface;
	unsigned int d_maxRetries;

	LoadCellValues d_cellValues;
	std::mutex d_mutex;
};


inline void Gamepad::AssertOpened() const {
	if ( d_handle ) {
		return;
	}

	throw std::runtime_error("Device is not opened");
}
