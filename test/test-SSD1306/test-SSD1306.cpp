// To generate font files:
//   gurax ../../Font/script/GenerateFont.gura -o Font shinonome12,shinonome14,shinonome16,shinonome18 *.cpp
#include <stdio.h>
#include <jxglib/SSD1306.h>
#include "Font/shinonome12.h"
#include "Font/shinonome14.h"
#include "Font/shinonome16.h"
#include "jxglib/Font/shinonome18.h"
#include "jxglib/Font/sisd8x16.h"
#include "jxglib/Font/sisd24x32.h"

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

using namespace jxglib;

void Test_Flash(SSD1306& display)
{
	for (int i = 0; i < 3; i++) {
		display.Flash(true);
		::sleep_ms(500);
		display.Flash(false);
		::sleep_ms(500);
	}
}

void Test_DrawPixel(SSD1306& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.Clear(blackDrawFlag? 0xff : 0x00);
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		int x = 40, y = 16;
		int xDir = +1, yDir = +1;
		for (int i = 0; i < 1000; i++) {
			display.DrawPixel(x, y);
			if (x + xDir < 0) xDir = +1;
			if (x + xDir >= display.GetWidth()) xDir = -1;
			if (y + yDir < 0) yDir = +1;
			if (y + yDir >= display.GetHeight()) yDir = -1;
			x += xDir, y += yDir;
			if (i % 10 == 0) display.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawHLine(SSD1306& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int x = -64; x <= display.GetWidth(); x++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			for (int i = 0; i < display.GetHeight(); i++) {
				display.DrawHLine(x, i, i);
			}
			display.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawVLine(SSD1306& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = -64; y <= display.GetHeight(); y++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			for (int i = 0; i < display.GetHeight(); i++) {
				display.DrawVLine(i, y, i);
				display.DrawVLine(i + display.GetHeight() * 1, display.GetHeight() - y - 1, -i);
			}
			display.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawLine(SSD1306& display)
{
	int xMid = display.GetWidth() / 2;
	int yMid = display.GetHeight() / 2;
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = 0; y < display.GetHeight(); y++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawLine(xMid, yMid, xRight, y);
			display.Refresh();
			::sleep_ms(10);
		}
		for (int x = xRight; x >= 0; x--) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawLine(xMid, yMid, x, yBottom);
			display.Refresh();
			::sleep_ms(10);
		}
		for (int y = yBottom; y >= 0; y--) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawLine(xMid, yMid, 0, y);
			display.Refresh();
			::sleep_ms(10);
		}
		for (int x = 0; x <= xRight; x++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawLine(xMid, yMid, x, 0);
			display.Refresh();
			::sleep_ms(10);
		}
	}
}

