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
		//int nKeys = USBHost::GetKeyboard().SenseKeyData(keyDataTbl, count_of(keyDataTbl));
		int nKeys = USBHost::GetKeyboard().GetKeyDataNB(&keyDataTbl[0])? 1 : 0;
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				const KeyData& keyData = keyDataTbl[i];
				if (i > 0) ::printf(" ");
				if (keyData.IsKeyCode()) {
					::printf("VK_%s", keyData.GetKeyCodeName());
				} else if (keyData.GetCharRaw() <= 'Z' - '@') {
					::printf("Ctrl+%c", keyData.GetCharRaw() + '@');
				} else {
					::printf("'%c'", keyData.GetCharRaw());
				}
			}
			::printf("\n");
		}
		Tickable::Tick();
	}
}
