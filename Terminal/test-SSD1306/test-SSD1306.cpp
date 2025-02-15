#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"
#include "jxglib/sample/Text_Botchan.h"
#include "jxglib/sample/Text_FarFarAway_700Words.h"
#include "jxglib/SSD1306.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c1, 400 * 1000);
	GPIO26.set_function_I2C1_SDA().pull_up();
	GPIO27.set_function_I2C1_SCL().pull_up();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	SSD1306 display(i2c1);
	display.Initialize();
	Terminal terminal;
	terminal.Initialize();
	terminal.AttachOutput(display).SetFont(Font::shinonome12).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal.Print(Text_Botchan); // Text_FarFarAway_700Words
	for (;;) {
		if (!GPIO18.get()) terminal.RollUp();
		if (!GPIO19.get()) terminal.RollDown();
		if (!GPIO20.get()) terminal.Dump.NoAddr().Cols(6)(reinterpret_cast<const void*>(0x10000000), 60);
		::sleep_ms(50);
	}
}
