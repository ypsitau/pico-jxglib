#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome16-japanese-level2.h"
#include "jxglib/Font/sisd24x32.h"
#include "jxglib/sample/Text_Botchan.h"
#include "jxglib/VT100.h"
#include "jxglib/UART.h"

using namespace jxglib;

Terminal terminal;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	GPIO18.init().pull_up();
	GPIO19.init().pull_up();
	GPIO20.init().pull_up();
	GPIO21.init().pull_up();
	//SSD1306 display(i2c0, 0x3c);
	//display.Initialize();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	terminal.AttachOutput(display);
	terminal.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2).ClearScreen();
	terminal.Println("hello");
	VT100::Decoder vt100Decoder;
	UART& uart = UART0;
	terminal.ShowCursor();
	for (;;) {
		int keyData;
		while (uart.raw.is_readable()) vt100Decoder.FeedChar(uart.raw.getc());
		if (!vt100Decoder.HasKeyData()) {
			// nothing to do
		} else if (vt100Decoder.GetKeyData(&keyData)) {
			if (keyData == VK_DELETE) {
				terminal.EraseCursor();
				if (terminal.GetEditor().DeleteChar()) {
					terminal.DrawEditBuff();
					terminal.DrawCursor();
				}
			} else if (keyData == VK_BACK) {
				terminal.EraseCursor();
				if (terminal.GetEditor().MoveCursorBackward() && terminal.GetEditor().DeleteChar()) {
					terminal.DrawEditBuff();
					terminal.DrawCursor();
				}
			} else if (keyData == VK_LEFT) {
				terminal.EraseCursor();
				if (terminal.GetEditor().MoveCursorBackward()) terminal.DrawCursor();
			} else if (keyData == VK_RIGHT) {
				terminal.EraseCursor();
				if (terminal.GetEditor().MoveCursorForward()) terminal.DrawCursor();
			}
		} else {
			terminal.EraseCursor();
			if (terminal.GetEditor().InsertChar(keyData)) {
				terminal.DrawEditBuff();
				terminal.DrawCursor();
			}
		}
		Tickable::Sleep(50);
	}
}
