//==============================================================================
// Keyboard.cpp
//==============================================================================
#include "jxglib/Common.h"
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

bool Keyboard::GetKeyCode(uint8_t* pKeyCode, uint8_t* pModifier)
{
	for (;;) {
		if (GetKeyCodeNB(pKeyCode, pModifier)) break;
		Tickable::Tick();
	}
	return true;
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
KeyboardRepeatable::KeyboardRepeatable() : repeater_(*this)
{
}

Keyboard& KeyboardRepeatable::SetRepeatTime(uint32_t msecDelay, uint32_t msecRate)
{
	repeater_.SetRepeatTime(msecDelay, msecRate);
	return *this;
}

int KeyboardRepeatable::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	uint8_t keyCodeTbl[6];
	int nKeys = 0;
	int nKeysIn = SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
	for (int i = 0; i < ChooseMin(nKeysIn, nKeysMax); i++) {
		KeyData keyData = GetKeyLayout().CreateKeyData(keyCodeTbl[i], GetModifier());
		if (keyData.IsValid()) keyDataTbl[nKeys++] = keyData;
	}
	return nKeys;
}

bool KeyboardRepeatable::GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier)
{
	uint8_t keyCode, modifier;
	*pKeyCode = 0;
	if (pModifier) *pModifier = 0;
	if (GetRepeater().GetKey(&keyCode, &modifier)) {
		*pKeyCode = keyCode;
		if (pModifier) *pModifier = modifier;
		return true;
	}
	return false;
}

bool KeyboardRepeatable::GetKeyDataNB(KeyData* pKeyData)
{
	uint8_t keyCode, modifier;
	if (GetRepeater().GetKey(&keyCode, &modifier)) {
		*pKeyData = GetKeyLayout().CreateKeyData(keyCode, modifier);
		return pKeyData->IsValid();
	}
	return false;
}

//------------------------------------------------------------------------------
// KeyboardRepeatable::Repeater
//------------------------------------------------------------------------------
KeyboardRepeatable::Repeater::Repeater(Keyboard& keyboard) : Tickable(-1, Tickable::Priority::Lowest),
	keyboard_{keyboard}, modifier_{0}, keyCode_{0}, readyFlag_{false}, msecDelay_{400}, msecRate_{100}
{
	Suspend();
}

bool KeyboardRepeatable::Repeater::SignalFirst(uint8_t keyCode, uint8_t modifier)
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

bool KeyboardRepeatable::Repeater::GetKey(uint8_t* pKeyCode, uint8_t* pModifier)
{
	if (readyFlag_) {
		readyFlag_ = false;
		*pKeyCode = keyCode_;
		*pModifier = modifier_;
		return true;
	}
	return false;
}

void KeyboardRepeatable::Repeater::OnTick()
{
	uint8_t keyCodeTbl[6];
	keyboard_.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
	for (int i = 0; i < count_of(keyCodeTbl) && keyCodeTbl[i]; i++) {
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
