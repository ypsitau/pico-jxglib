#include <stdio.h>
#include "jxglib/ST7789.h"
#include "jxglib/Common.h"
#include "Font/shinonome16.h"
#include "ImageData.h"

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

using namespace jxglib;

void Test_BouncingBall(ST7789& display)
{
	int x = 10, y = 10;
	int xDir = 1, yDir = 1;
	int wdBall = 50, htBall = 30;
	for (;;) {
		display.SetColor(Color::white);
		display.DrawRectFill(x, y, wdBall, htBall);
		if (x + xDir < 0) xDir = 1;
		if (x + xDir + wdBall > display.GetWidth()) xDir = -1;
		if (y + yDir < 0) yDir = 1;
		if (y + yDir + htBall > display.GetHeight()) yDir = -1;
		::sleep_ms(1);
		display.SetColor(Color::black);
		display.DrawRectFill(x, y, wdBall, htBall);
		x += xDir, y += yDir;
	}
}

void Test_DrawString(ST7789& display)
{
	display.Clear();
	display.SetFont(Font::shinonome16);
	display.SetFontScale(2, 2);
	for (int i = 0; ; i++) {
		display.DrawString(0, 0, "Hello World");
		//display.SetFontScale(1, 1);
		char str[32];
		::sprintf(str, "こんにちは:%d", i);
		display.DrawString(0, 32, str);
		::sprintf(str, "こんにちは:%d", i + 1);
		display.DrawString(0, 32 * 2, str);
		::sprintf(str, "こんにちは:%d", i + 2);
		display.DrawString(0, 32 * 3, str);
		::sprintf(str, "こんにちは:%d", i + 3);
		display.DrawString(0, 32 * 4, str);
		::sprintf(str, "こんにちは:%d", i + 4);
		display.DrawString(0, 32 * 5, str);
	}
}

void Test_DrawStringWrap(ST7789& display)
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
		display.SetFont(*fontSetTbl[iFont], 2);
		display.Clear();
		p = display.DrawStringWrap(0, 0, p);
		::sleep_ms(100);
	}
}

void Test_DrawImage(ST7789& display)
{
	display.Clear();
	display.DrawImage(0, 0, image);
}

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	GPIO18.set_function_SPI0_SCK();
	GPIO19.set_function_SPI0_TX();
	ST7789 display(spi0, 240, 240, GPIO20, GPIO21, GPIO22);
	display.Initialize();
	//Test_BouncingBall(display);
	//Test_WriteBuffer(display);
	//Test_DrawString(display);
	//Test_DrawStringWrap(display);
	Test_DrawImage(display);
}
