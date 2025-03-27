#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::Keyboard keyboard;

int main()
{
	::stdio_init_all();
	GPIO::Key keyTbl[] = {
		{GPIO16.pull_up(),	VK_Z,		},
		{GPIO17.pull_up(),	VK_X,		},
		{GPIO18.pull_up(),	VK_LEFT,	},
		{GPIO19.pull_up(),	VK_UP,		},
		{GPIO20.pull_up(),	VK_DOWN,	},
		{GPIO21.pull_up(),	VK_RIGHT,	},
	};
	keyboard.Initialize(keyTbl, count_of(keyTbl));
	for (;;) {
#if 0
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
#else
		uint8_t keyCodeTbl[6];
		int nKeys = keyboard.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
		if (nKeys > 0) {
			for (int i = 0; i < nKeys; i++) {
				if (i > 0) ::printf(" ");
				::printf("%s", GetKeyCodeName(keyCodeTbl[i]));
			}
			::printf("\n");
		}
#endif
		Tickable::Sleep(100);
	}
}
