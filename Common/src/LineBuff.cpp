//==============================================================================
// LineBuff.cpp
//==============================================================================
#include <malloc.h>
#include <memory.h>
#include "jxglib/WrappedPointer.h"
#include "jxglib/Printable.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LineBuff
//------------------------------------------------------------------------------
LineBuff::LineBuff() : buffBegin_{nullptr}, buffEnd_{nullptr}, pBuffLast_{nullptr}, pLineFirst_{nullptr}, pLineLast_{nullptr}
{
}

LineBuff::~LineBuff()
{
	::free(buffBegin_);
}

bool LineBuff::Allocate(int bytes)
{
	buffBegin_ = reinterpret_cast<char*>(::malloc(bytes));
	::memset(buffBegin_, 0xcc, bytes);
	if (!buffBegin_) return false;
	buffEnd_ = buffBegin_ + bytes;
	pBuffLast_ = buffBegin_;
	return true;
}

bool LineBuff::PrevLine(char** pp) const
{
	char* p = *pp;
	if (p == pLineFirst_) return false;
	WrappedPointer<char*> pointer(p, buffBegin_, buffEnd_);
	if (pointer.Get()) {
		for ( ; pointer.Get(); pointer.Backward()) ;
	} else {
		pointer.Backward();
	}
	pointer.Backward();
	for ( ; pointer.Get(); pointer.Backward()) {
		if (pointer.GetPointer() == pLineFirst_) {
			*pp = pLineFirst_;
			return true;
		}
	}
	*pp = pointer.Forward().GetPointer();
	return true;
}

bool LineBuff::PrevLine(const char** pp, int nLines) const
{
	for (int iLine = 0; iLine < nLines; iLine++) {
		if (!PrevLine(pp)) return false;
	}
	return true;
}

bool LineBuff::NextLine(char** pp) const
{
	char* p = *pp;
	if (p == pLineLast_) return false;
	WrappedPointer<char*> pointer(p, buffBegin_, buffEnd_);
	if (!pointer.Get()) return pointer.Forward().GetPointer();
	for ( ; pointer.Get(); pointer.Forward()) ;
	*pp = pointer.Forward().GetPointer();
	return true;
}

bool LineBuff::NextLine(const char** pp, int nLines) const
{
	for (int iLine = 0; iLine < nLines; iLine++) {
		if (!NextLine(pp)) return false;
	}
	return true;
}

LineBuff& LineBuff::PutChar(char ch)
{
	*pBuffLast_ = ch;
	WrappedPointer<char*> pointer(pBuffLast_, buffBegin_, buffEnd_);
	pBuffLast_ = pointer.Forward().GetPointer();
	if (!pLineFirst_) {
		pLineFirst_ = buffBegin_;
	} else if (pLineFirst_ == pBuffLast_) {
		NextLine(&pLineFirst_);
	}
	if (pLineMark_ == pBuffLast_) NextLine(&pLineMark_);
	return *this;
}

LineBuff& LineBuff::PutString(const char* str)
{
	for (const char* p = str; *p; p++) PutChar(*p);
	return *this;
}

LineBuff::Stream LineBuff::CreateStream() const
{
	return Stream(pLineFirst_, buffBegin_, buffEnd_, pBuffLast_);
}

void LineBuff::Print() const
{
	char buff[32];
	auto ToString = [this](char* buff, const char* p) -> const char* {
		if (p) {
			::sprintf(buff, "%04x", p - buffBegin_);
			return buff;
		} else {
			return "(null)";
		}
	};
	Printable::DumpT().DigitsAddr(4).Ascii()(buffBegin_, buffEnd_ - buffBegin_);
	Printf("buffEnd:%s", ToString(buff, buffEnd_));
	Printf(" pBuffLast:%s", ToString(buff, pBuffLast_));
	Printf(" pLineFirst:%s", ToString(buff, pLineFirst_));
	Printf(" pLineLast:%s\n", ToString(buff, pLineLast_));
}

//------------------------------------------------------------------------------
// LineBuff::Stream
//------------------------------------------------------------------------------
bool LineBuff::Stream::Read(void* buff, int bytesBuff, int* pBytesRead)
{
	int bytesRead = 0;
	char* pDst = reinterpret_cast<char*>(buff);
	for ( ; bytesRead < bytesBuff; charFeeder_.Forward()) {
		const char* p = charFeeder_.GetPointer();
		if (p == pBuffLast_) break;
		if (*p) {
			bytesRead++;
			*pDst++ = *p;
		}
	}
	*pBytesRead = bytesRead;
	return bytesRead > 0;
}

}
