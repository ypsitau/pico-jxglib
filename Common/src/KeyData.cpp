//==============================================================================
// KeyData.cpp
//==============================================================================
#include "jxglib/KeyData.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyData
//------------------------------------------------------------------------------
char KeyData::GetChar() const
{
	return !keyCodeFlag_? static_cast<char>(code_) :
		(code_ == VK_RETURN)?	'\r' :
		(code_ == VK_TAB)?		'\t' :
		(code_ == VK_BACK)?		'\b' :
		(code_ == VK_ESCAPE)?	'\x1b' :
		(code_ == VK_SPACE)?	' ' : '\0';
}

bool KeyData::IsChar() const
{
	return !keyCodeFlag_ || code_ == VK_RETURN || code_== VK_TAB ||
			code_ == VK_BACK || code_ == VK_ESCAPE || code_ == VK_SPACE;
}

}
