//==============================================================================
// GPIO.cpp
//==============================================================================
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// GPIO
//------------------------------------------------------------------------------
const GPIO GPIO::None(-1);
const GPIO0_T GPIO0;
const GPIO1_T GPIO1;
const GPIO2_T GPIO2;
const GPIO3_T GPIO3;
const GPIO4_T GPIO4;
const GPIO5_T GPIO5;
const GPIO6_T GPIO6;
const GPIO7_T GPIO7;
const GPIO8_T GPIO8;
const GPIO9_T GPIO9;
const GPIO10_T GPIO10;
const GPIO11_T GPIO11;
const GPIO12_T GPIO12;
const GPIO13_T GPIO13;
const GPIO14_T GPIO14;
const GPIO15_T GPIO15;
const GPIO16_T GPIO16;
const GPIO17_T GPIO17;
const GPIO18_T GPIO18;
const GPIO19_T GPIO19;
const GPIO20_T GPIO20;
const GPIO21_T GPIO21;
const GPIO22_T GPIO22;
const GPIO23_T GPIO23;
const GPIO24_T GPIO24;
const GPIO25_T GPIO25;
const GPIO26_T GPIO26;
const GPIO27_T GPIO27;
const GPIO28_T GPIO28;
const GPIO29_T GPIO29;

//------------------------------------------------------------------------------
// GPIO::Key
//------------------------------------------------------------------------------
GPIO::Key* GPIO::Key::pKeyHead_ = nullptr;

GPIO::Key::Key(const GPIO& gpio, uint8_t keyCode, uint32_t flags) :
		gpio_{gpio}, keyCode_{keyCode}, flags_{flags}, pressedFlag_{false}, pKeyNext_{pKeyHead_}
{
	pKeyHead_ = this;
}

void GPIO::Key::Initialize()
{
	gpio_.init().set_dir_IN();
	if (flags_ & PullUp) gpio_.pull_up();
	if (flags_ & PullDown) gpio_.pull_down();
}

//------------------------------------------------------------------------------
// GPIO::Keyboard
//------------------------------------------------------------------------------
GPIO::Keyboard::Keyboard() : Tickable(20, Tickable::Priority::Lowest)
{
}

void GPIO::Keyboard::Initialize()
{
	for (Key* pKey = Key::GetHead(); pKey; pKey = pKey->GetNext()) pKey->Initialize();
}

bool GPIO::Keyboard::IsPressed(uint8_t keyCode)
{
	for (Key* pKey = Key::GetHead(); pKey; pKey = pKey->GetNext()) {
		if (pKey->IsPressed() && keyCode == pKey->GetKeyCode()) return true;
	}
	return false;
}

int GPIO::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
{
	int nKeys = 0;
	for (Key* pKey = Key::GetHead(); pKey && nKeys < nKeysMax; pKey = pKey->GetNext()) {
		uint8_t keyCode = pKey->GetKeyCode();
		if (pKey->IsPressed() && keyCode) keyCodeTbl[nKeys++] = keyCode;
	}
	return nKeys;
}

void GPIO::Keyboard::OnTick()
{
	uint8_t modifier = 0;
	bool anyPressedFlag = false;
	for (Key* pKey = Key::GetHead(); pKey; pKey = pKey->GetNext()) {
		pKey->Update();
		if (pKey->IsPressed()) {
			anyPressedFlag = true;
			if (GetRepeat().SignalFirst(pKey->GetKeyCode(), modifier)) break;
		}
	}
	if (!anyPressedFlag) GetRepeat().Invalidate();
}

}
