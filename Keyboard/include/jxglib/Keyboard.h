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
	char GetChar();
	uint8_t GetKeyCode();
public:
	virtual bool GetKeyDataNB(KeyData* pKeyData) = 0;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) = 0;
};

//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
class KeyboardDumb : public Keyboard {
public:
	static KeyboardDumb Instance;
public:
	virtual bool GetKeyDataNB(KeyData* pKeyData) override { return false; }
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override { return 0; }
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
	virtual bool GetKeyDataNB(KeyData* pKeyData) override;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override;
};

}

#endif
