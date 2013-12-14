#pragma once

#include <stdexcept>
#include <errno.h>

#define NOT_YET_IMPLEMENTED(className,method) do {  \
	throw std::runtime_error(#className "::" #method "() is not yet implemented"); \
}while(0)


class SystemError : public std::runtime_error {
public :
	SystemError(int error);
	SystemError(const std::string & reason, int error);
	virtual ~SystemError() throw();
	
	int Error() const;
private :
	std::string StringifyError(int error);
	
	int d_error;
};


#define syscall_throw(fnct,errorValue) do { \
	throw os::SystemError( #fnct + std::string("() returned with error"),error); \
}while(0)


#define p_call(fnct,...) do { \
	if ( fnct(__VA_ARGS__) < 0 ) { \
		syscall_throw(fnct,errno); \
	} \
}while(0)

#define p_call_no_errno(fnct,...) do { \
	int res_ ## fnct = fnct(__VA_ARGS__); \
	if (res_ ## fnct != 0 ) { \
		syscall_throw(fnct, res_ ## fnct); \
	} \
}while(0)
