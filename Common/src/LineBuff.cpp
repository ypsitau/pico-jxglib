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

LineBuff& LineBuff::MarkLineLast()
{
	if (pLineFirst_ == pWrite_) NextLine(&pLineFirst_);
	pLineLast_ = pWrite_;
	return *this;
} 

LineBuff& LineBuff::PutChar(char ch)
{
	if (!pLineFirst_) {
		pLineFirst_ = buffBegin_;
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
	char buff[32];
	auto ToString = [this](char* buff, const char* p) -> const char* {
		if (p) {
			::sprintf(buff, "%04x", p - buffBegin_);
			return buff;
		} else {
			return "(null)";
		}
	};
	Printable::DumpT().DigitsAddr(4).PrintAscii()(buffBegin_, buffEnd_ - buffBegin_);
	Printf("buffEnd:%s", ToString(buff, buffEnd_));
	Printf(" pWrite:%s", ToString(buff, pWrite_));
	Printf(" pLineFirst:%s", ToString(buff, pLineFirst_));
	Printf(" pLineLast:%s\n", ToString(buff, pLineLast_));
}

}
