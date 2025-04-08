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

void GetKeyCodeNB(Printable& printable, Keyboard& keyboard)
{
	printable.Printf("Test for Keyboard::GetKeyCodeNB(). Press any keys..\n");
	for (;;) {
		uint8_t keyCode;
		if (keyboard.GetKeyCodeNB(&keyCode)) {
			printable.Printf("VK_%s\n", GetKeyCodeName(keyCode));
		}
		Tickable::Tick();
	}
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

void SenseKeyCode(Printable& printable, Keyboard& keyboard)
{
	printable.Printf("Test for Keyboard::SenseKeyCode(). Press any keys..\n");
	for (;;) {
		uint8_t keyCodeTbl[6];
		int nKeys = keyboard.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl), true);
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				printable.Printf((i == 0)? "VK_%s" : " VK_%s", GetKeyCodeName(keyCodeTbl[i]));
			}
			printable.Printf("\n");
		}
		Tickable::Sleep(10);
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
	
void IsPressed(Printable& printable, Keyboard& keyboard)
{
	bool includeModifiers = true;
	for (;;) {
		printable.Printf("VK_LEFT:%d VK_RIGHT:%d VK_UP:%d VK_DOWN:%d VK_Z:%d VK_X:%d\n",
			keyboard.IsPressed(VK_LEFT, includeModifiers), keyboard.IsPressed(VK_RIGHT, includeModifiers),
			keyboard.IsPressed(VK_UP, includeModifiers), keyboard.IsPressed(VK_DOWN, includeModifiers),
			keyboard.IsPressed(VK_Z, includeModifiers), keyboard.IsPressed(VK_X, includeModifiers));
		Tickable::Sleep(10);
	}
}

}
