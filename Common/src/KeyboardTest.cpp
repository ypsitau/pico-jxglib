//==============================================================================
// KeyboardTest.cpp
//==============================================================================
#include "jxglib/KeyboardTest.h"

namespace jxglib::KeyboardTest {

const char* TextizeKeyData(char* str, int len, const KeyData& keyData)
{
	if (keyData.IsKeyCode()) {
		::snprintf(str, len, "VK_%s", GetKeyCodeName(keyData.GetKeyCode()));
	} else if (keyData.GetChar() <= 'Z' - '@') {
		::snprintf(str, len, "Ctrl+%c", keyData.GetChar() + '@');
	} else {
		::snprintf(str, len, "'%c'", keyData.GetChar());
	}
	return str;
}

void GetKeyDataNB(Keyboard& keyboard)
{
	char str[64];
	::printf("Test for Keyboard::GetKeyDataNB(). Press any keys..\n");
	for (;;) {
		KeyData keyData;
		if (keyboard.GetKeyDataNB(&keyData)) {
			::printf("%s\n", TextizeKeyData(str, sizeof(str), keyData));
		}
		Tickable::Tick();
	}
}

void SenseKeyData(Keyboard& keyboard)
{
	char str[64];
	::printf("Test for Keyboard::SenseKeyData(). Press any keys..\n");
	for (;;) {
		KeyData keyDataTbl[6];
		int nKeys = keyboard.SenseKeyData(keyDataTbl, count_of(keyDataTbl));
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				const KeyData& keyData = keyDataTbl[i];
				::printf((i == 0)? "%s" : " %s", TextizeKeyData(str, sizeof(str), keyData));
			}
			::printf("\n");
		}
		Tickable::Sleep(10);
	}
}
	
}
