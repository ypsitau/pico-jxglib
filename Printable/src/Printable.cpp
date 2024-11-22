//==============================================================================
// Printable.cpp
//==============================================================================
#include <stdio.h>
#include <stdarg.h>
#include "jxglib/Printable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Printable
//------------------------------------------------------------------------------
void Printable::vprintf(const char* format, va_list args)
{
	char buff[256];
	::vsnprintf(buff, sizeof(buff), format, args);
	puts(buff);
}

void Printable::printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

}
