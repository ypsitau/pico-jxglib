#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400 * 1000);
	::spi_init(spi1, 125'000'000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	do {
		int nTrial = 100;
		SSD1306 display(i2c0);
		display.Initialize();
		display.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2);
		display.DrawStringWrap(0, 0, Text_Botchan).Refresh();
		absolute_time_t absTimeStart = ::get_absolute_time();
		for (int i = 0; i < nTrial; i++) display.Refresh();
		int64_t timeUSec = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		::printf("SSD1306 %lldusec\n", timeUSec / nTrial);
	} while (0);
	do {
		int nTrial = 100;
		ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
		display.Initialize(Display::Dir::Rotate0);
		Canvas canvas;
		canvas.AttachDrawable(display);
		canvas.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2);
		canvas.DrawStringWrap(0, 0, Text_Botchan).Refresh();
		absolute_time_t absTimeStart = ::get_absolute_time();
		for (int i = 0; i < nTrial; i++) canvas.Refresh();
		int64_t timeUSec = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		::printf("ST7789  %lldusec\n", timeUSec / nTrial);
	} while (0);
	for (;;) ::tight_loop_contents();
}
