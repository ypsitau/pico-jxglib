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
	pWrite_ = buffBegin_;
	return true;
}

bool LineBuff::PrevLine(char** pp) const
{
	char* p = *pp;
	if (p == pLineFirst_) return false;
	PointerWrapped<char*> pointer(p, buffBegin_, buffEnd_);
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
	PointerWrapped<char*> pointer(p, buffBegin_, buffEnd_);
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
		NextLine(&pLineFirst_);
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
