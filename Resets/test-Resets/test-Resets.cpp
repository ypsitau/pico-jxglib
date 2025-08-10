#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
	for (;;) Tickable::Tick();
}
