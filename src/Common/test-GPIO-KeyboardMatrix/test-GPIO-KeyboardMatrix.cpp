#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

GPIO::KeyboardMatrix keyboard;

int main()
{
	::stdio_init_all();
	do {
		static const Keyboard::KeySet keySetTbl[] = {
			VK_1,    VK_2, VK_3,     VK_BACK,
			VK_4,    VK_5, VK_6,     VK_UP,
			VK_7,    VK_8, VK_9,     VK_DOWN,
			VK_LEFT, VK_0, VK_RIGHT, VK_RETURN, //{0, Keyboard::Mod::ShiftL},
		};
		static const GPIO::KeyRow keyOutTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
		static const GPIO::KeyCol keyInTbl[] = { GPIO20, GPIO21, GPIO26, GPIO27 };
		keyboard.Initialize(keySetTbl, keyOutTbl, count_of(keyOutTbl), keyInTbl, count_of(keyInTbl), GPIO::LogicNeg | GPIO::PullUp);
	} while (0);
	Serial::Terminal terminal;
	terminal.Initialize();
	Shell::AttachTerminal(terminal);
	for (;;) {
		Tickable::Tick();
	}
}
