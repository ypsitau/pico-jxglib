#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Keyboard.h"

using namespace jxglib;

int main()
{
	Keyboard& keyboard = KeyboardStdio::Instance;
	::stdio_init_all();
	GPIO2.init().set_dir_OUT();
	::printf("Press any keys\n");
	for (;;) {
		::printf("%d\n", keyboard.GetChar());
	}
}
