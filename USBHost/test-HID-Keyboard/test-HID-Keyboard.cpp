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
	//KeyboardTest::GetKeyDataNB(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Printable::Instance, keyboard);
	KeyboardTest::SenseKeyCode(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Printable::Instance, keyboard);
}
