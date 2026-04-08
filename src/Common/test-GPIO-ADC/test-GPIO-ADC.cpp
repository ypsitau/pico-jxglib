#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "jxglib/Common.h"

using namespace jxglib;

void Test_adc_read()
{
	for (;;) {
		::printf("ADC0:%04x ADC1:%04x ADC2:%04x ADC3:%04x\n",
			GPIO26.adc_read(), GPIO27.adc_read(), GPIO28.adc_read(), GPIO29.adc_read());
	}
}

void Test_adc_set_round_robin()
{
	GPIO::adc_set_round_robin(GPIO26, GPIO27, GPIO28, GPIO29);
	for (int iChannel = 0; ; ) {
		::printf("ADC%d:%04x ", iChannel, ::adc_read());
		iChannel++;
		if (iChannel == 4) {
			::printf("\n");
			iChannel = 0;
		}
	}
}

int main()
{
	::stdio_init_all();
	::adc_init();
	GPIO26.adc_init();
	GPIO27.adc_init();
	GPIO28.adc_init();
	GPIO29.adc_init();
	Test_adc_read();
	//Test_adc_set_round_robin();
}
