#include "Gamepad.h"

#include "common.h"
#include <libusb.h> 
#include <iomanip>
#include <cstring>
#include <map>

#include <glog/logging.h>




Gamepad::Gamepad(libusb_device * device) 
	: d_device(device, &libusb_unref_device)
	, d_bulkInEP(0xff)
	, d_bulkOutEP(0xff)
	, d_maxRetries(20){
	

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
#ifdef LIBUSB_DARWIN_WORKAROUND
	AssertOpened();
#endif //LIBUSB_DARWIN_WORKAROUND
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


unsigned int Gamepad::MaxRetries() const { 
	return d_maxRetries; 
}

void Gamepad::SetMaxRetries(unsigned int max) { 
	d_maxRetries = max;
}


void Gamepad::BulkAll(bool in, uint8_t * data, size_t size) const {
	size_t written = 0;
	for (unsigned int i = d_maxRetries ; i > 0 ; --i) {
		int transferred;
		lusb_call(libusb_bulk_transfer,
		          d_handle.get(),
		          (in == true) ? d_bulkInEP : d_bulkOutEP,
		          data + written,
		          size - written,
		          &transferred,
		          0);
		
		if ( written == size ) {
			break;
		}

	}

	if( written < size ) {
		std::ostringstream os;
		os << "Could not transfer " << size 
		   << " from address " << data 
		   << " only " << written <<  " bytes written in " 
		   << d_maxRetries << " trials";
		throw std::runtime_error(os.str());
	}

}


VendorInReport_t Gamepad::SendInstruction(GSGHostInstruction_e i, 
                                           const ListOfParameter & params) {
	if( i <= INST_NONE || i >= INST_MAX) {
		throw std::out_of_range("Unrecognized instruction");
	}

	if( params.size() >= NB_PARAMS) {
		std::ostringstream os;
		os << "Device only accepts a maximum of " << NB_PARAMS << " parameters, but " 
		   << params.size() << " required";
		throw std::out_of_range(os.str());
	}
	
	VendorOutReport_t toSend;
#ifndef NDEBUG
	if (sizeof(toSend) != 26) {
		std::ostringstream os;
		os << "Internal Error, VendorOutSize_t byte size is " << sizeof(VendorOutReport_t)
		   << " but 26 expected";
		throw std::logic_error(os.str());
	}
#endif

	toSend.instructionID = i;
	std::memset(&toSend.params,0,sizeof(toSend.params));
	for(ListOfParameter::const_iterator p = params.begin();
	    p != params.end();
	    ++p ) {
		toSend.params[p - params.begin()].ID = p->ID;
		toSend.params[p - params.begin()].Value = p->Value;
	}

	BulkAll(false,(uint8_t*)&toSend,sizeof(toSend));

	VendorInReport_t toRead;
	
	BulkAll(true,(uint8_t*)&toRead,sizeof(toRead));

	return toRead;

}

const std::string VendorInErrorName(const VendorInError_e e) {
	typedef std::map<VendorInError_e,std::string> NameByErrors;
	static NameByErrors errors;
	if(errors.empty()) {
#define LOAD_ERROR(errName) do {  errors[errName] = #errName; }while(0)
		LOAD_ERROR(VI_ERR_NO_ERROR);		
#undef LOAD_ERROR
		if(errors.size() < VI_ERROR_MAX) {
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

void Gamepad::SetParameter(GSGParam_e id , uint16_t value) {
	ListOfParameter params;
	GSGParameter_t param;
	param.ID = id;
	param.Value = value;
	params.push_back(param);

	VendorInReport_t res = SendInstruction(INST_SET_PARAMS,params);

	std::ostringstream os;
	os << "setting parameter 0x"<< std::hex << (int)id << " to value " << std::dec << value;
	CheckResponse(res,VI_TYPE_PARAM_RETURN,os.str());
	//nothing to do
}

void Gamepad::CheckResponse(const VendorInReport_t & report,
                            VendorInReportType_e expected, 
                            const std::string & context) {
	if(report.type != expected ) {
		std::ostringstream os;
		os << "Received back unexpected packet type 0x"<< std::hex << (int)report.type
		   << " while " << context;
		throw std::runtime_error(os.str());
	}
	
	if(report.error != VI_ERR_NO_ERROR ) {
		std::ostringstream os;
		os << "Got error " << VendorInErrorName((VendorInError_e)report.error) 
		   << " while " << context;
		throw std::runtime_error(os.str());
	}
}

uint16_t Gamepad::GetParameter(GSGParam_e p) {
	ListOfParameterID ids;
	ListOfParameter result;
	
	ids.push_back(p);
	GetParametersPrivate(ids.begin(),ids.end(),result);

	if( result.size() != 1) {
		throw std::logic_error("Got wrong number of result while reading one parameter");
	}
	return result[0].Value;

}


void Gamepad::GetParameters(const ListOfParameterID & ids, 
                            ListOfParameter & result) {
	result.clear();
	ListOfParameterID::const_iterator thisStart = ids.begin();
	
	while(thisStart != ids.end() ) {
		ListOfParameterID::const_iterator thisEnd = thisStart;
		for(; thisEnd - thisStart < NB_PARAMS && thisEnd != ids.end(); ++thisEnd) {
		}

		GetParametersPrivate(thisStart,thisEnd,result);

		thisStart = thisEnd;

	}
}


void Gamepad::GetParametersPrivate(const ListOfParameterID::const_iterator & start,
                                   const ListOfParameterID::const_iterator & end,
                                   ListOfParameter & result) {
	ListOfParameter toSend;
	std::ostringstream context;
	context << " reading parameter(s) {";

	for(ListOfParameterID::const_iterator id = start;
	    id != end;
	    ++id) {
		GSGParameter_t p;
		p.ID = *id;
		p.Value = 0;
		toSend.push_back(p);
		context << std::hex << " 0x" << (int) *id;
	}
	context << " }";

	VendorInReport_t res = SendInstruction(INST_READ_PARAMS,toSend);
	
	CheckResponse(res,VI_TYPE_PARAM_RETURN,context.str());

	for(ListOfParameterID::const_iterator id = start;
	    id != end;
	    ++id) {
		unsigned int i = id - start;
		if ( res.data.params[i].ID != *id ) {
			std::ostringstream os;
			os << "USB Device send back parameter 0x" << std::hex << (int)res.data.params[i].ID 
			   << " instead of 0x" << *id << " while " << context.str();
			throw std::runtime_error(os.str());
		}
		GSGParameter_t p = res.data.params[i];
		result.push_back(p);
	}
	
}
                                   
const Gamepad::LoadCellValues & Gamepad::GetCells() {
	d_cellValues.assign(0,NB_CELLS);

	ListOfParameter  toSend;
	VendorInReport_t res = SendInstruction(INST_FETCH_CELL_VALUES,toSend);
	
	CheckResponse(res,VI_TYPE_CELL_RETURN,"fetching cells values");

	for(unsigned int i = 0; i < NB_CELLS; ++i) {
		d_cellValues[i] = res.data.cells[i];
	}

	return d_cellValues;
}



void Gamepad::SaveParamInEEPROM() {
	VendorInReport_t res = SendInstruction(INST_SAVE_IN_EEPROM,ListOfParameter());
	CheckResponse(res,VI_TYPE_PARAM_RETURN," saving parameter in eeprom");
}
