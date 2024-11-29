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

Printable& Printable::Dump(const void* buff, int cnt)
{
	const char* formatAddr;
	char formatData[32];
	const char* formatDataMeta;
	if (dumpStyle.upperCaseFlag) {
		formatAddr = "%0*X ";
		formatDataMeta = " %%0%dX";
	} else {
		formatAddr = "%0*x ";
		formatDataMeta = " %%0%dx";
	}
	::snprintf(formatData, sizeof(formatData), formatDataMeta, dumpStyle.bytesPerElem * 2);
	const uint8_t* p = reinterpret_cast<const uint8_t*>(buff);
	int iCol = 0;
	int nDigitsAddr = 0;
	if (dumpStyle.nDigitsAddr > 0) {
		nDigitsAddr = dumpStyle.nDigitsAddr;
	} else {
		uint32_t addrEnd = dumpStyle.addrStart + ((cnt - 1) / dumpStyle.nCols) * dumpStyle.nCols * dumpStyle.bytesPerElem;
		for ( ; addrEnd; addrEnd >>= 4, nDigitsAddr++) ;
		if (nDigitsAddr == 0) nDigitsAddr = 1;
	}
	uint32_t addr = dumpStyle.addrStart;
	for (int i = 0; i < cnt; i++, p += dumpStyle.bytesPerElem) {
		if (iCol == 0) Printf(formatAddr, nDigitsAddr, addr + i * dumpStyle.bytesPerElem);
		
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
