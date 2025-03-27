#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::KeyboardMatrix keyboard;

int main()
{
	const uint8_t keyCodeTbl[] = {
		VK_1, VK_2, VK_3, VK_A,
		VK_4, VK_5, VK_6, VK_B,
		VK_7, VK_8, VK_9, VK_C,
		VK_OEM_COMMA, VK_0, VK_OEM_PERIOD, VK_D,
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	::stdio_init_all();
	keyboard.Initialize(keyCodeTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	for (;;) {
		Tickable::Sleep(100);
	}
}
