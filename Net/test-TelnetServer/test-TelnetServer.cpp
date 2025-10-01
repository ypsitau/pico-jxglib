#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

//FAT::SDCard fat("SD", spi0, 10'000'000, {CS: GPIO5});
//
//SDCard& ShellCmd_SDCard_GetSDCard() { return fat.GetSDCard(); }	// callback for ShellCmd_SDCard

int main()
{
	stdio_init_all();
	//jxglib_enable_startup_script(false);
	jxglib_labo_init(false);
	while (true) {
		jxglib_tick();
	}
}
