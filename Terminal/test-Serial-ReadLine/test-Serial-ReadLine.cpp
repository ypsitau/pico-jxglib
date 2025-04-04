#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

GPIO::KeyboardMatrix keyboard;

TickableEntry(hoge, 300)
{
	static bool value = false;
	value = !value;
	GPIO2.put(value);
}

int main()
{
	::stdio_init_all();
	const GPIO::KeySet keySetTbl[] = {
		VK_1, VK_2, VK_3, VK_BACK,
		VK_4, VK_5, VK_6, VK_LEFT,
		VK_7, VK_8, VK_9, VK_RIGHT,
		VK_OEM_COMMA, VK_0, VK_OEM_PERIOD, VK_RETURN,
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	keyboard.Initialize(keySetTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	GPIO2.init().set_dir_OUT();
	terminal.Initialize();
	//terminal.AttachKeyboard(keyboard);
	terminal.AttachPrintable(Stdio::Printable::Instance).AttachKeyboard(Stdio::GetKeyboard());
	terminal.Println("ReadLine Test Program");
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
		//::printf("%d\n", terminal.GetChar());
	}
}
