//==============================================================================
// Keyboard.cpp
//==============================================================================
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------
Keyboard::Keyboard() : pKeyLayout_{&KeyLayout_106::Instance}
{}

char Keyboard::GetChar()
{
	KeyData keyData;
	for (;;) {
		if (GetKeyDataNB(&keyData) && keyData.IsChar()) break;
		Tickable::Tick();
	}
	return keyData.GetChar();
}

bool Keyboard::GetKeyData(KeyData* pKeyData)
{
	KeyData keyData;
	for (;;) {
		if (GetKeyDataNB(pKeyData)) break;
		Tickable::Tick();
	}
	return true;
}

bool Keyboard::IsPressed(uint8_t keyCode)
{
	uint8_t keyCodeTbl[6];
	int nKeys = SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
	for (int i = 0; i < nKeys; i++) if (keyCodeTbl[i] == keyCode) return true;
	return false;
}

//------------------------------------------------------------------------------
// KeyboardRepeatable
//------------------------------------------------------------------------------
KeyboardRepeatable::KeyboardRepeatable() : repeat_(*this)
{
}

Keyboard& KeyboardRepeatable::SetRepeatTime(uint32_t msecDelay, uint32_t msecRate)
{
	repeat_.SetRepeatTime(msecDelay, msecRate);
	return *this;
}

bool KeyboardRepeatable::GetKeyDataNB(KeyData* pKeyData)
{
	uint8_t keyCode, modifier;
	if (GetRepeat().GetKey(&keyCode, &modifier)) {
		*pKeyData = GetKeyLayout().CreateKeyData(keyCode, modifier);
		return pKeyData->IsValid();
	}
	return false;
}

//------------------------------------------------------------------------------
// KeyboardRepeatable::Repeat
//------------------------------------------------------------------------------
KeyboardRepeatable::Repeat::Repeat(Keyboard& keyboard) : Tickable(-1, Tickable::Priority::Lowest),
	keyboard_{keyboard}, modifier_{0}, keyCode_{0}, readyFlag_{false}, msecDelay_{400}, msecRate_{100}
{
	Suspend();
}

bool KeyboardRepeatable::Repeat::SignalFirst(uint8_t keyCode, uint8_t modifier)
{
	if (keyCode_ != keyCode) {
		modifier_ = modifier;
		keyCode_ = keyCode;
		readyFlag_ = true;
		ResetTick(msecDelay_);
		Resume();
		return true;
	}
	return false;
}

bool KeyboardRepeatable::Repeat::GetKey(uint8_t* pKeyCode, uint8_t* pModifier)
{
	if (readyFlag_) {
		readyFlag_ = false;
		*pKeyCode = keyCode_;
		*pModifier = modifier_;
		return true;
	}
	return false;
}

void KeyboardRepeatable::Repeat::OnTick()
{
	uint8_t keyCodeTbl[6];
	keyboard_.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
	for (int i = 0; i < count_of(keyCodeTbl); i++) {
		if (keyCode_ == keyCodeTbl[i]) {
			readyFlag_ = true;
			if (GetTick() != msecRate_) SetTick(msecRate_);
			return;
		}
	}
	Suspend();
}

//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
KeyboardDumb KeyboardDumb::Instance;

}
