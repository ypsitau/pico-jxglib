#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/USBHost.h"
#include "jxglib/CmdLine.h"
#include "jxglib/Font/shinonome12.h"

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
	terminal.Initialize()
		.AttachDisplay(display.Initialize())
		.AttachKeyboard(USBHost::GetKeyboard().SetCapsLockAsCtrl())
		.SetFont(Font::shinonome12);
	CmdLine::AttachTerminal(terminal);
	terminal.Println("CmdLine Test Program");
	for (;;) Tickable::Tick();
}
