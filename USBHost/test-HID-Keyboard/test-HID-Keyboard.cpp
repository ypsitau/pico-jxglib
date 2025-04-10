#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/KeyboardTest.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	Keyboard& keyboard = USBHost::GetKeyboard();
	keyboard.SetKeyLayout(KeyLayout_106::Instance).SetCapsLockAsCtrl();
	//KeyboardTest::GetKeyDataNB(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Instance, keyboard);
	KeyboardTest::SenseKeyCode(Stdio::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Instance, keyboard);
}
