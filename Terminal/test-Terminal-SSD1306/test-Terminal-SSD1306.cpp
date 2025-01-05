#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12-level2.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"
#include "jxglib/sample/Text_Botchan.h"
#include "jxglib/SSD1306.h"

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
	::i2c_init(i2c1, 400000);
	GPIO26.set_function_I2C1_SDA().pull_up();
	GPIO27.set_function_I2C1_SCL().pull_up();
	SSD1306 display(i2c1);
	display.Initialize();
	Terminal terminal;
	EventHandler eventHandler;
	terminal.Initialize();
	terminal.AttachOutput(display)
		.SetEventHandler(eventHandler)
		.SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2)
		//.SetColor(Color::black).SetColorBg(Color::white)
		.ClearScreen()
		//.Print(Text_FarFarAway_700Words)
		.Print(Text_Botchan);
}
