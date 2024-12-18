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
		static const int bytesPerLineInit = 16;
	private:
		Printable* pPrintable_;	// maybe nullptr
		bool upperCaseFlag_;
		int nDigitsAddr_;
		int nCols_;
		uint32_t addrStart_;
		int bytesPerElem_;
		bool bigEndianFlag_;
	public:
		DumpStyle(Printable* pPrintable);
	public:
		DumpStyle& UpperCase() { upperCaseFlag_ = true; return *this; }
		DumpStyle& LowerCase() { upperCaseFlag_ = false; return *this; }
		DumpStyle& DigitsAddr(int nDigitsAddr) { nDigitsAddr_ = nDigitsAddr; return *this; }
		DumpStyle& DigitsAddr_Auto() { nDigitsAddr_ = 0; return *this; }
		DumpStyle& Cols(int nCols) { nCols_ = nCols; return *this; }
		DumpStyle& AddrStart(uint32_t addrStart) { addrStart_ = addrStart; return *this; }
		DumpStyle& Data8Bit(int nCols = 16) { bytesPerElem_ = 1; nCols_ = nCols; bigEndianFlag_ = false; return *this; } 
		DumpStyle& Data16Bit(int nCols = 8) { bytesPerElem_ = 2; nCols_ = nCols; bigEndianFlag_ = false; return *this; } 
		DumpStyle& Data32Bit(int nCols = 4) { bytesPerElem_ = 4; nCols_ = nCols; bigEndianFlag_ = false; return *this; } 
		DumpStyle& Data64Bit(int nCols = 2) { bytesPerElem_ = 8; nCols_ = nCols; bigEndianFlag_ = false; return *this; } 
		DumpStyle& Data16BitBE(int nCols = 8) { bytesPerElem_ = 2; nCols_ = nCols; bigEndianFlag_ = true; return *this; } 
		DumpStyle& Data32BitBE(int nCols = 4) { bytesPerElem_ = 4; nCols_ = nCols; bigEndianFlag_ = true; return *this; } 
		DumpStyle& Data64BitBE(int nCols = 2) { bytesPerElem_ = 8; nCols_ = nCols; bigEndianFlag_ = true; return *this; } 
	public:
		DumpStyle& operator()(const void* buff, int bytes);
	};
public:
	DumpStyle Dump;
private:
	static Printable* pStandardOutput_;
public:
	Printable() : Dump(this) {}
public:
	virtual Printable& ClearScreen() = 0;
	virtual Printable& FlushScreen() = 0;
	virtual Printable& Locate(int col, int row) = 0;
	virtual Printable& PutChar(char ch) = 0;
	virtual Printable& PutCharRaw(char ch) { return PutChar(ch); }
	virtual Printable& Print(const char* str);
	virtual Printable& PrintRaw(const char* str);
	virtual Printable& Println(const char* str = "");
	virtual Printable& PrintlnRaw(const char* str = "");
	virtual Printable& VPrintf(const char* format, va_list args);
	virtual Printable& VPrintfRaw(const char* format, va_list args);
	Printable& Printf(const char* format, ...);
	Printable& PrintfRaw(const char* format, ...);
public:
	static void SetStandardOutput(Printable& printable) { pStandardOutput_ = &printable; }
	static Printable& GetStandardOutput() { return *pStandardOutput_; }
};

//------------------------------------------------------------------------------
// PrintableDumb
//------------------------------------------------------------------------------
class PrintableDumb : public Printable {
public:
	static PrintableDumb Instance;
public:
	virtual Printable& ClearScreen() override { return *this; }
	virtual Printable& FlushScreen() override { return *this; }
	virtual Printable& Locate(int col, int row) override { return *this; }
	virtual Printable& PutChar(char ch) override { return *this; }
};

//------------------------------------------------------------------------------
// Functions that expose methods provided by Printable class.
//------------------------------------------------------------------------------
extern Printable::DumpStyle Dump;

inline Printable& ClearScreen() { return Printable::GetStandardOutput().ClearScreen(); }
inline Printable& FlushScreen() { return Printable::GetStandardOutput().FlushScreen(); }
inline Printable& Locate(int col, int row) { return Printable::GetStandardOutput().Locate(col, row); }
inline Printable& PutChar(char ch) { return Printable::GetStandardOutput().PutChar(ch); }
inline Printable& PutCharRaw(char ch) { return Printable::GetStandardOutput().PutCharRaw(ch); }
inline Printable& Print(const char* str) { return Printable::GetStandardOutput().Print(str); }
inline Printable& PrintRaw(const char* str) { return Printable::GetStandardOutput().PrintRaw(str); }
inline Printable& Println(const char* str = "") { return Printable::GetStandardOutput().Println(str); }
inline Printable& PrintlnRaw(const char* str = "") { return Printable::GetStandardOutput().PrintlnRaw(str); }
inline Printable& VPrintf(const char* format, va_list args) { return Printable::GetStandardOutput().VPrintf(format, args); }
inline Printable& VPrintfRaw(const char* format, va_list args) { return Printable::GetStandardOutput().VPrintfRaw(format, args); }
Printable& Printf(const char* format, ...);
Printable& PrintfRaw(const char* format, ...);

}

#endif
