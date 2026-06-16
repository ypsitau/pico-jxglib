#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

//GPIO::Keyboard keyboard;

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
	//GPIO::Key keyTbl[] = {
	//	{ GPIO10.pull_up(), GPIO::LogicNeg, VK_LEFT		},
	//	{ GPIO11.pull_up(), GPIO::LogicNeg, VK_UP		},
	//	{ GPIO12.pull_up(), GPIO::LogicNeg, VK_DOWN		},
	//	{ GPIO13.pull_up(), GPIO::LogicNeg, VK_RIGHT	},
	//	{ GPIO14.pull_up(), GPIO::LogicNeg, {VK_Z, Keyboard::Mod::ShiftL} },
	//	{ GPIO15.pull_up(), GPIO::LogicNeg, VK_X		},
	//};
	//keyboard.Initialize(keyTbl, count_of(keyTbl));
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal);
	//KeyboardTest::GetKeyDataNB(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyCode(Stdio::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Instance, keyboard);
	for (;;) {
		Tickable::Tick();
	}
}
