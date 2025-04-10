//==============================================================================
// KeyData.cpp
//==============================================================================
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyData
//------------------------------------------------------------------------------
bool KeyData::IsCtrlDown() const	{ return Keyboard::IsCtrlDown(modifier_); }
bool KeyData::IsShiftDown() const	{ return Keyboard::IsShiftDown(modifier_); }
bool KeyData::IsAltDown() const		{ return Keyboard::IsAltDown(modifier_); }
bool KeyData::IsWindowsDown() const	{ return Keyboard::IsWindowsDown(modifier_); }

}
