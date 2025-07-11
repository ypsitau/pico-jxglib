//==============================================================================
// jxglib/LineBuff.h
//==============================================================================
#ifndef PICO_JXGLIB_LINEBUFF_H
#define PICO_JXGLIB_LINEBUFF_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

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
		virtual int Read(void* buff, int bytesBuff) override;
		virtual int Write(const void* buff, int bytesBuff) override { /* do nothing */ return 0; };
	};
private:
	int bytesBuff_;
	char* buffBegin_;
	char* pBuffLast_;
	char* pLineFirst_;
	char* pLineLast_;
	const char* pLineMark_;
public:
	LineBuff(int bytes = 4096);
	~LineBuff();
public:
	bool Initialize();
	void Clear();
public:
	char* GetBuffBegin() { return buffBegin_; }
	char* GetBuffEnd() { return buffBegin_ + bytesBuff_; }
	char* GetWrite() { return pBuffLast_; }
	char* GetLineFirst() { return pLineFirst_; }
	char* GetLineLast() { return pLineLast_; }
	const char* GetBuffBegin() const { return buffBegin_; }
	const char* GetBuffEnd() const { return buffBegin_ + bytesBuff_; }
	const char* GetWrite() const { return pBuffLast_; }
	const char* GetLineFirst() const { return pLineFirst_; }
	const char* GetLineLast() const { return pLineLast_; }
	const char* GetLineMark() const { return pLineMark_; }
	LineBuff& MoveLineLastHere() { pLineLast_ = pBuffLast_; return *this; }
	LineBuff& SetLineMark(const char* pLineMark) { pLineMark_ = pLineMark; return *this; }
	LineBuff& RemoveLineMark() { pLineMark_ = nullptr; return *this; }
	bool MoveLineMarkUp(int nLines = 1, const char* pLineStop = nullptr);
	bool MoveLineMarkDown(int nLines = 1, const char* pLineStop = nullptr);
public:
	bool PrevLine(char** pp) const;
	bool PrevLine(const char** pp) const { return PrevLine(const_cast<char**>(pp)); }
	bool PrevLine(const char** pp, int nLines) const;
	bool NextLine(char** pp) const;
	bool NextLine(const char** pp) const { return NextLine(const_cast<char**>(pp)); }
	bool NextLine(const char** pp, int nLines) const;
	LineBuff& PlaceChar(char ch) { *pBuffLast_ = ch; return *this; }
	WrappedCharFeeder CreateCharFeeder(const char* pBuffCur) { return WrappedCharFeeder(pBuffCur, GetBuffBegin(), GetBuffEnd()); }
	Reader CreateReader() const;
public:
	void PrintInfo(Printable& printable) const;
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { Clear(); return *this; }
	virtual bool Flush() override { /* do nothing */ return true; }
	virtual Printable& Locate(int col, int row) override { /* do nothing */ return *this; }
	virtual Printable& PutCharRaw(char ch) override;
};

}

#endif
