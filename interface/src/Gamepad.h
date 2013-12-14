#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include <libusb.h>


class Gamepad {
public :
	typedef std::shared_ptr<Gamepad> Ptr;
	typedef std::vector<Gamepad::Ptr> List;

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

private :
	typedef std::shared_ptr<libusb_device>        DevicePtr;
	typedef std::shared_ptr<libusb_device_handle> HandlePtr;

	
	
	Gamepad(libusb_device * dev);
	void AssertOpened();
	void Init();
	
	DevicePtr d_device;
	HandlePtr d_handle;

	uint8_t d_bulkInEP,d_bulkOutEP;

};
