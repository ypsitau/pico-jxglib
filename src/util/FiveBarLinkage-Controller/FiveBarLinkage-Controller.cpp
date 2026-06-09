#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	//LABOPlatform::Instance.Initialize();
	::adc_init();
	PWM16.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
	PWM17.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
	GPIO26.adc_init();
	GPIO27.adc_init();
	for (;;) {
		float valueL = GPIO26.adc_read() >> 4;
		float valueR = GPIO27.adc_read() >> 4;
		PWM16.set_chan_duty(((valueL - 128) / 128.) * 0.05 + 0.07);
		PWM17.set_chan_duty(((valueR - 128) / 128.) * 0.05 + 0.07);
		//::printf("%d %d\n", valueL, valueR);
		Tickable::Tick();
	}
}
