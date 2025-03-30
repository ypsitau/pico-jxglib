#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome16.h"
#include "jxglib/Font/sisd24x32.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"

using namespace jxglib;

Display::Terminal terminal1, terminal2, terminal3;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	terminal1.AttachDisplay(display, {0, 0, 240, 160}).SetColorBg(Color(0, 0, 128))
			.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal2.AttachDisplay(display, {0, 160, 120, 160}).SetColorBg(Color(0, 128, 0))
			.SetFont(Font::sisd24x32).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal3.AttachDisplay(display, {120, 160, 120, 160}).SetColorBg(Color(128, 0, 0))
			.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal1.Dump.Cols(8)(reinterpret_cast<const void*>(0x10000000), 64);
	terminal2.Printf("0123456789ABCDEFGHIJ");
	terminal3.Print(Text_FarFarAway_700Words);
	for (;;) ::tight_loop_contents();
}
