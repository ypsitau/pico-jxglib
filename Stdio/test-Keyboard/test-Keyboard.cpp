#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/KeyboardTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Printable& printable = Stdio::GetPrintable();
	Keyboard& keyboard = Stdio::GetKeyboard();
	//KeyboardTest::GetKeyDataNB(printable, keyboard);
	KeyboardTest::SenseKeyData(printable, keyboard);
}
