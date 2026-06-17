#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

GPIO::Keyboard keyboard;

//TickableEntry(KeyMonitor)
//{
//	KeyData keyData;
//	if (keyboard.GetKeyDataNB(&keyData)) {
//		char str[64];
//		::printf("%s\n", keyData.Textize(str, sizeof(str)));
//	}
//}

int main()
{
	::stdio_init_all();
	do {
		static const GPIO::Key keyTbl[] = {
			{ GPIO10, GPIO::LogicNeg | GPIO::PullUp, VK_LEFT		},
			{ GPIO11, GPIO::LogicNeg | GPIO::PullUp, VK_UP			},
			{ GPIO12, GPIO::LogicNeg | GPIO::PullUp, VK_DOWN		},
			{ GPIO13, GPIO::LogicNeg | GPIO::PullUp, VK_RIGHT		},
			{ GPIO14, GPIO::LogicNeg | GPIO::PullUp, {VK_Z, Keyboard::Mod::ShiftL} },
			{ GPIO15, GPIO::LogicNeg | GPIO::PullUp, VK_X			},
		};
		keyboard.Initialize(keyTbl, count_of(keyTbl));
	} while (0);
	Serial::Terminal terminal;
	terminal.Initialize();
	Shell::AttachTerminal(terminal);
	for (;;) {
		Tickable::Tick();
	}
}
