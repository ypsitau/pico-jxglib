#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

int main()
{
	//channel_config_set_transfer_data_size();
	::stdio_init_all();

	while (true) {
		::printf("Hello, world!\n");
		::sleep_ms(1000);
	}
}
