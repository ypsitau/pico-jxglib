#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/KeyboardTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Keyboard& keyboard = Stdio::Keyboard::Instance;
	//KeyboardTest::GetKeyDataNB(keyboard);
	KeyboardTest::SenseKeyData(keyboard);
}
