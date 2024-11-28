#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::adc_init();
	GPIO26.adc_init();
	GPIO27.adc_init();
	GPIO28.adc_init();
	GPIO29.adc_init();
	//GPIO26.adc_select_input();
	//GPIO27.adc_select_input();
	//GPIO28.adc_select_input();
	//GPIO29.adc_select_input();
	//GPIO::adc_set_round_robin(GPIO26, GPIO27, GPIO28, GPIO29, true);
	//int i = 0;
	//::adc_run(true);
	for (;;) {
		::printf("%04x %04x %04x %04x\n",
			GPIO26.adc_read(), GPIO27.adc_read(), GPIO28.adc_read(), GPIO29.adc_read());
		//::printf("%04x ", ::adc_read());
		//i++;
		//if (i == 4) {
		//	::printf("\n");
		//	i = 0;
		//}
	}
}
