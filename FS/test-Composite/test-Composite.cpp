#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/FAT/RAMDisk.h"
#include "jxglib/FAT/USBMSC.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	LFS::Flash		drive_A("A", 0x1010'0000, 0x0004'0000); 			// Flash address and size 256kB
	LFS::Flash		drive_B("B", 0x1014'0000, 0x0004'0000); 			// Flash address and size 256kB
	FAT::Flash		drive_C("*C", 0x1018'0000, 0x0004'0000); 			// Flash address and size 256kB
	FAT::Flash		drive_D("D", 0x101c'0000, 0x0004'0000); 			// Flash address and size 256kB
	FAT::RAMDisk	drive_E("E", 0x2'0000);								// RAM Disk 128kB
	FAT::SDCard		drive_F("F", spi0, 10'000'000, {CS: GPIO5});		// SDCard on SPI0 10MHz
	FAT::USBMSC		drive_G("G");										// USB Mass Storage Device (MSC) on USB Host
	//FAT::USBMSC		drive_H("H");										// USB Mass Storage Device (MSC) on USB Host
	USBHost::Initialize();
#if 0
	USBHost::Keyboard keyboard;
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	Display::Terminal terminal;
	terminal.Initialize().AttachDisplay(display.Initialize(Display::Dir::Rotate90))
		.AttachKeyboard(keyboard.SetCapsLockAsCtrl()).SetFont(Font::shinonome16);
	Shell::AttachTerminal(terminal);
#else
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
#endif
	for (;;) Tickable::Tick();
}
