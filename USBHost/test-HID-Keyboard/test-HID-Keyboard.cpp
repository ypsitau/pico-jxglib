#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	::printf("----\n");
	for (;;) {
		KeyData keyDataTbl[6];
		int nKeys = USBHost::GetKeyboard().SenseKeyData(keyDataTbl, count_of(keyDataTbl));
		//int nKeys = USBHost::GetKeyboard().GetKeyData(keyDataTbl[0])? 1 : 0;
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				const KeyData& keyData = keyDataTbl[i];
				if (i > 0) ::printf(" ");
				if (keyData.IsKeyCode()) {
					::printf("VK_%s", keyData.GetKeyCodeName());
				} else if (keyData.GetCharCode() <= 'Z' - '@') {
					::printf("Ctrl+%c", keyData.GetCharCode() + '@');
				} else {
					::printf("'%c'", keyData.GetCharCode());
				}
			}
			::printf("\n");
		}
		Tickable::Tick();
	}
}
