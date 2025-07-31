#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

int main(void)
{
	stdio_init_all();
	jxglib_labo_init();
	for (;;) jxglib_tick();
}
