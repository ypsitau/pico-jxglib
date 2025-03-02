#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7789.h"
//#include "jxglib/Font/naga10-japanese-level2.h"
#include "jxglib/Font/shinonome12-japanese-level2.h"
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
#if 0
	SSD1306 display(i2c0, 0x3c);
	display.Initialize();
	const FontSet& fontSet = Font::shinonome12;
#else
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	const FontSet& fontSet = Font::shinonome16;
	//const FontSet& fontSet = Font::sisd24x32;
#endif
	terminal.AttachOutput(display);
	terminal.SetFont(fontSet).SetSpacingRatio(1., 1.2).ClearScreen();
	//terminal.SetFont(Font::naga10).SetSpacingRatio(1., 1.2).ClearScreen();
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
			switch (keyData) {
			case VK_RETURN:	terminal.Edit_Finish('\n');		break;
			case VK_DELETE:	terminal.Edit_Delete();			break;
			case VK_BACK:	terminal.Edit_Back();			break;
			case VK_LEFT:	terminal.Edit_MoveBackward();	break;
			case VK_RIGHT:	terminal.Edit_MoveForward();	break;
			default: break;
			}
		} else if (keyData < 0x20) {
			switch (keyData) {
			case 'A' - '@': terminal.Edit_MoveHome();		break;
			case 'B' - '@': terminal.Edit_MoveBackward();	break;
			case 'C' - '@': break;
			case 'D' - '@': terminal.Edit_Delete();			break;
			case 'E' - '@': terminal.Edit_MoveEnd();		break;
			case 'F' - '@': terminal.Edit_MoveForward();	break;
			case 'K' - '@': terminal.Edit_DeleteToEnd();	break;
			default: break;
			}
		} else {
			terminal.Edit_InsertChar(keyData);
		}
		Tickable::Sleep(50);
	}
}
