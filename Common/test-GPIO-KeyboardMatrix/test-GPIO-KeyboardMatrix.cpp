#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::KeyboardMatrix keyboard;

void Test_GetKeyDataNB(Keyboard& keyboard)
{
	for (;;) {
		KeyData keyData;
		if (keyboard.GetKeyDataNB(&keyData)) {
			if (keyData.IsKeyCode()) {
				::printf("VK_%s\n", GetKeyCodeName(keyData.GetKeyCode()));
			} else if (keyData.GetChar() <= 'Z' - '@') {
				::printf("Ctrl+%c\n", keyData.GetChar() + '@');
			} else {
				::printf("'%c'\n", keyData.GetChar());
			}
		}
		Tickable::Tick();
	}
}

void Test_SenseKeyData(Keyboard& keyboard)
{
	for (;;) {
		KeyData keyDataTbl[6];
		int nKeys = keyboard.SenseKeyData(keyDataTbl, count_of(keyDataTbl));
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				const KeyData& keyData = keyDataTbl[i];
				if (i > 0) ::printf(" ");
				if (keyData.IsKeyCode()) {
					::printf("VK_%s", GetKeyCodeName(keyData.GetKeyCode()));
				} else if (keyData.GetChar() <= 'Z' - '@') {
					::printf("Ctrl+%c", keyData.GetChar() + '@');
				} else {
					::printf("'%c'", keyData.GetChar());
				}
			}
			::printf("\n");
		}
		Tickable::Sleep(100);
	}
}

int main()
{
	::stdio_init_all();
	static const uint8_t keyCodeTbl[] = {
		VK_1, VK_2, VK_3, VK_A,
		VK_4, VK_5, VK_6, VK_B,
		VK_7, VK_8, VK_9, VK_C,
		VK_OEM_COMMA, VK_0, VK_OEM_PERIOD, VK_D,
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	keyboard.Initialize(keyCodeTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	//Test_GetKeyDataNB(keyboard);
	Test_SenseKeyData(keyboard);
}
