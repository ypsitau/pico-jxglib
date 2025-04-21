//==============================================================================
// Printable.cpp
//==============================================================================
#include <stdio.h>
#include <stdarg.h>
#include "jxglib/Printable.h"
#include "jxglib/Stdio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Printable
//------------------------------------------------------------------------------
Printable* Printable::pStandardOutput_ = &Stdio::Instance;

Printable& Printable::Print(const char* str)
{
	for (const char* p = str; *p; p++) PutChar(*p);
	return *this;
}

Printable& Printable::PrintRaw(const char* str)
{
	for (const char* p = str; *p; p++) PutCharRaw(*p);
	return *this;
}

Printable& Printable::VPrintf(const char* format, va_list args)
{
	char buff[256];
	::vsnprintf(buff, sizeof(buff), format, args);
	Print(buff);
	return *this;
}

Printable& Printable::VPrintfRaw(const char* format, va_list args)
{
	char buff[256];
	::vsnprintf(buff, sizeof(buff), format, args);
	PrintRaw(buff);
	return *this;
}

Printable& Printable::Println(const char* str)
{
	Print(str);
	Print("\n");
	return *this;
}

Printable& Printable::PrintlnRaw(const char* str)
{
	PrintRaw(str);
	PrintRaw("\n");
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

Printable& Printable::PrintfRaw(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	VPrintfRaw(format, args);
	va_end(args);
	return *this;
}

//------------------------------------------------------------------------------
// Printable::DumpT
//------------------------------------------------------------------------------
Printable::DumpT::DumpT(Printable* pPrintable) : pPrintable_(pPrintable),
	upperCaseFlag_{true}, nDigitsAddr_{-1}, nCols_{16}, addrStart_{0}, bytesPerElem_{1},
	bigEndianFlag_{false}, asciiFlag_{false}
{}

Printable::DumpT& Printable::DumpT::operator()(const void* buff, int cnt)
{
	Printable& printable = pPrintable_? *pPrintable_ : *pStandardOutput_;
	char formatAddr[8];
	char formatData[16];
	char asciiBuff[64];
	int iCol = 0;
	int nDigitsAddr = nDigitsAddr_;
	if (nDigitsAddr < 0) {
		nDigitsAddr = 0;
		uint32_t addrEnd = addrStart_ + ((cnt - 1) / nCols_) * nCols_ * bytesPerElem_;
		for ( ; addrEnd; addrEnd >>= 4, nDigitsAddr++) ;
		if (nDigitsAddr == 0) nDigitsAddr = 1;
	}
	::snprintf(formatAddr, sizeof(formatAddr), "%%0%d%c  ", nDigitsAddr, upperCaseFlag_? 'X' : 'x');
	::snprintf(formatData, sizeof(formatData), "%%0%d%s%c", bytesPerElem_ * 2,
		(bytesPerElem_ < 4)? "" : (bytesPerElem_ < 8)? "l" : "ll", upperCaseFlag_? 'X' : 'x');
	uint32_t addr = addrStart_;
	char* pAsciiBuff;
	const uint8_t* pElem = reinterpret_cast<const uint8_t*>(buff);
	for (int i = 0; i < cnt; i++, pElem += bytesPerElem_) {
		if (iCol == 0) {
			if (nDigitsAddr > 0) {
				printable.Printf(formatAddr, addr + i * bytesPerElem_);
			}
			pAsciiBuff = asciiBuff;
		} else {
			printable.Print(" ");
		}
		if (bytesPerElem_ == 1) {	
			printable.Printf(formatData, *pElem);
		} else if (bytesPerElem_ == 2) {
			uint16_t num = bigEndianFlag_?
				((static_cast<uint16_t>(pElem[0]) << 8) + static_cast<uint16_t>(pElem[1])) :
				((static_cast<uint16_t>(pElem[1]) << 8) + static_cast<uint16_t>(pElem[0]));
			printable.Printf(formatData, num);
		} else if (bytesPerElem_ == 4) {
			uint32_t num = bigEndianFlag_?
				((static_cast<uint32_t>(pElem[0]) << 24) + (static_cast<uint32_t>(pElem[1]) << 16) +
				(static_cast<uint32_t>(pElem[2]) << 8) + static_cast<uint32_t>(pElem[3])) :
				((static_cast<uint32_t>(pElem[3]) << 24) + (static_cast<uint32_t>(pElem[2]) << 16) +
				(static_cast<uint32_t>(pElem[1]) << 8) + static_cast<uint32_t>(pElem[0]));
			printable.Printf(formatData, num);
		} else if (bytesPerElem_ == 8) {
			uint64_t num = bigEndianFlag_?
				((static_cast<uint64_t>(pElem[0]) << 56) + (static_cast<uint64_t>(pElem[1]) << 48) +
				(static_cast<uint64_t>(pElem[2]) << 40) + (static_cast<uint64_t>(pElem[3]) << 32) +
				(static_cast<uint64_t>(pElem[4]) << 24) + (static_cast<uint64_t>(pElem[5]) << 16) +
				(static_cast<uint64_t>(pElem[6]) << 8) + static_cast<uint64_t>(pElem[7])) :
				(static_cast<uint64_t>(pElem[7]) << 56) + (static_cast<uint64_t>(pElem[6]) << 48) +
				((static_cast<uint64_t>(pElem[5]) << 40) + (static_cast<uint64_t>(pElem[4]) << 32) +
				(static_cast<uint64_t>(pElem[3]) << 24) + (static_cast<uint64_t>(pElem[2]) << 16) +
				(static_cast<uint64_t>(pElem[1]) << 8) + static_cast<uint64_t>(pElem[0]));
			printable.Printf(formatData, num);
		}
		for (int i = 0; i < bytesPerElem_; i++) *pAsciiBuff++ = pElem[i];
		iCol++;
		if (iCol == nCols_) {
			if (asciiFlag_) PrintAscii(printable, asciiBuff, bytesPerElem_ * iCol);
			printable.Println();
			iCol = 0;
		}
	}
	if (iCol > 0) {
		if (asciiFlag_) {
			for ( ; iCol < nCols_; iCol++) {
				printable.Print((iCol % 8 == 0)? "  " : " ");
				for (int i = 0; i < bytesPerElem_ * 2; i++) printable.PutChar(' ');
			}
			PrintAscii(printable, asciiBuff, bytesPerElem_ * iCol);
		}
		printable.Println();
	}
	return *this;
}

void Printable::DumpT::PrintAscii(Printable& printable, const char* asciiBuff, int bytes)
{
	printable.Print("  ");
	for (int i = 0; i < bytes; i++) {
		char ch = asciiBuff[i];
		printable.PutChar((0x20 <= ch && ch < 0x7f)? ch : '.');
	}
}

//------------------------------------------------------------------------------
// PrintableDumb
//------------------------------------------------------------------------------
PrintableDumb PrintableDumb::Instance;

//------------------------------------------------------------------------------
// Functions that expose methods provided by Printable class.
//------------------------------------------------------------------------------
Printable::DumpT Dump(nullptr);

Printable& Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Printable::GetStandardOutput().VPrintf(format, args);
	va_end(args);
	return Printable::GetStandardOutput();
}

Printable& PrintfRaw(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Printable::GetStandardOutput().VPrintfRaw(format, args);
	va_end(args);
	return Printable::GetStandardOutput();
}

}
