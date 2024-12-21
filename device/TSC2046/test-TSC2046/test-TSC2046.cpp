#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TSC2046.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	TSC2046 touch(spi0, GPIO6, GPIO7);
	for (;;) {
		::printf("%d\n", touch.IsTouched(), touch.ReadX(), touch.ReadY());
	}
}
