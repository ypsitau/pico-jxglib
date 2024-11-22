//==============================================================================
// jxglib/Printable.h
//==============================================================================
#ifndef PICO_JXGLIB_PRINTABLE_H
#define PICO_JXGLIB_PRINTABLE_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Printable
//------------------------------------------------------------------------------
class Printable {
public:
	virtual void flush() = 0;
	virtual void puts(const char* str) = 0;
	virtual void vprintf(const char* format, va_list args);
	void printf(const char* format, ...);
};

}

#endif
