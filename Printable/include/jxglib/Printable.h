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
	virtual void flush() = 0;
	virtual void puts(const char* str) = 0;
	virtual void vprintf(const char* format, va_list args);
	void printf(const char* format, ...);
	void dump(const void* buff, int bytes);
};

}

#endif
