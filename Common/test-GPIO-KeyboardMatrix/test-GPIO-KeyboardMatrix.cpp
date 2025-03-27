#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::KeyboardMatrix keyboard;

int main()
{
	::stdio_init_all();
	keyboard.Initialize();
	for (;;) {
		Tickable::Sleep(100);
	}
}
