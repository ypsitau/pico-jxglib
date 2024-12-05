#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIOBox pioBox(PIOVAR_program(blink), PIOVAR_get_default_config(blink));
	pioBox.SetGPIO_out(GPIO3);
	pioBox.SetGPIO_in(GPIO4);
	pioBox.SetGPIO_set(GPIO5);
	pioBox.SetGPIO_sideset(GPIO6);
	pioBox.ClaimResource();
	pioBox.Init();
	pioBox.sm.put(0);
	pioBox.sm.get();
	pioBox.sm.is_rx_fifo_empty();
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