void Test_DrawRect(SSD1306& display)
{
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawRect(0, 0, i * 2, i);
			display.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawRect(xRight, yBottom, -i * 2, -i);
			display.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawRectFill(SSD1306& display)
{
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawRectFill(0, 0, i * 2, i);
			display.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Clear(blackDrawFlag? 0xff : 0x00);
			display.DrawRectFill(xRight, display.GetHeight() - 1, -i * 2, -i);
			display.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawBitmap(SSD1306& display)
{
	const uint8_t bmp[] = {
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b11111111,0b00000000,0b00000000,0b11111110,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000011,0b10000000,0b11111111,0b11111110,0b00000001,0b11000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000110,0b00000000,0b00000000,0b00000000,0b00000000,0b11100000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00001100,0b00011000,0b00000000,0b00000000,0b00011100,0b01110000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00011000,0b00111000,0b00000000,0b00000000,0b00011100,0b00111000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00111000,0b00111000,0b00000000,0b00000000,0b00001100,0b00011000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00110000,0b00111000,0b00000000,0b00000000,0b00000000,0b00001100,0b00000000,0b00000000,
		0b00000000,0b00000000,0b01100001,0b11111111,0b10000000,0b00000001,0b11000001,0b11001100,0b00000000,0b00000000,
		0b00000000,0b00000000,0b01100001,0b11111111,0b10000000,0b00000001,0b11000001,0b11000110,0b00000000,0b00000000,
		0b00000000,0b00000000,0b11000000,0b00111000,0b00000000,0b00000001,0b11000000,0b11000110,0b00000000,0b00000000,
		0b00000000,0b00000000,0b11000000,0b00111000,0b00000000,0b00000000,0b00000000,0b00000011,0b00000000,0b00000000,
		0b00000000,0b00000001,0b10000000,0b00111000,0b00000000,0b00000000,0b00011100,0b00000011,0b00000000,0b00000000,
		0b00000000,0b00000001,0b10000000,0b00111000,0b00000000,0b00000000,0b00011100,0b00000001,0b10000000,0b00000000,
		0b00000000,0b00000001,0b10000000,0b00000011,0b11001111,0b01110011,0b11111100,0b00000001,0b10000000,0b00000000,
		0b00000000,0b00000011,0b00000000,0b00000111,0b11110000,0b00000111,0b11110000,0b00000000,0b11000000,0b00000000,
		0b00000000,0b00000011,0b00000000,0b00001100,0b00110000,0b00001110,0b00111000,0b00000000,0b11000000,0b00000000,
		0b00000000,0b00000110,0b00000000,0b00011000,0b00011000,0b00001100,0b00011000,0b00000000,0b11000000,0b00000000,
		0b00000000,0b00000110,0b00000000,0b00011000,0b00011000,0b00001100,0b00011000,0b00000000,0b01100000,0b00000000,
		0b00000000,0b00000110,0b00000000,0b00011000,0b00011000,0b00011100,0b00011000,0b00000000,0b01100000,0b00000000,
		0b00000000,0b00000110,0b00000000,0b00011100,0b00111111,0b11111100,0b00111000,0b00000000,0b01100000,0b00000000,
		0b00000000,0b00001100,0b00000000,0b00011111,0b11110000,0b00001111,0b11111000,0b00000000,0b00100000,0b00000000,
		0b00000000,0b00001100,0b00000000,0b00110111,0b11100000,0b00000011,0b11101100,0b00000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000000,0b01100000,0b00000000,0b00000000,0b00000110,0b00000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000000,0b11100000,0b00000000,0b00000000,0b00000111,0b00000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000000,0b11000000,0b00000000,0b00000000,0b00000011,0b00000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000001,0b10000000,0b00000000,0b00000000,0b00000001,0b10000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000011,0b00000000,0b00000000,0b00000000,0b00000001,0b11000000,0b00110000,0b00000000,
		0b00000000,0b00001100,0b00000111,0b00000000,0b00000000,0b00000000,0b00000000,0b11100000,0b01100000,0b00000000,
		0b00000000,0b00000110,0b00001110,0b00000000,0b00000000,0b00000000,0b00000000,0b01110000,0b11100000,0b00000000,
		0b00000000,0b00000011,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11000000,0b00000000,
		0b00000000,0b00000001,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b10000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
		0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
	};
	display.DrawBitmap(0, 0, bmp, 80, 64, false);
	display.Refresh();
	::sleep_ms(2000);
}

void Test_DrawStringWrap(SSD1306& display)
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
		&Font::shinonome12, &Font::shinonome14, &Font::shinonome16, &Font::shinonome18,
		&Font::sisd8x16, &Font::sisd24x32,
	};
	int iFont = 0;
	int iStr = 0;
	const char* p = strTbl[iStr++];
	display.SetColor(Color::white);
	display.SetColorBg(Color::black);
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
		display.SetFont(*fontSetTbl[iFont]);
		display.Clear();
		p = display.DrawStringWrap(0, 0, p);
		display.Refresh();
		::sleep_ms(200);
	}
}

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0);
	display.Initialize();
	for (;;) {
		Test_Flash(display);
		Test_DrawPixel(display);
		Test_DrawHLine(display);
		Test_DrawVLine(display);
		Test_DrawLine(display);
		Test_DrawRect(display);
		Test_DrawRectFill(display);
		//Test_DrawBitmap(display);
		Test_DrawStringWrap(display);
	}
}