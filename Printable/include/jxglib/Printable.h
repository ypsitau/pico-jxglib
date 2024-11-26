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
	class DumpAttr {
	public:
		bool upperCaseFlag;
		int nDigitsAddr;
		int nCols;
	public:
		DumpAttr() : upperCaseFlag{true}, nDigitsAddr{4}, nCols{8} {}
	public:
		DumpAttr& UpperCase() { upperCaseFlag = true; return *this; }
		DumpAttr& LowerCase() { upperCaseFlag = false; return *this; }
		DumpAttr& DigitsAddr(int nDigitsAddr) { this->nDigitsAddr = nDigitsAddr; return *this; }
		DumpAttr& Cols(int nCols) { this->nCols = nCols; return *this; }
	};
public:
	DumpAttr dumpAttr;
public:
	virtual Printable& Clear() = 0;
	virtual Printable& Flush() = 0;
	virtual Printable& Locate(int col, int row) = 0;
	virtual Printable& Print(const char* str) = 0;
	virtual Printable& Println(const char* str = "");
	virtual Printable& VPrintf(const char* format, va_list args);
	Printable& Printf(const char* format, ...);
	Printable& Dump(const void* buff, int bytes);
};

}

#endif
