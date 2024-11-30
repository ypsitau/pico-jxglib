#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	while (true) {
		UART::Default.Printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
