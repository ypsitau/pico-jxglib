#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/RAMDisk.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/FAT/USBMSC.h"
#include "jxglib/RTC/Pico.h"
#include "jxglib/RTC/DS3231.h"
#include "jxglib/Font/shinonome16-japanese-level2.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	bi_decl(bi_1pin_with_name(GPIO2,	"SD Card SCL"));
	bi_decl(bi_1pin_with_name(GPIO3,	"SD Card MOSI"));
	bi_decl(bi_1pin_with_name(GPIO4,	"SD Card MISO"));
	bi_decl(bi_1pin_with_name(GPIO5,	"SD Card CS"));
	bi_decl(bi_1pin_with_name(GPIO10,	"ST7789 RST"));
	bi_decl(bi_1pin_with_name(GPIO11,	"ST7789 DC"));
	bi_decl(bi_1pin_with_name(GPIO12,	"ST7789 CS"));
	bi_decl(bi_1pin_with_name(GPIO13,	"ST7789 BL"));
	bi_decl(bi_1pin_with_name(GPIO14,	"ST7789 SCL"));
	bi_decl(bi_1pin_with_name(GPIO15,	"ST7789 MOSI"));
	bi_decl(bi_1pin_with_name(GPIO16,	"RTC SDA"));
	bi_decl(bi_1pin_with_name(GPIO17,	"RTC SCL"));
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO16.set_function_I2C0_SDA();
	GPIO17.set_function_I2C0_SCL();
	::i2c_init(i2c0, 1000'000);
	RTC::DS3231 rtc(i2c0);
	//RTC::Pico rtc;
	LFS::Flash		driveA("A:", 0x1010'0000, 0x0004'0000); 		// Flash address and size 256kB
	LFS::Flash		driveB("B:", 0x1014'0000, 0x0004'0000); 		// Flash address and size 256kB
	LFS::Flash		driveC("*C:", 0x1018'0000, 0x0004'0000); 		// Flash address and size 256kB
	FAT::Flash		driveD("D:", 0x101c'0000, 0x0004'0000); 		// Flash address and size 256kB
	FAT::RAMDisk	driveE("E:", 0x0002'0000);						// RAM Disk 128kB
	FAT::SDCard		driveF("F:", spi0, 10'000'000, {CS: GPIO5});	// SDCard on SPI0 10MHz
	FAT::USBMSC		driveG("G:");									// USB Mass Storage Device (MSC) on USB Host
	FAT::USBMSC		driveH("H:");									// USB Mass Storage Device (MSC) on USB Host
#if 0
	::spi_init(spi1, 125'000'000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	Display::Terminal terminal;
	terminal.Initialize().AttachDisplay(display.Initialize(Display::Dir::Rotate90)).SetFont(Font::shinonome16);
	USBHost::Initialize();
	USBHost::Keyboard keyboard;
	terminal.AttachKeyboard(keyboard.SetCapsLockAsCtrl());
	//terminal.AttachKeyboard(Stdio::GetKeyboard());
	Shell::AttachTerminal(terminal);
#else
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
#endif
	for (;;) Tickable::Tick();
}
