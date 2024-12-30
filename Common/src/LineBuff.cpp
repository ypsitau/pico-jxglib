//==============================================================================
// LineBuff.cpp
//==============================================================================
#include <malloc.h>
#include "jxglib/LineBuff.h"

namespace jxglib {

LineBuff::LineBuff() : buffTop_{nullptr}, buffBottom_{nullptr}, pWrite_{nullptr}, pRead_{nullptr}
{
}

bool LineBuff::Allocate(int bytes)
{
	buffTop_ = reinterpret_cast<char*>(::malloc(bytes));
	if (!buffTop_) return false;
	buffBottom_ = buffTop_ + bytes;
	pWrite_ = pRead_ = pLineTop_ = pLineCur_ = buffTop_;
	return true;
}

const char* LineBuff::PrevLine(const char* p) const
{
	for ( ; *p; p--) if (p == buffTop_) p = buffBottom_ - 1;
	return (p == buffTop_)? buffBottom_ - 1 : p - 1;
}

const char* LineBuff::NextLine(const char* p) const
{
	for ( ; *p; p++) if (p == buffBottom_) p = buffTop_;
	return p + 1;
}

}
