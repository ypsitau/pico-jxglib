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

//------------------------------------------------------------------------------
// Printable::DumpStyle
//------------------------------------------------------------------------------
Printable::DumpStyle::DumpStyle(Printable* pPrintable) : pPrintable_(pPrintable),
	upperCaseFlag_{true}, nDigitsAddr_{0}, nCols_{16}, addrStart_{0}, bytesPerElem_{1}
{}

Printable::DumpStyle& Printable::DumpStyle::operator()(const void* buff, int cnt)
{
	Printable& printable = pPrintable_? *pPrintable_ : *pStandardOutput_;
	char formatAddr[32];
	char formatData[32];
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
	::snprintf(formatAddr, sizeof(formatAddr), "%%0%d%c ", nDigitsAddr, upperCaseFlag_? 'X' : 'x');
	::snprintf(formatData, sizeof(formatData), " %%0%d%s%c", bytesPerElem_ * 2,
		(bytesPerElem_ < 4)? "" : (bytesPerElem_ < 8)? "l" : "ll", upperCaseFlag_? 'X' : 'x');
	//printable.Printf("%s  %s\n", formatAddr, formatData);
	uint32_t addr = addrStart_;
	for (int i = 0; i < cnt; i++, p += bytesPerElem_) {
		if (iCol == 0) printable.Printf(formatAddr, addr + i * bytesPerElem_);
		if (bytesPerElem_ == 1) {	
			printable.Printf(formatData, *p);
		} else if (bytesPerElem_ == 2) {
			uint16_t num = bigEndianFlag_?
				((static_cast<uint16_t>(p[0]) << 8) + static_cast<uint16_t>(p[1])) :
				((static_cast<uint16_t>(p[1]) << 8) + static_cast<uint16_t>(p[0]));
			printable.Printf(formatData, num);
		} else if (bytesPerElem_ == 4) {
			uint32_t num = bigEndianFlag_?
				((static_cast<uint32_t>(p[0]) << 24) + (static_cast<uint32_t>(p[1]) << 16) +
				(static_cast<uint32_t>(p[2]) << 8) + static_cast<uint32_t>(p[3])) :
				((static_cast<uint32_t>(p[3]) << 24) + (static_cast<uint32_t>(p[2]) << 16) +
				(static_cast<uint32_t>(p[1]) << 8) + static_cast<uint32_t>(p[0]));
			printable.Printf(formatData, num);
		} else if (bytesPerElem_ == 8) {
			uint64_t num = bigEndianFlag_?
				((static_cast<uint64_t>(p[0]) << 56) + (static_cast<uint64_t>(p[1]) << 48) +
				(static_cast<uint64_t>(p[2]) << 40) + (static_cast<uint64_t>(p[3]) << 32) +
				(static_cast<uint64_t>(p[4]) << 24) + (static_cast<uint64_t>(p[5]) << 16) +
				(static_cast<uint64_t>(p[6]) << 8) + static_cast<uint64_t>(p[7])) :
				(static_cast<uint64_t>(p[7]) << 56) + (static_cast<uint64_t>(p[6]) << 48) +
				((static_cast<uint64_t>(p[5]) << 40) + (static_cast<uint64_t>(p[4]) << 32) +
				(static_cast<uint64_t>(p[3]) << 24) + (static_cast<uint64_t>(p[2]) << 16) +
				(static_cast<uint64_t>(p[1]) << 8) + static_cast<uint64_t>(p[0]));
			printable.Printf(formatData, num);
		}
		iCol++;
		if (iCol == nCols_) {
			printable.Println();
			iCol = 0;
		}
	}
	if (iCol > 0) printable.Println();
	return *this;
}

//------------------------------------------------------------------------------
// PrintableDumb
//------------------------------------------------------------------------------
PrintableDumb PrintableDumb::Instance;

//------------------------------------------------------------------------------
// Functions that expose methods provided by Printable class.
//------------------------------------------------------------------------------
Printable::DumpStyle Dump(nullptr);

Printable& Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Printable::GetStandardOutput().VPrintf(format, args);
	va_end(args);
	return Printable::GetStandardOutput();
}

}
