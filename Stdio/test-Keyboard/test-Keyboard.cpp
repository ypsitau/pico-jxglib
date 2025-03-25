#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Keyboard& keyboard = Stdio::Keyboard::Instance;
	::printf("Press any key..\n");
	for (;;) {
		KeyData keyData;
		keyboard.GetKeyData(&keyData);
		if (keyData.IsKeyCode()) {
			::printf("VK_%s\n", GetKeyCodeName(keyData.GetKeyCode()));
		} else if (keyData.GetChar() <= 'Z' - '@') {
			::printf("Ctrl+%c\n", keyData.GetChar() + '@');
		} else {
			::printf("'%c'\n", keyData.GetChar());
		}
	}
}
