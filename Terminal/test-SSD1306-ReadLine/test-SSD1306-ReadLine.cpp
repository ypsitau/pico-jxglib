#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/SSD1306.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	::i2c_init(i2c0, 400 * 1000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0, 0x3c);
	display.Initialize();
	terminal.AttachOutput(display);
	terminal.AttachInput(USBHost::GetKeyboard().SetCapsLockAsCtrl());
	terminal.SetFont(Font::shinonome12).SetSpacingRatio(1., 1)
		.SetColor(Color::white).SetColorBg(Color::black)
		.SetColorInEdit(Color::white).SetColorCursor(Color::white)
		.ClearScreen();
	terminal.Println("ReadLine Test Program");
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
	}
}
