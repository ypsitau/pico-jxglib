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
	bool GetKeyCode(uint8_t* pKeyCode, uint8_t* pModifier = nullptr);
	bool GetKeyData(KeyData* pKeyData);
	char GetChar();
public:
	virtual Keyboard& SetCapsLockAsCtrl(bool capsLockAsCtrlFlag = true) { return *this; }
	virtual Keyboard& SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) { return *this; }
	virtual uint8_t GetModifier() { return 0; }
	virtual bool IsPressed(uint8_t keyCode, bool includeModifiers = false);
	virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) = 0;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) = 0;
	virtual bool GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier = nullptr) = 0;
	virtual bool GetKeyDataNB(KeyData* pKeyData) = 0;
};

//------------------------------------------------------------------------------
// KeyboardRepeatable
//------------------------------------------------------------------------------
class KeyboardRepeatable : public Keyboard {
public:
	class Repeater : public Tickable {
	private:
		Keyboard& keyboard_;
		uint8_t modifier_;
		uint8_t keyCode_;
		bool readyFlag_;
		uint32_t msecDelay_;
		uint32_t msecRate_;
	public:
		Repeater(Keyboard& keyboard);
		void SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) { msecDelay_ = msecDelay, msecRate_ = msecRate; }
		void Invalidate() { modifier_ = 0, keyCode_ = 0, readyFlag_ = false; }
		bool SignalFirst(uint8_t keyCode, uint8_t modifier);
		bool GetKey(uint8_t* pKeyCode, uint8_t* pModifier);
	public:
		// virtual functions of Tickable
		virtual const char* GetTickableName() const override { return "KeyboardRepeatable::Repeater"; }
		virtual void OnTick() override;
	};
private:
	Repeater repeater_;
public:
	KeyboardRepeatable();
public:
	Repeater& GetRepeater() { return repeater_; }
public:
	// virtual function of Keyboard
	virtual Keyboard& SetRepeatTime(uint32_t msecDelay, uint32_t msecRate) override;
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override;
	virtual bool GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier = nullptr) override;
	virtual bool GetKeyDataNB(KeyData* pKeyData) override;
};
	
//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
class KeyboardDumb : public Keyboard {
public:
	static KeyboardDumb Instance;
public:
	virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) override { return 0; }
	virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override { return 0; }
	virtual bool GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier = nullptr) override { return false; }
	virtual bool GetKeyDataNB(KeyData* pKeyData) override { return false; }
};
	
}

#endif
