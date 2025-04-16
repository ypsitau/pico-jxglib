#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/KeyboardTest.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

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

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	//KeyboardTest::GetKeyDataNB(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyCode(Stdio::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Instance, keyboard);
	Printable& printable = Stdio::Instance;
	char str[64];
	printable.Printf("Test for Keyboard::SenseKeyData(). Press any keys..\n");
	for (;;) {
		Keyboard& keyboard = USBHost::GetKeyboard();
		keyboard.SetKeyLayout(KeyLayout_106::Instance).SetCapsLockAsCtrl();
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
