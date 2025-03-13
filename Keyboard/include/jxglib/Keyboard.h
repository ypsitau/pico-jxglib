//==============================================================================
// jxglib/Keyboard.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYBOARD_H
#define PICO_JXGLIB_KEYBOARD_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/KeyCode.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------
class Keyboard {
public:
	struct Mod {
		static const uint8_t CtrlL		= (1 << 0);
		static const uint8_t ShiftL		= (1 << 1);
		static const uint8_t AltL		= (1 << 2);
		static const uint8_t WindowsL	= (1 << 3);
		static const uint8_t CtrlR		= (1 << 4);
		static const uint8_t ShiftR		= (1 << 5);
		static const uint8_t AltR		= (1 << 6);
		static const uint8_t WindowsR	= (1 << 7);
	};
	struct KeyData {
		int code;
		bool vkFlag;
		uint8_t modifier;
	public:
		bool IsCtrlDown() const		{ return !!(modifier & (Mod::CtrlL | Mod::CtrlR)); }
		bool IsShiftDown() const	{ return !!(modifier & (Mod::ShiftL | Mod::ShiftR)); }
		bool IsAltDown() const		{ return !!(modifier & (Mod::AltL | Mod::AltR)); }
		bool IsWindowsDown() const	{ return !!(modifier & (Mod::WindowsL | Mod::WindowsR)); }
	};
public:
	Keyboard() {}
public:
	virtual bool GetKeyData(KeyData& keyData) = 0;
};

}

#endif
