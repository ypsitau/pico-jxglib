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
				::printf("%02x\n", keyData.GetKeyCode());
			} else {
				::printf("'%c'\n", keyData.GetCharCode());
			}
		}
		Tickable::Sleep(100);
	}
}
