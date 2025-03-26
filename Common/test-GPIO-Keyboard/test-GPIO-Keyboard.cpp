#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

GPIO::Keyboard keyboard;

int main()
{
	::stdio_init_all();
	GPIO::Key keyLeft(GPIO10, VK_LEFT);
	GPIO::Key keyUp(GPIO11, VK_UP);
	GPIO::Key keyDown(GPIO12, VK_DOWN);
	GPIO::Key keyRight(GPIO13, VK_RIGHT);
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
