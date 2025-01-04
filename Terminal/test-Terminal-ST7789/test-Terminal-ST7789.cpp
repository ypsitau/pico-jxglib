#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Font/shinonome16.h"
#include "jxglib/ST7789.h"
#include "jxglib/Terminal.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"

using namespace jxglib;

class EventHandler : public Terminal::EventHandler {
public:
	virtual void OnNewLine(Terminal& terminal) override {
		terminal.RefreshScreen();
		::sleep_ms(10);
	}
};

int main()
{
	EventHandler eventHandler;
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal;
	terminal.Initialize();
	terminal.SetEventHandler(&eventHandler);
	terminal.SetFont(Font::shinonome16);
	//terminal.SetColor(Color::black).SetColorBg(Color::white);
	terminal.AttachOutput(display);
	terminal.ClearScreen();
	terminal.Print(Text_FarFarAway_700Words);
}
