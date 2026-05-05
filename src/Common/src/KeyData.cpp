//==============================================================================
// KeyData.cpp
//==============================================================================
#include <stdio.h>
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyData
//------------------------------------------------------------------------------
bool KeyData::IsCtrlDown() const	{ return Keyboard::IsCtrlDown(modifier_); }
bool KeyData::IsShiftDown() const	{ return Keyboard::IsShiftDown(modifier_); }
bool KeyData::IsAltDown() const		{ return Keyboard::IsAltDown(modifier_); }
bool KeyData::IsWindowsDown() const	{ return Keyboard::IsWindowsDown(modifier_); }

const char* KeyData::Textize(char* str, int len) const
{
	if (IsKeyCode()) {
		::snprintf(str, len, "VK_%s", GetKeyCodeName(GetKeyCode()));
	} else if (GetChar() <= 'Z' - '@') {
		::snprintf(str, len, "Ctrl+%c", GetChar() + '@');
	} else {
		::snprintf(str, len, "'%c'", GetChar());
	}
	return str;
}

}
