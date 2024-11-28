#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//UART uart(uart_default);
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
