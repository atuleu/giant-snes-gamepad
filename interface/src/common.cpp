#include "common.h"

#include <sstream>

#include <libusb.h> 

std::string SystemError::StringifyError(int error) {
	std::ostringstream os;
	os << "(" << error << ": " << strerror(error) << ")";
	return os.str();
}

SystemError::SystemError(int error)
	: std::runtime_error("System error " + StringifyError(error) + ".")
	, d_error(error){
}

SystemError::SystemError(const std::string& reason , int error)
	: std::runtime_error(reason + " " + StringifyError(error) + ".")
	, d_error(error){
}

SystemError::~SystemError() throw(){
}

int SystemError::Error() const{
	return d_error;
}


LibUsbError::LibUsbError(int error)
	: std::runtime_error(std::string("libusb error: ") + libusb_error_name(error))
	, d_error(error) {
}

LibUsbError::LibUsbError(const std::string & reason, int error) 
	: std::runtime_error(reason + ": " + libusb_error_name(error))
	, d_error(error) {
}


LibUsbError::~LibUsbError() throw() {
}


int LibUsbError::Error() const {
	return d_error;
}
