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
LineBuff::LineBuff(int bytesBuff) : bytesBuff_{bytesBuff}, buffBegin_{nullptr},
		pBuffLast_{nullptr}, pLineFirst_{nullptr}, pLineLast_{nullptr}, pLineMark_{nullptr}
{
}

LineBuff::~LineBuff()
{
	::free(buffBegin_);
}

bool LineBuff::Initialize()
{
	if (buffBegin_) return true;
	buffBegin_ = reinterpret_cast<char*>(::malloc(bytesBuff_));
	if (!buffBegin_) return false;
	::memset(buffBegin_, 0x00, bytesBuff_);
	pBuffLast_ = buffBegin_;
	return true;
}

void LineBuff::Clear()
{
	pBuffLast_ = buffBegin_;
	pLineFirst_ = pLineLast_ = nullptr;
	pLineMark_ = nullptr;
}

bool LineBuff::MoveLineMarkUp(int nLines, const char* pLineStop)
{
	int nLinesMove = 0;
	for ( ; nLinesMove < nLines && (pLineMark_ != pLineStop) && PrevLine(&pLineMark_); nLinesMove++) ;
	return nLinesMove > 0;
}

bool LineBuff::MoveLineMarkDown(int nLines, const char* pLineStop)
{
	int nLinesMove = 0;
	for ( ; nLinesMove < nLines && (pLineMark_ != pLineStop) && NextLine(&pLineMark_); nLinesMove++) ;
	return nLinesMove > 0;
}

bool LineBuff::PrevLine(char** pp) const
{
	char* p = *pp;
	if (p == pLineFirst_) return false;
	WrappedPointer<const char*> pointer(p, GetBuffBegin(), GetBuffEnd());
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
	*pp = const_cast<char*>(pointer.Forward().GetPointer());
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
	WrappedPointer<const char*> pointer(p, GetBuffBegin(), GetBuffEnd());
	if (!pointer.Get()) return pointer.Forward().GetPointer();
	for ( ; pointer.Get(); pointer.Forward()) ;
	*pp = const_cast<char*>(pointer.Forward().GetPointer());
	return true;
}

bool LineBuff::NextLine(const char** pp, int nLines) const
{
	for (int iLine = 0; iLine < nLines; iLine++) {
		if (!NextLine(pp)) return false;
	}
	return true;
}

LineBuff::Reader LineBuff::CreateReader() const
{
	return Reader(pLineFirst_, GetBuffBegin(), GetBuffEnd(), pBuffLast_);
}

void LineBuff::PrintInfo(Printable& printable) const
{
	char buff[32];
	auto ToString = [this](char* buff, const char* p) -> const char* {
		if (p) {
			::sprintf(buff, "%04x", p - GetBuffBegin());
			return buff;
		} else {
			return "(null)";
		}
	};
	printable.Dump.DigitsAddr(4).Ascii()(GetBuffBegin(), bytesBuff_);
	printable.Printf("pBuffLast:%s", ToString(buff, pBuffLast_));
	printable.Printf(" pLineFirst:%s", ToString(buff, pLineFirst_));
	printable.Printf(" pLineLast:%s\n", ToString(buff, pLineLast_));
}

Printable& LineBuff::PutChar(char ch)
{
	*pBuffLast_ = ch;
	WrappedPointer<char*> pointer(pBuffLast_, GetBuffBegin(), GetBuffEnd());
	pBuffLast_ = pointer.Forward().GetPointer();
	if (!pLineFirst_) {
		pLineFirst_ = GetBuffBegin();
	} else if (pLineFirst_ == pBuffLast_) {
		NextLine(&pLineFirst_);
	}
	if (pLineMark_ == pBuffLast_) NextLine(&pLineMark_);
	return *this;
}

//------------------------------------------------------------------------------
// LineBuff::Reader
//------------------------------------------------------------------------------
bool LineBuff::Reader::Read(void* buff, int bytesBuff, int* pBytesRead)
{
	int bytesRead = 0;
	char* pDst = reinterpret_cast<char*>(buff);
	for ( ; bytesRead < bytesBuff; charFeeder_.Forward()) {
		const char* pBuffCur = charFeeder_.GetPointer();
		if (pBuffCur == pBuffLast_) break;
		if (*pBuffCur) {
			bytesRead++;
			*pDst++ = *pBuffCur;
		}
	}
	*pBytesRead = bytesRead;
	return bytesRead > 0;
}

}
