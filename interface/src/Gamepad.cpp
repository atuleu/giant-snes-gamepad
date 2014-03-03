#include "Gamepad.h"

#include "common.h"
#include <libusb.h> 
#include <iomanip>
#include <cstring>
#include <map>

#include <glog/logging.h>


std::mutex Gamepad::s_mutex;
Gamepad::DeviceByIdentifier Gamepad::s_devices;

Gamepad::Gamepad(libusb_device * device) 
	: d_device(device, &libusb_unref_device) {
	
	//need to open to avoid segfault of libsusb, see
	//http://www.libusb.org/ticket/171
	libusb_device_handle * handle;
	try { 
		d_mutex.lock();
		lusb_call(libusb_open,d_device.get(),&handle);
		unsigned char buffer[1000];
		libusb_device_descriptor desc;
		lusb_call(libusb_get_device_descriptor,device,&desc);
		if(desc.iSerialNumber == 0 ) {
			throw std::runtime_error("Device has no serial number");
		}
		lusb_call(libusb_get_string_descriptor_ascii,handle,desc.iSerialNumber,buffer,1000);
		d_serialNumber = (char*)buffer;
		
		Init();

		libusb_close(handle);
		d_mutex.unlock();	

	} catch(...) {
		libusb_close(handle);
		d_mutex.unlock();
		throw;
	}



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
	    << "}";
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

		uint8_t portNumber = libusb_get_port_number(device);
		uint8_t busNumber = libusb_get_bus_number(device);
		uint8_t devNumber = libusb_get_device_address(device);

		DLOG(INFO) << "Found USB Device " << desc << " at "
		           << (int)portNumber << "::" 
		           << (int)busNumber << "::"
		           << (int)devNumber;
		
		if( desc.idVendor  != ID_VENDOR  ||
		    desc.idProduct != ID_PRODUCT ||
		    desc.bcdDevice != BCD_VERSION   ) {
			libusb_unref_device(device);
			continue;
		}

		Identifier id(devNumber + (busNumber << 8) + (portNumber << 16) );
		//Save locally 
		s_mutex.lock();
		DeviceByIdentifier::const_iterator fi = s_devices.find(id);
		if (fi == s_devices.end()) {
			try { 
				LOG(INFO) << "Found new device at "<< (int)portNumber << ":" << (int)busNumber << ":" << (int)devNumber;
				s_devices[id] = Ptr(new Gamepad(device));
			} catch (const std::exception & e) {
				LOG(WARNING) << "USB device has right (vid,pid,bcd), but cannot create wrapper : " 
				             << e.what();
				s_mutex.unlock();
				continue;
			}
			res.push_back(s_devices[id]);

		} else {
			res.push_back(fi->second);
		}
		s_mutex.unlock();
		
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
	ReadAllParams();
}

void Gamepad::Close() {
	d_mutex.lock();
	//	try {
	//		lusb_call(libusb_release_interface,d_handle.get(),d_vendorInterface);
	// } catch( const std::exception & e) {
	//	LOG(ERROR) << "Got error on Gamepad interface release: " << e.what(); 
	// }
	// this does not throws
	d_handle = HandlePtr();
	d_mutex.unlock();
}



void Gamepad::ReadAllParams() {
	d_mutex.lock();
	d_parameters.assign(GSG_NUM_PARAMS,0x6942);
	try {
		lusb_call(libusb_control_transfer,
		          d_handle.get(),
		          REQ_VENDOR_IN,
		          INST_READ_ALL_PARAMS,
		          0,
		          0,
		          (unsigned char *)&d_parameters[0],
		          2 * GSG_NUM_PARAMS,
		          0);
	} catch(...) {
		d_mutex.unlock();
		throw;
	}
	d_mutex.unlock();
}


uint16_t Gamepad::GetParam(GSGParam_e id) {
	if ( id >= GSG_NUM_PARAMS || id < 0) {
		std::ostringstream os;
		os << "Id: " << (int)id << " is out of range [ 0 ," << GSG_NUM_PARAMS << "[";
		throw std::out_of_range(os.str());
	}

	uint16_t res;
	d_mutex.lock();
	res = d_parameters[id];
	d_mutex.unlock();
	return res;
}


void Gamepad::SetParam(GSGParam_e id, uint16_t value) {
	if ( id >= GSG_NUM_PARAMS || id < 0) {
		std::ostringstream os;
		os << "Id: " << (int)id << " is out of range [ 0 ," << GSG_NUM_PARAMS << "[";
		throw std::out_of_range(os.str());
	}
	d_mutex.lock();
	DLOG(INFO) << "Try to set param" << id << " to " << value;
	try {
		lusb_call(libusb_control_transfer,
		          d_handle.get(),
		          REQ_VENDOR_OUT,
		          INST_SET_PARAM,
		          value, // WTF they inversed it here o_O
		          id,    // WTF they inversed it here o_O
		          NULL,
		          0,
		          0);
		d_parameters[id] = value;
	} catch ( const LibUsbError & e) {
		DLOG(ERROR) << "Could not set param "<< id << " to "<< value 
		            << " : " << e.what() << " | " << libusb_strerror((libusb_error)e.Error());

		d_mutex.unlock();
		throw;
	} catch (...) {
		d_mutex.unlock();
		throw;
	}
	d_mutex.unlock();
}

void Gamepad::FetchLoadCellValues(LoadCellValues & cells) {
	cells.clear();
	uint8_t data[3 * NUM_BUTTONS];
	d_mutex.lock();
	try {
		lusb_call(libusb_control_transfer,
		          d_handle.get(),
		          REQ_VENDOR_IN,
		          INST_FETCH_CELL_VALUES,
		          0,
		          0,
		          data,
		          3 * 12,
		          0);
	} catch(...) {
		d_mutex.unlock();
		throw;
	}
	d_mutex.unlock();

	for(size_t i = 0; i < NUM_BUTTONS; ++i) {
		uint16_t value = data[2 * i + 0 ] + (data[2 *i + 1] << 8 );
		uint8_t count = data[2 * NUM_BUTTONS + i]; 
		cells.push_back(std::make_pair(value,count));
	}

}


void Gamepad::SaveParamInEEPROM() {
	d_mutex.lock();
	try {
		lusb_call(libusb_control_transfer,
		          d_handle.get(),
		          REQ_VENDOR_OUT,
		          INST_SAVE_IN_EEPROM,
		          0,
		          0,
		          NULL,
		          0,
		          0);
	} catch (...) {
		d_mutex.unlock();
		throw;
	}

	d_mutex.unlock();
}

const std::string & Gamepad::SerialNumber() const {
	return d_serialNumber;
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

	DLOG(INFO) << "Device Initialized accordingly";
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

