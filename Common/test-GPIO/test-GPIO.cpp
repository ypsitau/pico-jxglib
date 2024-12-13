#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	GPIO12.init().set_dir_OUT().set_drive_strength(GPIO_DRIVE_STRENGTH_2MA).put(true);
	GPIO13.init().set_dir_OUT().set_drive_strength(GPIO_DRIVE_STRENGTH_4MA).put(true);
	GPIO14.init().set_dir_OUT().set_drive_strength(GPIO_DRIVE_STRENGTH_8MA).put(true);
	GPIO15.init().set_dir_OUT().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA).put(true);
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
