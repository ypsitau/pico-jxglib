#include <stdio.h>
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	uint16_t* buff = reinterpret_cast<uint16_t*>(::malloc(240 * 240 * 2));
	ST7789 tft(spi0, 240, 240, 20, 21, 22);
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	::gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	tft.Initialize();
	tft.raw.ColumnAddressSet(0, 240);
	tft.raw.RowAddressSet(0, 240);
	uint16_t* p = buff;
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(i, 0, 0);
		for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, i, 0);
		for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, 0, i);
	}
	tft.raw.MemoryWriteBy16Bit(buff, 240 * 240);
	::memset(buff, 0xff, 240 * 240 * 2);		
	tft.raw.ColumnAddressSet(30, 100);
	tft.raw.RowAddressSet(30, 60);
	tft.raw.MemoryWriteBy16Bit(buff, 50 * 30);
#if 0
	for (;;) {
		::memset(buff, 0x00, 240 * 240 * 2);		
		tft.raw.MemoryWriteBy16Bit(buff, 240 * 240);
		::sleep_ms(10);
		::memset(buff, 0xff, 240 * 240 * 2);		
		tft.raw.MemoryWriteBy16Bit(buff, 240 * 240);
		::sleep_ms(10);
	}
#endif
}
