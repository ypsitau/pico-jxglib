#include <stdio.h>
#include "jxglib/ST7789.h"
#include "Font/shinonome16.h"
#include "ImageData.h"

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

using namespace jxglib;

void Test_WriteBuffer(ST7789& screen)
{
	uint16_t* buff = reinterpret_cast<uint16_t*>(::malloc(240 * 240 * 2));
	uint8_t offset = 0;
	for ( ; ; offset++) {
		uint16_t* p = buff;
		for (int i = 0; i < 240; i++) {
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(i + offset, 0, 0);
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, i + offset, 0);
			for (int j = 0; j < 80; j++, p++) *p = Color::RGB565(0, 0, i + offset);
		}
		screen.WriteBuffer(0, 0, 240, 240, buff);
		screen.DrawRectFill(100, 100, 30, 30);
	}
	::free(buff);
}

void Test_BouncingBall(ST7789& screen)
{
	int x = 10, y = 10;
	int xDir = 1, yDir = 1;
	int wdBall = 50, htBall = 30;
	for (;;) {
		screen.SetColor(Color::white);
		screen.DrawRectFill(x, y, wdBall, htBall);
		if (x + xDir < 0) xDir = 1;
		if (x + xDir + wdBall > screen.GetWidth()) xDir = -1;
		if (y + yDir < 0) yDir = 1;
		if (y + yDir + htBall > screen.GetHeight()) yDir = -1;
		::sleep_ms(1);
		screen.SetColor(Color::black);
		screen.DrawRectFill(x, y, wdBall, htBall);
		x += xDir, y += yDir;
	}
}

void Test_DrawString(ST7789& screen)
{
	screen.Clear();
	screen.SetFont(Font::shinonome16);
	screen.SetFontScale(2, 2);
	for (int i = 0; ; i++) {
		screen.DrawString(0, 0, "Hello World");
		//screen.SetFontScale(1, 1);
		char str[32];
		::sprintf(str, "こんにちは:%d", i);
		screen.DrawString(0, 32, str);
		::sprintf(str, "こんにちは:%d", i + 1);
		screen.DrawString(0, 32 * 2, str);
		::sprintf(str, "こんにちは:%d", i + 2);
		screen.DrawString(0, 32 * 3, str);
		::sprintf(str, "こんにちは:%d", i + 3);
		screen.DrawString(0, 32 * 4, str);
		::sprintf(str, "こんにちは:%d", i + 4);
		screen.DrawString(0, 32 * 5, str);
	}
}

void Test_DrawStringWrap(ST7789& screen)
{
	const char* strTbl[] = {
		" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxy{|}~",
		"The quick brown fox jumps over the lazy dog",
		"あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん",
		"アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン",
		"親譲りの無鉄砲で小供の時から損ばかりしている。小学校に居る時分学校の"
		"二階から飛び降りて一週間ほど腰を抜かした事がある。"
	};
	const FontSet* fontSetTbl[] = {
		&Font::shinonome16,
	};
	int iDrawMode = 0;
	int iFont = 0;
	int iStr = 0;
	const char* p = strTbl[iStr++];
	for (;;) {
		if (!*p) {
			if (iStr >= ArrayNumberOf(strTbl) || ((*strTbl[iStr] & 0x80 != 0) && !fontSetTbl[iFont]->HasExtraFont())) {
				iStr = 0;
				iFont++;
				if (iFont >= ArrayNumberOf(fontSetTbl)) {
					iFont = 0;
				}
			}
			p = strTbl[iStr++];
		}
		screen.SetFont(*fontSetTbl[iFont], 2);
		screen.Clear();
		p = screen.DrawStringWrap(0, 0, p);
		::sleep_ms(100);
	}
}

void Test_DrawImage(ST7789& screen)
{
	screen.Clear();
	screen.DrawImage(0, 0, image);
}

int main()
{
	ST7789 screen(spi0, 240, 240, 20, 21, 22);
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	::gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	screen.Initialize();
	//Test_BouncingBall(screen);
	//Test_WriteBuffer(screen);
	//Test_DrawString(screen);
	//Test_DrawStringWrap(screen);
	Test_DrawImage(screen);
}
