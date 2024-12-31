//==============================================================================
// LineBuff.cpp
//==============================================================================
#include <malloc.h>
#include <memory.h>
#include "jxglib/Printable.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

LineBuff::LineBuff() : buffBegin_{nullptr}, buffEnd_{nullptr}, pWrite_{nullptr}, pLineTop_{nullptr}, pLineCur_{nullptr}
{
}

bool LineBuff::Allocate(int bytes)
{
	buffBegin_ = reinterpret_cast<char*>(::malloc(bytes));
	::memset(buffBegin_, 0x00, bytes);
	if (!buffBegin_) return false;
	buffEnd_ = buffBegin_ + bytes;
	pWrite_ = buffBegin_;
	return true;
}

char* LineBuff::PrevLine(char* p) const
{
	PointerWrapped<char*> pointer(p, buffBegin_, buffEnd_);
	while (pointer.Get()) pointer.Backward();
	pointer.Backward();
	while (pointer.Get()) pointer.Backward();
	return pointer.Forward().GetPointer();
}

const char* LineBuff::PrevLine(const char* p, int nLines) const
{
	for (int iLine = 0; iLine < nLines; iLine++) p = PrevLine(p);
	return p;
}

char* LineBuff::NextLine(char* p) const
{
	PointerWrapped<char*> pointer(p, buffBegin_, buffEnd_);
	while (pointer.Get()) pointer.Forward();
	return pointer.Forward().GetPointer();
}

const char* LineBuff::NextLine(const char* p, int nLines) const
{
	for (int iLine = 0; iLine < nLines; iLine++) p = NextLine(p);
	return p;
}

LineBuff& LineBuff::MarkLineCur()
{
	pLineCur_ = pWrite_;
	if (!pLineTop_) pLineTop_ = pLineCur_;
	return *this;
} 

LineBuff& LineBuff::PutChar(char ch)
{
	if (!pLineTop_) {
		pLineCur_ = pLineTop_ = buffBegin_;
	} else if (pWrite_ != pLineTop_) {
		// do nothing
	} else if (pLineCur_ == pLineTop_) {
		pLineCur_ = pLineTop_ = NextLine(pLineTop_);
	} else {
		pLineTop_ = NextLine(pLineTop_);
	}
	*pWrite_ = ch;
	PointerWrapped<char*> pointer(pWrite_, buffBegin_, buffEnd_);
	pWrite_ = pointer.Forward().GetPointer();
	return *this;
}

LineBuff& LineBuff::PutString(const char* str)
{
	for (const char* p = str; *p; p++) PutChar(*p);
	return *this;
}

void LineBuff::Print() const
{
	Dump(buffBegin_, buffEnd_ - buffBegin_);
	Printf("buffEnd:%04x pWrite:%04x pLineTop:%04x pLineCur:%04x\n",
		buffEnd_ - buffBegin_, pWrite_ - buffBegin_, pLineTop_ - buffBegin_, pLineCur_ - buffBegin_);
}

}
