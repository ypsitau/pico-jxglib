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

void Printable::dump(const void* buff, int bytes)
{
	const char* formatAddr;
	const char* formatData;
	if (dumpAttr.upperCaseFlag) {
		formatAddr = "%0*X ";
		formatData = " %02X";
	} else {
		formatAddr = "%0*x ";
		formatData = " %02x";
	}
	const uint8_t* p = reinterpret_cast<const uint8_t*>(buff);
	int iCol = 0;
	for (int i = 0; i < bytes; i++, p++) {
		if (iCol == 0) printf(formatAddr, dumpAttr.nDigitsAddr, i);
		printf(formatData, *p);
		iCol++;
		if (iCol == dumpAttr.nCols) {
			puts("\n");
			iCol = 0;
		}
	}
	if (iCol > 0) puts("\n");
}

}
