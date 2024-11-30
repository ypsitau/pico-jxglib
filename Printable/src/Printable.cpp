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
Printable* Printable::pStandardOutput_ = &PrintableDumb::Instance;

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

Printable::DumpStyle& Printable::DumpStyle::operator()(const void* buff, int cnt)
{
	const char* formatAddr;
	char formatData[32];
	const char* formatDataMeta;
	if (upperCaseFlag_) {
		formatAddr = "%0*X ";
		formatDataMeta = " %%0%dX";
	} else {
		formatAddr = "%0*x ";
		formatDataMeta = " %%0%dx";
	}
	::snprintf(formatData, sizeof(formatData), formatDataMeta, bytesPerElem_ * 2);
	const uint8_t* p = reinterpret_cast<const uint8_t*>(buff);
	int iCol = 0;
	int nDigitsAddr = 0;
	if (nDigitsAddr_ > 0) {
		nDigitsAddr = nDigitsAddr_;
	} else {
		uint32_t addrEnd = addrStart_ + ((cnt - 1) / nCols_) * nCols_ * bytesPerElem_;
		for ( ; addrEnd; addrEnd >>= 4, nDigitsAddr++) ;
		if (nDigitsAddr == 0) nDigitsAddr = 1;
	}
	uint32_t addr = addrStart_;
	for (int i = 0; i < cnt; i++, p += bytesPerElem_) {
		if (iCol == 0) printable_.Printf(formatAddr, nDigitsAddr, addr + i * bytesPerElem_);
		
		printable_.Printf(formatData, *p);
		
		iCol++;
		if (iCol == nCols_) {
			printable_.Println();
			iCol = 0;
		}
	}
	if (iCol > 0) printable_.Println();
	return *this;
}

//------------------------------------------------------------------------------
// PrintableDumb
//------------------------------------------------------------------------------
PrintableDumb PrintableDumb::Instance;

//------------------------------------------------------------------------------
// Functions that expose methods provided by Printable class.
//------------------------------------------------------------------------------
Printable& Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Printable::GetStandardOutput().VPrintf(format, args);
	va_end(args);
	return Printable::GetStandardOutput();
}

}
