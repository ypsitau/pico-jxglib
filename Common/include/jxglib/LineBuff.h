//==============================================================================
// jxglib/LineBuff.h
//==============================================================================
#ifndef PICO_JXGLIB_LINEBUFF_H
#define PICO_JXGLIB_LINEBUFF_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/WrappedPointer.h"
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LineBuff
//------------------------------------------------------------------------------
class LineBuff {
public:
	class StreamEx : public Stream {
	};
private:
	char* buffBegin_;
	char* buffEnd_;
	char* pBuffLast_;
	char* pLineFirst_;
	char* pLineLast_;
public:
	LineBuff();
	~LineBuff();
public:
	bool Allocate(int bytes);
public:
	const char* GetBuffBegin() const { return buffBegin_; }
	const char* GetBuffEnd() const { return buffEnd_; }
	const char* GetWrite() const { return pBuffLast_; }
	const char* GetLineFirst() const { return pLineFirst_; }
	const char* GetLineLast() const { return pLineLast_; }
public:
	bool PrevLine(char** pp) const;
	bool PrevLine(const char** pp) const { return PrevLine(const_cast<char**>(pp)); }
	bool PrevLine(const char** pp, int nLines) const;
	bool NextLine(char** pp) const;
	bool NextLine(const char** pp) const { return NextLine(const_cast<char**>(pp)); }
	bool NextLine(const char** pp, int nLines) const;
	LineBuff& MarkLineLast();
	LineBuff& PlaceChar(char ch) { *pBuffLast_ = ch; return *this; }
	LineBuff& PutChar(char ch);
	LineBuff& PutString(const char* str);
	WrappedCharFeeder MakeCharFeeder(const char* p) { return WrappedCharFeeder(p, buffBegin_, buffEnd_); }
public:
	void Print() const;
};

}

#endif
