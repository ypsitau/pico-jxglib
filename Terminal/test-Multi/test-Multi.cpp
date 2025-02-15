#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome16.h"
#include "jxglib/Font/sisd24x32.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal1, terminal2, terminal3;
	terminal1.Initialize();
	terminal2.Initialize();
	terminal3.Initialize();
	terminal1.AttachOutput(display, {0, 0, 240, 160});
	terminal2.AttachOutput(display, {0, 160, 120, 160});
	terminal3.AttachOutput(display, {120, 160, 120, 160});
	terminal1.SetColorBg(Color(0, 0, 128)).ClearScreen();
	terminal2.SetColorBg(Color(0, 128, 0)).ClearScreen();
	terminal3.SetColorBg(Color(128, 0, 0)).ClearScreen();
	terminal1.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2);
	terminal2.SetFont(Font::sisd24x32).SetSpacingRatio(1., 1.2);
	terminal3.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2);
	terminal1.Dump.Cols(8)(reinterpret_cast<const void*>(0x10000000), 64);
	terminal2.Printf("0123456789ABCDEFGHIJ");
	terminal3.Print(Text_FarFarAway_700Words);
	for (;;) ::tight_loop_contents();
}
