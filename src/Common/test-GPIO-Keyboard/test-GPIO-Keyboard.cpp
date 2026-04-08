#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/KeyData.h"
#include "jxglib/KeyboardTest.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

GPIO::Keyboard keyboard;

int main()
{
	::stdio_init_all();
	GPIO::Key keyTbl[] = {
		{ GPIO16.pull_up(), GPIO::LogicNeg, {VK_Z, Keyboard::Mod::ShiftL} },
		{ GPIO17.pull_up(), GPIO::LogicNeg, VK_X		},
		{ GPIO18.pull_up(), GPIO::LogicNeg, VK_LEFT		},
		{ GPIO19.pull_up(), GPIO::LogicNeg, VK_UP		},
		{ GPIO20.pull_up(), GPIO::LogicNeg, VK_DOWN		},
		{ GPIO21.pull_up(), GPIO::LogicNeg, VK_RIGHT	},
	};
	keyboard.Initialize(keyTbl, count_of(keyTbl));
	KeyboardTest::GetKeyDataNB(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyData(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyCode(Stdio::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Instance, keyboard);
}
