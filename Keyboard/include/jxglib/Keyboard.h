//==============================================================================
// jxglib/Keyboard.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYBOARD_H
#define PICO_JXGLIB_KEYBOARD_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/KeyCode.h"
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------
class Keyboard {
public:
public:
	Keyboard() {}
public:
	virtual bool GetKeyData(KeyData& keyData) = 0;
};

class KeyboardStdio : public Keyboard {
private:
	VT100::Decoder decoder_;
public:
	static KeyboardStdio Instance;
public:
	virtual bool GetKeyData(KeyData& keyData);
};

}

#endif
