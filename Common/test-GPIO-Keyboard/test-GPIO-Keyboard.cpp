#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::Key keyA(GPIO16, 'Z', GPIO::Key::PullUp);
GPIO::Key keyB(GPIO17, 'X', GPIO::Key::PullUp);
GPIO::Key keyLeft(GPIO18, VK_LEFT, GPIO::Key::PullUp);
GPIO::Key keyUp(GPIO19, VK_UP, GPIO::Key::PullUp);
GPIO::Key keyDown(GPIO20, VK_DOWN, GPIO::Key::PullUp);
GPIO::Key keyRight(GPIO21, VK_RIGHT, GPIO::Key::PullUp);

GPIO::Keyboard keyboard;

int main()
{
	uint8_t keyCodeTbl[6];
	::stdio_init_all();
	keyboard.Initialize();
	while (true) {
		KeyData keyData;
		if (keyboard.GetKeyData(&keyData)) {
			if (keyData.IsKeyCode()) {
				::printf("VK_%s", GetKeyCodeName(keyData.GetKeyCode()));
			} else if (keyData.GetChar() <= 'Z' - '@') {
				::printf("Ctrl+%c", keyData.GetChar() + '@');
			} else {
				::printf("'%c'", keyData.GetChar());
			}
			::printf("\n");
		}
		//int nKeys = keyboard.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
		//if (nKeys > 0) {
		//	for (int i = 0; i < nKeys; i++) {
		//		if (i > 0) ::printf(" ");
		//		::printf("%s", GetKeyCodeName(keyCodeTbl[i]));
		//	}
		//	::printf("\n");
		//}
		Tickable::Sleep(100);
	}
}
