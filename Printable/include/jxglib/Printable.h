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
	class DumpStyle {
	public:
		bool upperCaseFlag;
		int nDigitsAddr;
		int nCols;
		uint32_t addrStart;
		int bytesPerElem;
		bool bigEndianFlag;
	public:
		DumpStyle() : upperCaseFlag{true}, nDigitsAddr{0}, nCols{8}, addrStart{0}, bytesPerElem{1} {}
	public:
		DumpStyle& UpperCase() { upperCaseFlag = true; return *this; }
		DumpStyle& LowerCase() { upperCaseFlag = false; return *this; }
		DumpStyle& DigitsAddr(int nDigitsAddr) { this->nDigitsAddr = nDigitsAddr; return *this; }
		DumpStyle& DigitsAddr_Auto() { this->nDigitsAddr = 0; return *this; }
		DumpStyle& Cols(int nCols) { this->nCols = nCols; return *this; }
		DumpStyle& AddrStart(uint32_t addrStart) { this->addrStart = addrStart; return *this; }
		DumpStyle& Data8Bit() { bytesPerElem = 1; bigEndianFlag = false; return *this; } 
		DumpStyle& Data16Bit() { bytesPerElem = 2; bigEndianFlag = false; return *this; } 
		DumpStyle& Data32Bit() { bytesPerElem = 4; bigEndianFlag = false; return *this; } 
		DumpStyle& Data64Bit() { bytesPerElem = 8; bigEndianFlag = false; return *this; } 
		DumpStyle& Data16BitBE() { bytesPerElem = 2; bigEndianFlag = true; return *this; } 
		DumpStyle& Data32BitBE() { bytesPerElem = 4; bigEndianFlag = true; return *this; } 
		DumpStyle& Data64BitBE() { bytesPerElem = 8; bigEndianFlag = true; return *this; } 
	};
public:
	DumpStyle dumpStyle;
public:
	virtual Printable& ClearScreen() = 0;
	virtual Printable& FlushScreen() = 0;
	virtual Printable& Locate(int col, int row) = 0;
	virtual Printable& Print(const char* str) = 0;
	virtual Printable& Println(const char* str = "");
	virtual Printable& VPrintf(const char* format, va_list args);
	Printable& Printf(const char* format, ...);
	Printable& Dump(const void* buff, int bytes);
};

}

#endif
