//==============================================================================
// KeyData.cpp
//==============================================================================
#include "jxglib/KeyData.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyData
//------------------------------------------------------------------------------
#if 0
uint8_t KeyData::GetKeyCode() const
{
	return (keyCodeFlag_ || code_ == VK_BACK || code_== VK_TAB || code_ == VK_RETURN ||
		code_ == VK_ESCAPE || code_ == VK_SPACE)? code_ :
		('0' <= code_ && code_ <= '9')? code_ :
		('A' <= code_ && code_ <= 'Z')? code_ :
		('a' <= code_ && code_ <= 'z')? code_ - 'a' + 'A' : 0;
}

char KeyData::GetChar() const
{
	return IsChar()? static_cast<char>(code_) : '\0';
}

bool KeyData::IsKeyCode() const
{
	return keyCodeFlag_ || code_ == VK_BACK || code_== VK_TAB || code_ == VK_RETURN ||
		code_ == VK_ESCAPE || code_ == VK_SPACE ||
		('0' <= code_ && code_ <= '9') ||
		('A' <= code_ && code_ <= 'Z') ||
		('a' <= code_ && code_ <= 'z');
}

bool KeyData::IsChar() const
{
	return !keyCodeFlag_ || code_ == VK_BACK || code_== VK_TAB || code_ == VK_RETURN ||
		code_ == VK_ESCAPE || code_ == VK_SPACE ||
		('0' <= code_ && code_ <= '9') ||
		('A' <= code_ && code_ <= 'Z');
}
#endif

}
