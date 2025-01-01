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
	char* pLineFirst_;
	char* pLineLast_;
public:
	LineBuff();
public:
	bool Allocate(int bytes);
public:
	const char* GetBuffBegin() const { return buffBegin_; }
	const char* GetBuffEnd() const { return buffEnd_; }
	const char* GetWrite() const { return pWrite_; }
	const char* GetLineFirst() const { return pLineFirst_; }
	const char* GetLineLast() const { return pLineLast_; }
public:
	char* PrevLine(char* p) const;
	const char* PrevLine(const char* p) const { return PrevLine(const_cast<char*>(p)); }
	const char* PrevLine(const char* p, int nLines) const;
	char* NextLine(char* p) const;
	const char* NextLine(const char* p) const { return NextLine(const_cast<char*>(p)); }
	const char* NextLine(const char* p, int nLines) const;
	LineBuff& MarkLineLast();
	LineBuff& PlaceChar(char ch) { *pWrite_ = ch; return *this; }
	LineBuff& PutChar(char ch);
	LineBuff& PutString(const char* str);
	CharFeederWrapped MakeCharFeeder(const char* p) { return CharFeederWrapped(p, buffBegin_, buffEnd_); }
public:
	void Print() const;
};

}

#endif
