//==============================================================================
// jxglib/Printable.h
//==============================================================================
#ifndef PICO_JXGLIB_PRINTABLE_H
#define PICO_JXGLIB_PRINTABLE_H
#include <string.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Printable
//------------------------------------------------------------------------------
class Printable {
public:
	class DumpT {
	public:
		static const int bytesPerLineInit = 16;
	private:
		Printable* pPrintable_;	// maybe nullptr
		bool upperCaseFlag_;
		int nDigitsAddr_;
		int bytesPerRow_;
		uint32_t addrStart_;
		int bytesPerElem_;
		bool bigEndianFlag_;
		bool addrFlag_;
		bool asciiFlag_;
	public:
		DumpT(Printable* pPrintable = nullptr);
		DumpT(Printable& printable) : DumpT(&printable) {}
	public:
		DumpT& SetPrintable(Printable& printable) { pPrintable_ = &printable; return *this; }
		DumpT& UpperCase() { upperCaseFlag_ = true; return *this; }
		DumpT& LowerCase() { upperCaseFlag_ = false; return *this; }
		DumpT& DigitsAddr(int nDigitsAddr) { nDigitsAddr_ = nDigitsAddr; return *this; }
		DumpT& AutoDigitsAddr() { nDigitsAddr_ = -1; return *this; }
		DumpT& BytesPerRow(int bytesPerRow) { bytesPerRow_ = bytesPerRow; return *this; }
		DumpT& AddrStart(uint32_t addrStart) { addrStart_ = addrStart; return *this; }
		DumpT& Data8Bit() { bytesPerElem_ = 1; bigEndianFlag_ = false; return *this; } 
		DumpT& Data16Bit() { bytesPerElem_ = 2; bigEndianFlag_ = false; return *this; } 
		DumpT& Data32Bit() { bytesPerElem_ = 4; bigEndianFlag_ = false; return *this; } 
		DumpT& Data64Bit() { bytesPerElem_ = 8; bigEndianFlag_ = false; return *this; } 
		DumpT& Data16BitBE() { bytesPerElem_ = 2; bigEndianFlag_ = true; return *this; } 
		DumpT& Data32BitBE() { bytesPerElem_ = 4; bigEndianFlag_ = true; return *this; } 
		DumpT& Data64BitBE() { bytesPerElem_ = 8; bigEndianFlag_ = true; return *this; } 
		DumpT& Addr(bool addrFlag = true) { addrFlag_ = addrFlag; return *this; }
		DumpT& Ascii(bool asciiFlag = true) { asciiFlag_ = asciiFlag; return *this; }
	public:
		int GetBytesPerRow() const { return bytesPerRow_; }
		int GetBytesPerElem() const { return bytesPerElem_; }
		bool IsUpperCase() const { return upperCaseFlag_; }
		bool IsBigEndian() const { return bigEndianFlag_; }
		bool HasAscii() const { return asciiFlag_; }
	public:
		DumpT& operator()(const void* buff, int bytes);
	private:
		void PrintAscii(Printable& printable, const char* asciiBuff, int bytes);
	};
public:
	DumpT Dump;
private:
	static Printable* pStandardOutput_;
public:
	Printable() : Dump(this) {}
public:
	virtual Printable& ClearScreen() = 0;
	virtual bool Flush() = 0;
	virtual Printable& Locate(int col, int row) = 0;
	virtual Printable& GetSize(int* pnCols, int* pnRows) { *pnCols = 80, *pnRows = 25; return *this; }
	virtual Printable& PutChar(char ch) { return PutCharRaw(ch); };
	virtual Printable& PutCharRaw(char ch) = 0;
	virtual Printable& Print(const char* str);
	virtual Printable& PrintRaw(const char* str);
	virtual Printable& Println(const char* str = "");
	virtual Printable& PrintlnRaw(const char* str = "");
	virtual Printable& VPrintf(const char* format, va_list args);
	virtual Printable& VPrintfRaw(const char* format, va_list args);
	Printable& PutByte(uint8_t data) { return PutCharRaw(static_cast<uint8_t>(data)); }
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
	virtual bool Flush() override { return true; }
	virtual Printable& Locate(int col, int row) override { return *this; }
	virtual Printable& PutCharRaw(char ch) override { return *this; }
};

//------------------------------------------------------------------------------
// Functions that expose methods provided by Printable class.
//------------------------------------------------------------------------------
extern Printable::DumpT Dump;

inline Printable& ClearScreen() { return Printable::GetStandardOutput().ClearScreen(); }
inline bool Flush() { return Printable::GetStandardOutput().Flush(); }
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
