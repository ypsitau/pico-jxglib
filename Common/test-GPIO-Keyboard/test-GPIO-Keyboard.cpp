#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/KeyboardTest.h"

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
	KeyboardTest::GetKeyDataNB(keyboard);
	//KeyboardTest::SenseKeyData(keyboard);
}
