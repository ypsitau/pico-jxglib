#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

void gpio_callback1(uint gpio, uint32_t events)
{
    printf("1: GPIO%d %08x\n", gpio, events);
}

void gpio_callback2(uint gpio, uint32_t events)
{
    printf("2: GPIO%d %08x\n", gpio, events);
}

absolute_time_t timeStart = nil_time;

void handlerGPIO10()
{
	int64_t usecInsensitive = 100 * 1000;
	//int64_t usecInsensitive = 0;
	uint gpio = 10;
	uint32_t mask = ::gpio_get_irq_event_mask(gpio);
	if (mask & (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE)) {
		::gpio_acknowledge_irq(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
		if (timeStart == nil_time || ::absolute_time_diff_us(timeStart, ::get_absolute_time()) > usecInsensitive) {
			timeStart = ::get_absolute_time();
			printf("%08llx GPIO%d %d\n", timeStart, gpio, ::gpio_get(gpio));
		}
	}
}

void handlerGPIO11()
{
	uint32_t mask = ::gpio_get_irq_event_mask(11);
	if (mask & (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE)) {
		::gpio_acknowledge_irq(11, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
		printf("handlerGPIO11 %d\n", mask);
	}
}

int main()
{
	::stdio_init_all();
	GPIO10.init().set_dir(GPIO_IN).pull_up().
			set_irq_enabled(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true).
			add_raw_irq_handler(handlerGPIO10);
	GPIO11.init().set_dir(GPIO_IN).pull_up().
			set_irq_enabled(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true).
			add_raw_irq_handler(handlerGPIO11);
	::irq_set_enabled(IO_IRQ_BANK0, true);
	
	//gpio_set_irq_enabled_with_callback()
	//GPIO10.set_irq_enabled_with_callback(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback1);
	//GPIO11.set_irq_enabled_with_callback(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback2);
	for (;;) {
		//absolute_time_t start = ::get_absolute_time();
		//::sleep_ms(1000);
		//int64_t time = ::absolute_time_diff_us(start, ::get_absolute_time());
		//::printf("%lld\n", time);
		//::printf("%d\n", GPIO10.get());
	}
}
