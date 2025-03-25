//==============================================================================
// jxglib/Keyboard.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYBOARD_H
#define PICO_JXGLIB_KEYBOARD_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/KeyCode.h"
#include "jxglib/KeyData.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyLayout
//------------------------------------------------------------------------------
class KeyLayout {
public:
	struct CharEntry {
		char charCode;
		char charCodeShift;
	};
private:
	bool nonUSFlag_;
public:
	KeyLayout(bool nonUSFlag) : nonUSFlag_{nonUSFlag} {}
public:
	bool IsNonUS() const { return nonUSFlag_; }
	KeyData CreateKeyData(uint8_t keyCode, uint8_t modifier) const;
public:
	virtual const CharEntry* GetCharEntryTbl() const = 0;
	virtual uint8_t ConvKeyCodeToCharCode(uint8_t keyCode, uint8_t modifier) const;
};

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------
class Keyboard {
private:
	const KeyLayout* pKeyLayout_;
public:
	Keyboard();
public:
	Keyboard& SetKeyLayout(const KeyLayout& keyLayout) { pKeyLayout_ = &keyLayout; return *this; }
	const KeyLayout& GetKeyLayout() const { return *pKeyLayout_; }
	bool GetKeyData(KeyData* pKeyData);
	char GetChar();
public:
	virtual Keyboard& SetCapsLockAsCtrl(bool capsLockAsCtrlFlag = true) { return *this; }
	virtual Keyboard& SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) { return *this; }
	virtual uint8_t GetModifier() { return 0; }
	virtual bool IsPressed(uint8_t keyCode);
	virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1);
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1);
	virtual bool GetKeyDataNB(KeyData* pKeyData) = 0;
};

//------------------------------------------------------------------------------
// KeyLayout_101
//------------------------------------------------------------------------------
class KeyLayout_101 : public KeyLayout {
public:
	static const KeyLayout_101 Instance;
public:
	KeyLayout_101() : KeyLayout(false) {}
	virtual const CharEntry* GetCharEntryTbl() const;
};

//------------------------------------------------------------------------------
// KeyLayout_106
//------------------------------------------------------------------------------
class KeyLayout_106 : public KeyLayout {
public:
	static const KeyLayout_106 Instance;
public:
	KeyLayout_106() : KeyLayout(true) {}
	virtual const CharEntry* GetCharEntryTbl() const;
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
	
}

#endif
