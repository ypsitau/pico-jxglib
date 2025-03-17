#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost host;
	host.Initialize();
	::printf("----\n");
	for (;;) {
		KeyData keyData;
		if (USBHost::GetKeyboard().GetKeyData(keyData)) {
			if (keyData.IsKeyCode()) {
				::printf("VK_%s\n", keyData.GetKeyCodeName());
			} else if (keyData.GetCharCode() <= 'Z' - '@') {
				::printf("Ctrl+%c\n", keyData.GetCharCode() + '@');
			} else {
				::printf("'%c'\n", keyData.GetCharCode());
			}
		}
		Tickable::Tick();
	}
}
