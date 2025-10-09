#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
#if 0	
	::spi_init(spi1, 125'000'000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	//DrawableTest::DrawString(display);
#endif
	for (;;) Tickable::Tick();
}
