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

void GetKeyDataNB(Printable& printable, Keyboard& keyboard)
{
	char str[64];
	printable.Printf("Test for Keyboard::GetKeyDataNB(). Press any keys..\n");
	for (;;) {
		KeyData keyData;
		if (keyboard.GetKeyDataNB(&keyData)) {
			printable.Printf("%s\n", TextizeKeyData(str, sizeof(str), keyData));
		}
		Tickable::Tick();
	}
}

void SenseKeyData(Printable& printable, Keyboard& keyboard)
{
	char str[64];
	printable.Printf("Test for Keyboard::SenseKeyData(). Press any keys..\n");
	for (;;) {
		KeyData keyDataTbl[6];
		int nKeys = keyboard.SenseKeyData(keyDataTbl, count_of(keyDataTbl));
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				const KeyData& keyData = keyDataTbl[i];
				printable.Printf((i == 0)? "%s" : " %s", TextizeKeyData(str, sizeof(str), keyData));
			}
			printable.Printf("\n");
		}
		Tickable::Sleep(10);
	}
}
	
void SenseKeyCode(Printable& printable, Keyboard& keyboard)
{
	printable.Printf("Test for Keyboard::SenseKeyCode(). Press any keys..\n");
	for (;;) {
		uint8_t keyCodeTbl[6];
		int nKeys = keyboard.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				printable.Printf((i == 0)? "%s" : " %s", GetKeyCodeName(keyCodeTbl[i]));
			}
			printable.Printf("\n");
		}
		Tickable::Sleep(10);
	}
}

void IsPressed(Printable& printable, Keyboard& keyboard)
{
	for (;;) {
		printable.Printf("VK_1:%d VK_2:%d VK_3:%d VK_4:%d VK_5:%d VK_6:%d\n",
			keyboard.IsPressed(VK_1), keyboard.IsPressed(VK_2), keyboard.IsPressed(VK_3),
			keyboard.IsPressed(VK_4), keyboard.IsPressed(VK_5), keyboard.IsPressed(VK_6));
		Tickable::Sleep(10);
	}
}

}
