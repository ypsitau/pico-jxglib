#include <stdio.h>
#include "jxglib/ST7789.h"
#include "Font/shinonome16.h"

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
	int x = 10, y = 10;
	int xDir = 1, yDir = 1;
	int wdBall = 50, htBall = 30;
	tft.SetColor(Color::black.RGB565());
	tft.Fill();
	tft.SetFont(Font::shinonome16);
	tft.SetFontScale(2, 2);
	tft.SetColor(Color::white.RGB565());
	for (int i = 0; ; i++) {
		tft.DrawString(0, 0, "Hello World");
		//tft.SetFontScale(1, 1);
		char str[32];
		::sprintf(str, "こんにちは:%d", i);
		tft.DrawString(0, 32, str);
		::sprintf(str, "こんにちは:%d", i + 1);
		tft.DrawString(0, 32 * 2, str);
		::sprintf(str, "こんにちは:%d", i + 2);
		tft.DrawString(0, 32 * 3, str);
		::sprintf(str, "こんにちは:%d", i + 3);
		tft.DrawString(0, 32 * 4, str);
		::sprintf(str, "こんにちは:%d", i + 4);
		tft.DrawString(0, 32 * 5, str);
	}
#if 0
	for (;;) {
		tft.SetColor(Color::RGB565(255, 255, 255));
		tft.DrawRectFill(x, y, wdBall, htBall);
		if (x + xDir < 0) xDir = 1;
		if (x + xDir + wdBall > tft.GetWidth()) xDir = -1;
		if (y + yDir < 0) yDir = 1;
		if (y + yDir + htBall > tft.GetHeight()) yDir = -1;
		::sleep_ms(1);
		tft.SetColor(Color::RGB565(0, 0, 0));
		tft.DrawRectFill(x, y, wdBall, htBall);
		x += xDir, y += yDir;
	}
#endif
#if 0
	uint8_t offset = 0;
	for ( ; ; offset++) {
		uint16_t* p = buff;
		for (int i = 0; i < 240; i++) {
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(i + offset, 0, 0);
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, i + offset, 0);
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, 0, i + offset);
		}
		tft.Transfer(0, 0, 240, 240, buff);
		tft.DrawRectFill(x, y, 30, 30);
	}
#endif
	//::memset(buff, 0xff, 240 * 240 * 2);		
	//tft.raw.ColumnAddressSet(30, 100);
	//tft.raw.RowAddressSet(30, 60);
	//tft.raw.MemoryWrite16(buff, 50 * 30);
#if 0
	for (;;) {
		::memset(buff, 0x00, 240 * 240 * 2);		
		tft.raw.MemoryWrite16(buff, 240 * 240);
		::sleep_ms(10);
		::memset(buff, 0xff, 240 * 240 * 2);		
		tft.raw.MemoryWrite16(buff, 240 * 240);
		::sleep_ms(10);
	}
#endif
}
