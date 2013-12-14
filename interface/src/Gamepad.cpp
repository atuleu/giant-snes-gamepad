#include "Gamepad.h"

#include "common.h"
#include <libusb.h> 
#include <iomanip>

#include <glog/logging.h>

#include "../../common/communication.h"


Gamepad::Gamepad(libusb_device * device) 
	: d_device(device, &libusb_unref_device)
	, d_bulkInEP(0xff)
	, d_bulkOutEP(0xff){
	

#ifdef LIBUSB_DARWIN_WORKAROUND
	//need to open to avoid segfault of libsusb, see
	//http://www.libusb.org/ticket/171
	Open();
#endif
	Init();
#ifdef LIBUSB_DARWIN_WORKAROUND
	// uneeded, but would like not to forget Open() call for
	// cross-platform development
	Close();
#endif
}


Gamepad::~Gamepad() {
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
	if(d_handle) {
		return;
	}
	libusb_device_handle * handle;
	lusb_call(libusb_open,d_device.get(),&handle);
	d_handle = HandlePtr(handle,&libusb_close);
}

void Gamepad::Close() {
	d_handle = HandlePtr();
}


void Gamepad::Init() {
	typedef std::shared_ptr<struct libusb_config_descriptor> ConfigPtr;
	struct libusb_config_descriptor * _config(NULL);

	lusb_call(libusb_get_active_config_descriptor,d_device.get(),&_config);
	ConfigPtr config(_config , &libusb_free_config_descriptor);
	
	//inspects and check interface

	const libusb_interface_descriptor *hid(NULL),*vendor(NULL);
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

			if ( vendor == NULL && 
			     iDesc.bInterfaceClass == CLASS_VENDOR_SPECIFIC &&
			     iDesc.bInterfaceSubClass == SUBCLASS_VENDOR_SPECIFIC &&
			     iDesc.bInterfaceProtocol == PROTOCOL_VENDOR_SPECIFIC ) {
				vendor = &iDesc;
			}
		}
	}

	if(vendor == NULL || hid == NULL) {
		std::ostringstream os;
		os << "Bad USB Device configuration. Found " << config->bNumInterfaces
		   << " interfaces; ";
		if(hid) { 
			os << "One of them (" << (int)hid->bInterfaceNumber << ") is a HID device, but no Vendor specific interface found";
		} else if(vendor) {
			os << "One of them (" << (int)vendor->bInterfaceNumber << ") is a Vendor specific interface, but no HID found";
		} else {
			os << "No vendor specific or HID interface found";
		}
		throw std::runtime_error(os.str());
	}

	// check for Vendor interface endpoint
	const struct libusb_endpoint_descriptor *in(NULL),*out(NULL);
	for(unsigned int i = 0 ; i < vendor->bNumEndpoints; ++i) {
		uint8_t address = vendor->endpoint[i].bEndpointAddress;
		//check if endpoint is an out endpoint
		if(out == NULL && ! ( LIBUSB_ENDPOINT_IN & address) ) {
			out = &(vendor->endpoint[i]);
			continue;
		}

		if(in == NULL && (LIBUSB_ENDPOINT_IN & address) ) {
			in = &(vendor->endpoint[i]);
		}
	}

	if (in == NULL || out == NULL) {
		std::ostringstream os;
		os << "Bad USB Device configuration. Found " << vendor->bNumEndpoints 
		   << " EP for Vendor interface;";
		if(out) {
			os << "One of them is OUT, but no IN found";
		} else if (in) {
			os << "One of them is IN, but no OUT found";
		} else {
			os << "Neither IN or OUT EP found";
		}
		throw std::runtime_error(os.str());
	}
	
	d_bulkOutEP = out->bEndpointAddress;
	d_bulkInEP = in->bEndpointAddress;

}

