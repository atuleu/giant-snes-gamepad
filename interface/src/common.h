#pragma once

#include <stdexcept>

#define NOT_YET_IMPLEMENTED(className,method) do {  \
	throw std::runtime_error(#className "::" #method "() is not yet implemented"); \
}while(0)
