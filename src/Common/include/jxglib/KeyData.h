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
	char GetChar() const { return keyCodeFlag_? '\0' : static_cast<char>(code_); }
	bool IsKeyCode() const { return keyCodeFlag_; }
	bool IsChar() const { return !keyCodeFlag_; }
	uint8_t GetModifier() const { return modifier_; }
	bool IsCtrlDown() const;
	bool IsShiftDown() const;
	bool IsAltDown() const;
	bool IsWindowsDown() const;
};

}

#endif
