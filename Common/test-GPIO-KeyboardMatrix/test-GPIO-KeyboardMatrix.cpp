#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/KeyboardTest.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

GPIO::KeyboardMatrix keyboard;

int main()
{
	::stdio_init_all();
	const GPIO::KeySet keySetTbl[] = {
		VK_1, VK_2, VK_3, VK_UP,
		VK_4, VK_5, VK_6, VK_DOWN,
		VK_7, VK_8, VK_9, VK_LEFT,
		VK_Z, VK_0, VK_X, VK_RIGHT, //{0, KeyData::Mod::ShiftL}
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	//const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_down(), GPIO21.pull_down(), GPIO26.pull_down(), GPIO27.pull_down() };
	keyboard.Initialize(keySetTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	KeyboardTest::GetKeyCodeNB(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::GetKeyDataNB(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::SenseKeyCode(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Printable::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Printable::Instance, keyboard);
}
