//==============================================================================
// jxglib/Keyboard.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYBOARD_H
#define PICO_JXGLIB_KEYBOARD_H
#include "pico/stdlib.h"
#include "jxglib/KeyLayout.h"
#include "jxglib/Tickable.h"

namespace jxglib {

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
	virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1) = 0;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) = 0;
	virtual bool GetKeyDataNB(KeyData* pKeyData) = 0;
};

//------------------------------------------------------------------------------
// KeyboardRepeatable
//------------------------------------------------------------------------------
class KeyboardRepeatable : public Keyboard {
public:
	class Repeat : public Tickable {
	private:
		Keyboard& keyboard_;
		uint8_t modifier_;
		uint8_t keyCode_;
		bool readyFlag_;
		uint32_t msecDelay_;
		uint32_t msecRate_;
	public:
		Repeat(Keyboard& keyboard);
		void SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) { msecDelay_ = msecDelay, msecRate_ = msecRate; }
		void Invalidate() { modifier_ = 0, keyCode_ = 0, readyFlag_ = false; }
		bool SignalFirst(uint8_t keyCode, uint8_t modifier);
		bool GetKey(uint8_t* pKeyCode, uint8_t* pModifier);
	public:
		// virtual function of Tickable
		virtual void OnTick() override;
	};
private:
	Repeat repeat_;
public:
	KeyboardRepeatable();
public:
	Repeat& GetRepeat() { return repeat_; }
public:
	// virtual function of Keyboard
	virtual Keyboard& SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) override;
	virtual bool GetKeyDataNB(KeyData* pKeyData) override;
};
	
//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
class KeyboardDumb : public Keyboard {
public:
	static KeyboardDumb Instance;
public:
	virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1) override { return 0; }
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override { return 0; }
	virtual bool GetKeyDataNB(KeyData* pKeyData) override { return false; }
};
	
}

#endif
