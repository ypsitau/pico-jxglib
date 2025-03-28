#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/KeyboardTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	Keyboard& keyboard = USBHost::GetKeyboard();
	keyboard.SetKeyLayout(KeyLayout_106::Instance).SetCapsLockAsCtrl();
	KeyboardTest::GetKeyDataNB(keyboard);
	//KeyboardTest::SenseKeyData(keyboard);
}
