//==============================================================================
// GPIO.cpp
//==============================================================================
#include <memory.h>
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
GPIO::Key::Key(const GPIO& gpio, uint32_t flags, uint8_t keyCode, uint8_t modifier) :
	gpio_{gpio}, flags_{flags}, keyCode_{keyCode}, modifier_{modifier}, pressedFlag_{false}
{
}

void GPIO::Key::Initialize()
{
	gpio_.set_dir_IN().put(0).set_function_SIO();
}

//------------------------------------------------------------------------------
// GPIO::KeySet
//------------------------------------------------------------------------------
GPIO::KeySet::KeySet(uint8_t keyCode, uint8_t modifier) : keyCode_{keyCode}, modifier_{modifier}
{
}

//------------------------------------------------------------------------------
// GPIO::KeyRow
//------------------------------------------------------------------------------
GPIO::KeyRow::KeyRow(const GPIO& gpio) : gpio_{gpio}
{
}

//------------------------------------------------------------------------------
// GPIO::KeyCol
//------------------------------------------------------------------------------
GPIO::KeyCol::KeyCol(const GPIO& gpio) : gpio_{gpio}
{
}

//------------------------------------------------------------------------------
// GPIO::Keyboard
//------------------------------------------------------------------------------
GPIO::Keyboard::Keyboard(int msecTick) : Tickable(msecTick, Tickable::Priority::Lowest), keyTbl_{nullptr}, nKeys_{0}
{
}

void GPIO::Keyboard::Initialize(Key* keyTbl, int nKeys)
{
	keyTbl_ = keyTbl, nKeys_ = nKeys;
	for (int i = 0; i < nKeys_; i++) keyTbl_[i].Initialize();
}

bool GPIO::Keyboard::IsPressed(uint8_t keyCode)
{
	for (int i = 0; i < nKeys_; i++) {
		Key& key = keyTbl_[i];
		if (key.IsPressed() && keyCode == key.GetKeyCode()) return true;
	}
	return false;
}

int GPIO::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
{
	int nKeys = 0;
	for (int i = 0; i < nKeys_; i++) {
		Key& key = keyTbl_[i];
		uint8_t keyCode = key.GetKeyCode();
		if (key.IsPressed() && keyCode) keyCodeTbl[nKeys++] = keyCode;
	}
	return nKeys;
}

void GPIO::Keyboard::OnTick()
{
	bool anyPressedFlag = false;
	for (int i = 0; i < nKeys_; i++) {
		Key& key = keyTbl_[i];
		key.Update();
		if (key.IsPressed()) {
			anyPressedFlag = true;
			if (GetRepeater().SignalFirst(key.GetKeyCode(), key.GetModifier())) break;
		}
	}
	if (!anyPressedFlag) GetRepeater().Invalidate();
}

//------------------------------------------------------------------------------
// GPIO::KeyboardMatrix
//------------------------------------------------------------------------------
GPIO::KeyboardMatrix::KeyboardMatrix(int msecTick) : Tickable(msecTick, Tickable::Priority::Lowest),
	keySetTbl_{nullptr}, keyRowTbl_{nullptr}, nKeyRows_{0}, keyColTbl_{nullptr}, nKeyCols_{0},
	iKeyRow_{0}, nKeysScanned_{0}
{
}

void GPIO::KeyboardMatrix::Initialize(const KeySet* keySetTbl,
	const KeyRow* keyRowTbl, int nKeyRows, const KeyCol* keyColTbl, int nKeyCols, uint32_t flags)
{
	keySetTbl_ = keySetTbl;
	keyRowTbl_ = keyRowTbl, nKeyRows_ = nKeyRows;
	keyColTbl_ = keyColTbl, nKeyCols_ = nKeyCols;
	if (flags & LogicNeg) {
		valueActive_ = false, valueInactive_ = true;
	} else {
		valueActive_ = true, valueInactive_ = false;
	}
	for (int iKeyRow = 0; iKeyRow < nKeyRows_; iKeyRow++) {
		keyRowTbl_[iKeyRow].GetGPIO().set_dir_OUT().put(valueInactive_).set_function_SIO();
	}
	for (int iKeyCol = 0; iKeyCol < nKeyCols_; iKeyCol++) {
		keyColTbl_[iKeyCol].GetGPIO().set_dir_IN().put(0).set_function_SIO();
	}
	::memset(keyCodeCapturedTbl_, 0x00, sizeof(keyCodeCapturedTbl_));
	::memset(keyCodeScannedTbl_, 0x00, sizeof(keyCodeScannedTbl_));
	iKeyRow_ = 0;
	nKeysScanned_ = 0;
	keyRowTbl_[iKeyRow_].put(valueActive_);
}

int GPIO::KeyboardMatrix::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
{
	nKeysMax = ChooseMin(nKeysMax, static_cast<int>(count_of(keyCodeCapturedTbl_)));
	int nKeys = 0;
	for ( ; nKeys < nKeysMax; nKeys++) {
		uint8_t keyCode = keyCodeCapturedTbl_[nKeys];
		if (!keyCode) break;
		keyCodeTbl[nKeys] = keyCode;
	}
	return nKeys;
}

void GPIO::KeyboardMatrix::OnTick()
{
	for (int iKeyCol = 0; iKeyCol < nKeyCols_; iKeyCol++) {
		if (nKeysScanned_ < count_of(keyCodeScannedTbl_) && keyColTbl_[iKeyCol].get() == valueActive_) {
			keyCodeScannedTbl_[nKeysScanned_++] = keySetTbl_[iKeyCol + iKeyRow_ * nKeyCols_].GetKeyCode();
		}
	}
	keyRowTbl_[iKeyRow_].put(valueInactive_);
	iKeyRow_++;
	if (iKeyRow_ == nKeyRows_) {
		::memcpy(keyCodeCapturedTbl_, keyCodeScannedTbl_, sizeof(keyCodeCapturedTbl_));
		::memset(keyCodeScannedTbl_, 0x00, sizeof(keyCodeScannedTbl_));
		iKeyRow_ = 0;
		nKeysScanned_ = 0;
		if (keyCodeCapturedTbl_[0] == 0) {
			GetRepeater().Invalidate();
		} else {
			for (int i = 0; i < count_of(keyCodeCapturedTbl_); i++) {
				uint8_t keyCode = keyCodeCapturedTbl_[i];
				if (!keyCode) break;
				const uint8_t modifier = 0;
				if (GetRepeater().SignalFirst(keyCode, modifier)) break;
			}
		}
	}
	keyRowTbl_[iKeyRow_].put(valueActive_);
}

}
