#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/SSD1306.h"
#include "jxglib/Terminal.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"

using namespace jxglib;

class EventHandler : public Terminal::EventHandler {
public:
	virtual void OnLineFeed(Terminal& terminal) override { ::sleep_ms(500); }
};

int main()
{
	EventHandler eventHandler;
	::stdio_init_all();
	::i2c_init(i2c1, 400000);
	GPIO26.set_function_I2C1_SDA().pull_up();
	GPIO27.set_function_I2C1_SCL().pull_up();
	SSD1306 display(i2c1);
	display.Initialize();
	Terminal terminal;
	terminal.Initialize();
	terminal.SetEventHandler(&eventHandler);
	terminal.SetFont(Font::shinonome12);
	terminal.SetColor(Color::black).SetColorBg(Color::white);
	terminal.AttachOutput(display);
	terminal.ClearScreen();
	for (int i = 0; ; i++) {
		terminal.Printf("Hello %d\n", i);
	}
}
