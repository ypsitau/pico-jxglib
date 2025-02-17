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
class LineBuff : public Printable {
public:
	class Reader : public Stream {
	private:
		WrappedCharFeeder charFeeder_;
		const char* pBuffLast_;
	public:
		Reader(const char* pBuffCur, const char* buffBegin, const char* buffEnd, const char* pBuffLast) :
			charFeeder_(pBuffCur, buffBegin, buffEnd), pBuffLast_{pBuffLast} {}
		Reader(const Reader& stream) : charFeeder_(stream.charFeeder_) {}
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
	const char* pLineMark_;
public:
	LineBuff();
	~LineBuff();
public:
	bool Initialize(int bytes);
	void Clear();
public:
	char* GetBuffBegin() { return buffBegin_; }
	char* GetBuffEnd() { return buffEnd_; }
	char* GetWrite() { return pBuffLast_; }
	char* GetLineFirst() { return pLineFirst_; }
	char* GetLineLast() { return pLineLast_; }
	const char* GetBuffBegin() const { return buffBegin_; }
	const char* GetBuffEnd() const { return buffEnd_; }
	const char* GetWrite() const { return pBuffLast_; }
	const char* GetLineFirst() const { return pLineFirst_; }
	const char* GetLineLast() const { return pLineLast_; }
	const char* GetLineMark() const { return pLineMark_; }
	LineBuff& MoveLineLastHere() { pLineLast_ = pBuffLast_; return *this; }
	LineBuff& SetLineMark(const char* pLineMark) { pLineMark_ = pLineMark; return *this; }
	LineBuff& RemoveLineMark() { pLineMark_ = nullptr; return *this; }
	bool MoveLineMarkUp(const char* pLineStop = nullptr) {
		return (pLineMark_ != pLineStop) && PrevLine(&pLineMark_);
	}
	bool MoveLineMarkDown(const char* pLineStop = nullptr) {
		return (pLineMark_ != pLineStop) && NextLine(&pLineMark_);
	}
public:
	bool PrevLine(char** pp) const;
	bool PrevLine(const char** pp) const { return PrevLine(const_cast<char**>(pp)); }
	bool PrevLine(const char** pp, int nLines) const;
	bool NextLine(char** pp) const;
	bool NextLine(const char** pp) const { return NextLine(const_cast<char**>(pp)); }
	bool NextLine(const char** pp, int nLines) const;
	LineBuff& PlaceChar(char ch) { *pBuffLast_ = ch; return *this; }
	WrappedCharFeeder CreateCharFeeder(const char* pBuffCur) { return WrappedCharFeeder(pBuffCur, buffBegin_, buffEnd_); }
	Reader CreateReader() const;
public:
	void PrintInfo(Printable& printable) const;
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { Clear(); return *this; }
	virtual Printable& RefreshScreen() override { /* do nothing */ return *this; }
	virtual Printable& Locate(int col, int row) override { /* do nothing */ return *this; }
	virtual Printable& PutChar(char ch) override;
};

}

#endif
