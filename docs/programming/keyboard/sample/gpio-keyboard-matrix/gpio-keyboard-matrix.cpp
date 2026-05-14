#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
	static const Keyboard::KeySet keySetTbl[] = {
		VK_1,    VK_2, VK_3,     VK_BACK,
		VK_4,    VK_5, VK_6,     VK_UP,
		VK_7,    VK_8, VK_9,     VK_DOWN,
		VK_LEFT, VK_0, VK_RIGHT, VK_RETURN, //{0, Keyboard::Mod::ShiftL},
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
    GPIO::KeyboardMatrix keyboard;
	keyboard.Initialize(keySetTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
    for (;;) {
        KeyData keyData;
        if (keyboard.GetKeyDataNB(&keyData)) {
            char ch = keyData.GetChar();
            if (ch) {
                ::printf("Char:    '%c'\n", ch);
            } else {
                ::printf("KeyCode: 0x%02x (modifier: %03b)\n", keyData.GetKeyCode(), keyData.GetModifier());
            }
        }
        Tickable::Sleep(100);
    }
}
