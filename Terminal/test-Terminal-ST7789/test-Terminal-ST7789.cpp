#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome16-level2.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"
#include "jxglib/sample/Text_Botchan.h"
#include "jxglib/ST7789.h"

using namespace jxglib;

class EventHandler : public Terminal::EventHandler {
public:
	virtual void OnNewLine(Terminal& terminal) override {
		terminal.RefreshScreen();
		::sleep_ms(400);
	}
};

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal;
	EventHandler eventHandler;
	terminal.Initialize();
	terminal.AttachOutput(display)
		.SetEventHandler(eventHandler)
		.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2)
		//.SetColor(Color::black).SetColorBg(Color::white)
		.ClearScreen()
		//.Print(Text_FarFarAway_700Words)
		.Print(Text_Botchan);
}
