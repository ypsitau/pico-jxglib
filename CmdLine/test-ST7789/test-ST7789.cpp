#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/CmdLine.h"
#include "jxglib/ST7789.h"
#include "jxglib/USBHost.h"
#include "jxglib/Font/shinonome12.h"

using namespace jxglib;

Display::Terminal terminal;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate90);
	terminal.AttachOutput(display);
	terminal.AttachInput(USBHost::GetKeyboard().SetCapsLockAsCtrl());
	terminal.SetFont(Font::shinonome12);
	CmdLine::AttachTerminal(terminal);
	for (;;) Tickable::Tick();
}
