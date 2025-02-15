#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/misaki_gothic-japanese-level2.h"
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
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	Terminal terminal;
	terminal.Initialize();
	terminal.AttachOutput(display)
		.SetFont(Font::shinonome16).SetSpacingRatio(1, 1.5)
		.SetColor({255, 255, 255}).SetColorBg({0, 0, 64})
		.ClearScreen().Print(Text_Botchan); // Text_FarFarAway_700Words

	::printf("\n");
	for (;;) {
		::printf("[J] Roll Down  [K] Roll Up  [D] Dump\r");
		int ch = ::getchar();
		if (ch == 'j') {
			terminal.RollDown();
		} else if (ch == 'k') {
			terminal.RollUp();
		} else if (ch == 'd') {
			terminal.Dump.Cols(8).Ascii()(reinterpret_cast<const void*>(0x10000000), 64);
		}
	}
}
