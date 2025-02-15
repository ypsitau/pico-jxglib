#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome16-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	Terminal terminal;
	terminal.Initialize();
	terminal.AttachOutput(display).SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal.Print(Text_Botchan); // Text_FarFarAway_700Words
	for (;;) {
		if (!GPIO18.get()) terminal.RollUp();
		if (!GPIO19.get()) terminal.RollDown();
		if (!GPIO20.get()) terminal.Dump.Cols(8).Ascii()(reinterpret_cast<const void*>(0x10000000), 64);
		::sleep_ms(50);
	}
}
