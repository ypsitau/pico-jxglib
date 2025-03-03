#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7789.h"
//#include "jxglib/Font/naga10-japanese-level2.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"
#include "jxglib/Font/shinonome16-japanese-level2.h"
#include "jxglib/Font/sisd24x32.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

Terminal terminal;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	GPIO21.init().pull_up();
#if 0
	SSD1306 display(i2c0, 0x3c);
	display.Initialize();
	const FontSet& fontSet = Font::shinonome12;
#else
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	const FontSet& fontSet = Font::shinonome16;
	//const FontSet& fontSet = Font::sisd24x32;
#endif
	terminal.AttachOutput(display);
	terminal.AttachInput(UART::Default);
	terminal.SetFont(fontSet).SetSpacingRatio(1., 1.2).ClearScreen();
	//terminal.SetFont(Font::naga10).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal.Dump.Cols(8)(reinterpret_cast<const void*>(0x10000000), 128);
	terminal.ShowCursor();
	for (;;) {
		Tickable::Sleep(50);
	}
}
