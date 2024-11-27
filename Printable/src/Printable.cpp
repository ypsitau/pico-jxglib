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
Printable& Printable::VPrintf(const char* format, va_list args)
{
	char buff[256];
	::vsnprintf(buff, sizeof(buff), format, args);
	Print(buff);
	return *this;
}

Printable& Printable::Println(const char* str)
{
	Print(str);
	Print("\n");
	return *this;
}

Printable& Printable::Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	VPrintf(format, args);
	va_end(args);
	return *this;
}

Printable& Printable::Dump(const void* buff, int bytes)
{
	const char* formatAddr;
	const char* formatData;
	if (dumpStyle.upperCaseFlag) {
		formatAddr = "%0*X ";
		formatData = " %02X";
	} else {
		formatAddr = "%0*x ";
		formatData = " %02x";
	}
	const uint8_t* p = reinterpret_cast<const uint8_t*>(buff);
	int iCol = 0;
	for (int i = 0; i < bytes; i++, p++) {
		if (iCol == 0) Printf(formatAddr, dumpStyle.nDigitsAddr, i);
		Printf(formatData, *p);
		iCol++;
		if (iCol == dumpStyle.nCols) {
			Println();
			iCol = 0;
		}
	}
	if (iCol > 0) Println();
	return *this;
}

}
