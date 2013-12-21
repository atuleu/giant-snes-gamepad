#include "Gamepad.h"

#include "common.h"
#include <libusb.h> 
#include <iomanip>
#include <cstring>
#include <map>

#include <glog/logging.h>




Gamepad::Gamepad(libusb_device * device) 
	: d_device(device, &libusb_unref_device) {
	
#ifdef LIBUSB_DARWIN_WORKAROUND
	//need to open to avoid segfault of libsusb, see
	//http://www.libusb.org/ticket/171
	libusb_device_handle * handle;
	try { 
		d_mutex.lock();
		lusb_call(libusb_open,d_device.get(),&handle);
#endif
	Init();
#ifdef LIBUSB_DARWIN_WORKAROUND
	// uneeded, but would like not to forget Open() call for
	// cross-platform development
	//Close();
	} catch(...) {
		libusb_close(handle);
		d_mutex.unlock();
		throw;
	}
	libusb_close(handle);
	d_mutex.unlock();	
#endif

}


Gamepad::~Gamepad() {
	//explicitely close the thread
	Close();
}


class LUsbDeviceListDeleter {
public :
	LUsbDeviceListDeleter(bool decrement) :
		d_decrement(decrement) {
	}
	void operator()(libusb_device ** list) {
		libusb_free_device_list(list,d_decrement ? 1 : 0);
	}
private :
	bool d_decrement;
};

std::ostream & operator << (std::ostream & out, const struct libusb_device_descriptor & desc) {
	std::ios::fmtflags old = out.flags();
	out << std::hex << std::setfill('0') 
	    << "{ vid: 0x" << std::setw(4) << desc.idVendor 
	    << " pid: 0x" << std::setw(4) << desc.idProduct 
	    << " bcd: 0x" << std::setw(4) << desc.bcdDevice 
	    << " }";
	out.flags(old);
	return out;
}


Gamepad::List Gamepad::ListAll() {
	libusb_device **list;
	const int count(libusb_get_device_list(NULL, &list));
	if (count < 0) {
		lusb_throw(libusb_get_device_list,count);
	}

	std::shared_ptr<libusb_device*> listPtr(list,LUsbDeviceListDeleter(false));

	List res;

	for (unsigned int i = 0; i < count; ++i) {
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		lusb_call(libusb_get_device_descriptor,list[i],&desc);
		LOG(INFO) << "Found USB Device " << desc;
		
		if( desc.idVendor  != ID_VENDOR  ||
		    desc.idProduct != ID_PRODUCT ||
		    desc.bcdDevice != BCD_VERSION   ) {
			libusb_unref_device(device);
			continue;
		}
		
		try { 
			res.push_back(Ptr(new Gamepad(device)));
		} catch (const std::exception & e) {
			LOG(WARNING) << "USB device has right (vid,pid,bcd), but cannot create wrapper : " 
			             << e.what();
			continue;
		}

		
	}
	return res;
	

}

void Gamepad::Open() {
	d_mutex.lock();
	if(d_handle) {
		d_mutex.unlock();
		return;
	}
	try {
		libusb_device_handle * handle;
		lusb_call(libusb_open,d_device.get(),&handle);
		d_handle = HandlePtr(handle,&libusb_close);
		//lusb_call(libusb_claim_interface,d_handle.get(),d_vendorInterface);
		
	} catch(...) {
		d_mutex.unlock();
		throw;
	}
	d_mutex.unlock();
}

void Gamepad::Close() {
	d_mutex.lock();
	try {
		//		lusb_call(libusb_release_interface,d_handle.get(),d_vendorInterface);
	} catch( const std::exception & e) {
		LOG(ERROR) << "Got error on Gamepad interface release: " << e.what(); 
	}
	// this does not throws
	d_handle = HandlePtr();
	d_mutex.unlock();
}


void Gamepad::Init() {
	typedef std::shared_ptr<struct libusb_config_descriptor> ConfigPtr;
	struct libusb_config_descriptor * _config(NULL);

	lusb_call(libusb_get_active_config_descriptor,d_device.get(),&_config);
	ConfigPtr config(_config , &libusb_free_config_descriptor);
	
	//inspects and check interface

	const libusb_interface_descriptor *hid(NULL);
	for(unsigned int i = 0; i < config->bNumInterfaces; ++i) {
		const struct libusb_interface & itf = config->interface[i];
		for(unsigned int a = 0; a < itf.num_altsetting; ++a) {
			const struct libusb_interface_descriptor & iDesc = itf.altsetting[a];
			if ( hid == NULL && 
			     iDesc.bInterfaceClass == LIBUSB_CLASS_HID && 
			     iDesc.bInterfaceSubClass == SUBCLASS_NON_BOOT && 
			     iDesc.bInterfaceProtocol ==  PROTOCOL_NON_BOOT ) {
				hid = &iDesc;
			}
		}
	}

	if( hid == NULL) {
		std::ostringstream os;
		os << "Bad USB Device configuration. Found " << config->bNumInterfaces
		   << " None of them is HID";
		throw std::runtime_error(os.str());
	}

	LOG(INFO) << "Device Initialized accordingly";
}


const std::string VendorInErrorName(const VendorInError_e e) {
	typedef std::map<VendorInError_e,std::string> NameByErrors;
	static NameByErrors errors;
	if(errors.empty()) {
#define LOAD_ERROR(errName) do {  errors[errName] = #errName; }while(0)
		LOAD_ERROR(GSG_ERR_NO_ERROR);		
#undef LOAD_ERROR
		if(errors.size() < GSG_ERROR_MAX) {
			throw std::logic_error("VendorInErrorName() not up to date");
		}
	}

	NameByErrors::const_iterator fi = errors.find(e);
	if(fi == errors.end() ) {
		static std::string unknown("Unknown error");
		return unknown;
	}
	return fi->second;
}

