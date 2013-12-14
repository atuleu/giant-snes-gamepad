#include "common.h"

#include <sstream>

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
