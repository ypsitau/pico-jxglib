#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/JSON.h"

int main()
{
	::stdio_init_all();

	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
