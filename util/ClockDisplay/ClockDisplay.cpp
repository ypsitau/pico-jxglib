#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/RTC/DS323x.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/Font/shinonome18.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	::spi_init(spi1, 125'000'000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	::i2c_init(i2c0, 400'000);
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	RTC::DS323x rtc(i2c0);
	DateTime dtPrev;
	for (;;) {
		DateTime dt;
		RTC::Get(&dt);
		if (dt != dtPrev) {
			int ySeparator = display.GetHeight() * 2 / 5;
			dtPrev = dt;
			char str[32];
			display.SetFont(Font::shinonome18).SetFontScale(2, 2).SetColor(Color(192, 192, 192));
			::snprintf(str, sizeof(str), "%04d-%02d-%02d", dt.year, dt.month, dt.day);
			Size size = display.CalcStringSize(str);
			display.DrawString((display.GetWidth() - size.width) / 2, ySeparator - 8 - size.height, str);
			display.SetFont(Font::shinonome18).SetFontScale(4, 4).SetColor(Color(192, 192, 255));
			::snprintf(str, sizeof(str), "%02d:%02d:%02d", dt.hour, dt.min, dt.sec);
			size = display.CalcStringSize(str);
			display.DrawString((display.GetWidth() - size.width) / 2, ySeparator + 8, str);
			display.Refresh();
		}
		Tickable::Tick();
	}
}
