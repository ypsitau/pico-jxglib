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
	Keyboard() {}
public:
	virtual bool GetKeyData(KeyData& keyData) = 0;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax) = 0;
};

//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
class KeyboardDumb : public Keyboard {
public:
	static KeyboardDumb Instance;
public:
	virtual bool GetKeyData(KeyData& keyData) override { return false; }
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax) override { return 0; }
};
	
//------------------------------------------------------------------------------
// KeyboardStdio
//------------------------------------------------------------------------------
class KeyboardStdio : public Keyboard {
private:
	VT100::Decoder decoder_;
public:
	static KeyboardStdio Instance;
public:
	virtual bool GetKeyData(KeyData& keyData) override;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax) override;
};

}

#endif
