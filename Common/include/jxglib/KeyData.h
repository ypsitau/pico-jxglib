//==============================================================================
// jxglib/KeyData.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYDATA_H
#define PICO_JXGLIB_KEYDATA_H
#include "pico/stdlib.h"
#include "jxglib/KeyCode.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyData
//------------------------------------------------------------------------------
struct KeyData {
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
public:
	uint8_t code_;
	bool keyCodeFlag_;
	uint8_t modifier_;
public:
	KeyData() : code_{0}, keyCodeFlag_{false}, modifier_{0} {}
	KeyData(uint8_t code, bool keyCodeFlag, uint8_t modifier) :
		code_{code}, keyCodeFlag_{keyCodeFlag}, modifier_{modifier} {}
	KeyData(const KeyData& keyData) :
		code_{keyData.code_}, keyCodeFlag_{keyData.keyCodeFlag_}, modifier_{keyData.modifier_} {}
public:
	bool IsValid() const { return code_ != 0; }
	uint8_t GetKeyCode() const { return keyCodeFlag_? code_ : 0; }
	const char* GetKeyCodeName() const { return jxglib::GetKeyCodeName(code_); }
	char GetChar() const;
	char GetCharRaw() const { return keyCodeFlag_? '\0' : static_cast<char>(code_); }
	bool IsKeyCode() const { return keyCodeFlag_; }
	bool IsChar() const;
	bool IsCharRaw() const { return !keyCodeFlag_; }
	uint8_t GetModifier() const { return modifier_; }
	bool IsCtrlDown() const		{ return !!(modifier_ & (Mod::CtrlL | Mod::CtrlR)); }
	bool IsShiftDown() const	{ return !!(modifier_ & (Mod::ShiftL | Mod::ShiftR)); }
	bool IsAltDown() const		{ return !!(modifier_ & (Mod::AltL | Mod::AltR)); }
	bool IsWindowsDown() const	{ return !!(modifier_ & (Mod::WindowsL | Mod::WindowsR)); }
};

}

#endif
