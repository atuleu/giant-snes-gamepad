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
	typedef std::vector<uint16_t>       LoadCellValues;
	// LUFA's VID
	const static uint16_t ID_VENDOR   = 0x03eb;
	// LUFA's Test PID
	const static uint16_t ID_PRODUCT  = 0x2043;
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

	uint16_t GetParam(GSGParam_e);
	void SetParam(GSGParam_e id, uint16_t value);
	void FetchLoadCellValues(LoadCellValues & cells);
	void SaveParamInEEPROM();

private :
	typedef std::shared_ptr<libusb_device>        DevicePtr;
	typedef std::shared_ptr<libusb_device_handle> HandlePtr;
	typedef std::vector<uint16_t>       ListOfParameter;
	
	
	Gamepad(libusb_device * dev);
	void AssertOpened() const;
	void Init();
	void ReadAllParams();	

	DevicePtr d_device;
	HandlePtr d_handle;
	std::mutex d_mutex;

	ListOfParameter d_parameters;
};


inline void Gamepad::AssertOpened() const {
	if ( d_handle ) {
		return;
	}

	throw std::runtime_error("Device is not opened");
}
