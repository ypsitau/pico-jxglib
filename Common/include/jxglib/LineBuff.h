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
	class Stream : public jxglib::Stream {
	private:
		WrappedCharFeeder charFeeder_;
		const char* pBuffLast_;
	public:
		Stream(const char* p, const char* buffBegin, const char* biffEnd, const char* pBuffLast) :
			charFeeder_(p, buffBegin, biffEnd), pBuffLast_{pBuffLast} {}
		Stream(const Stream& stream) : charFeeder_(stream.charFeeder_) {}
	public:
		// virtual functions of Stream
		virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) override;
		virtual bool Write(const void* buff, int bytesBuff) override { /* do nothing */ return false; };
	};
private:
	char* buffBegin_;
	char* buffEnd_;
	char* pBuffLast_;
	char* pLineFirst_;
	char* pLineLast_;
	char* pLineMark_;
public:
	LineBuff();
	~LineBuff();
public:
	bool Allocate(int bytes);
public:
	char* GetBuffBegin() { return buffBegin_; }
	char* GetBuffEnd() { return buffEnd_; }
	char* GetWrite() { return pBuffLast_; }
	char* GetLineFirst() { return pLineFirst_; }
	char* GetLineLast() { return pLineLast_; }
	char* GetLineMark() { return pLineMark_; }
	const char* GetBuffBegin() const { return buffBegin_; }
	const char* GetBuffEnd() const { return buffEnd_; }
	const char* GetWrite() const { return pBuffLast_; }
	const char* GetLineFirst() const { return pLineFirst_; }
	const char* GetLineLast() const { return pLineLast_; }
	const char* GetLineMark() const { return pLineMark_; }
public:
	bool PrevLine(char** pp) const;
	bool PrevLine(const char** pp) const { return PrevLine(const_cast<char**>(pp)); }
	bool PrevLine(const char** pp, int nLines) const;
	bool NextLine(char** pp) const;
	bool NextLine(const char** pp) const { return NextLine(const_cast<char**>(pp)); }
	bool NextLine(const char** pp, int nLines) const;
	LineBuff& MoveLineLastHere();
	LineBuff& PlaceChar(char ch) { *pBuffLast_ = ch; return *this; }
	LineBuff& PutChar(char ch);
	LineBuff& PutString(const char* str);
	WrappedCharFeeder CreateCharFeeder(const char* p) { return WrappedCharFeeder(p, buffBegin_, buffEnd_); }
	Stream CreateStream() const;
public:
	void Print() const;
};

}

#endif
