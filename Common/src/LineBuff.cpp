//==============================================================================
// LineBuff.cpp
//==============================================================================
#include <malloc.h>
#include <memory.h>
#include "jxglib/Printable.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

LineBuff::LineBuff() : buffBegin_{nullptr}, buffEnd_{nullptr}, pWrite_{nullptr}, pLineFirst_{nullptr}, pLineLast_{nullptr}
{
}

bool LineBuff::Allocate(int bytes)
{
	buffBegin_ = reinterpret_cast<char*>(::malloc(bytes));
	::memset(buffBegin_, 0xcc, bytes);
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
	while (pointer.Get()) {
		if (pointer.GetPointer() == pLineFirst_) return pLineFirst_;
		pointer.Backward();
	}
	return pointer.Forward().GetPointer();
}

const char* LineBuff::PrevLine(const char* p, int nLines) const
{
	for (int iLine = 0; iLine < nLines && p != pLineFirst_; iLine++) p = PrevLine(p);
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

LineBuff& LineBuff::MarkLineLast()
{
	pLineLast_ = pWrite_;
	return *this;
} 

LineBuff& LineBuff::PutChar(char ch)
{
	if (!pLineFirst_) {
		pLineFirst_ = buffBegin_;
		if (!pLineLast_) pLineLast_ = pLineFirst_;
	} else if (pWrite_ == pLineFirst_) {
		pLineFirst_ = NextLine(pLineFirst_);
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
	Printf("buffEnd:%04x pWrite:%04x pLineFirst:%04x pLineLast:%04x\n",
		buffEnd_ - buffBegin_, pWrite_ - buffBegin_, pLineFirst_ - buffBegin_, pLineLast_ - buffBegin_);
}

}
