//==============================================================================
// jxglib/LineBuff.h
//==============================================================================
#ifndef PICO_JXGLIB_LINEBUFF_H
#define PICO_JXGLIB_LINEBUFF_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Pointer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LineBuff
//------------------------------------------------------------------------------
class LineBuff {
private:
	char* buffBegin_;
	char* buffEnd_;
	char* pWrite_;
	char* pLineTop_;
	char* pLineCur_;
public:
	LineBuff();
public:
	bool Allocate(int bytes);
	const char* GetLineCur() const { return pLineCur_; }
	char* PrevLine(char* p) const;
	const char* PrevLine(const char* p) const { return PrevLine(const_cast<char*>(p)); }
	const char* PrevLine(const char* p, int nLines) const;
	char* NextLine(char* p) const;
	const char* NextLine(const char* p) const { return NextLine(const_cast<char*>(p)); }
	const char* NextLine(const char* p, int nLines) const;
	LineBuff& MarkLineCur();
	LineBuff& PlaceChar(char ch) { *pWrite_ = ch; return *this; }
	LineBuff& PutChar(char ch);
	LineBuff& PutString(const char* str);
	CharFeederWrapped MakeCharFeeder(const char* p) { return CharFeederWrapped(p, buffBegin_, buffEnd_); }
};

}

#endif
