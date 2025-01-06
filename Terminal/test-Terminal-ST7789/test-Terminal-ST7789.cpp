#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome16-level2.h"
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
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal;
	terminal.Initialize();
	terminal.AttachOutput(display)
		.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2)
		.ClearScreen()
		.Print(Text_Botchan); // Text_FarFarAway_700Words
	for (;;) {
		int ch = ::getchar();
		if (ch == 'j') { terminal.RollUp(); } else if (ch == 'k') { terminal.RollDown(); }
	}
}
