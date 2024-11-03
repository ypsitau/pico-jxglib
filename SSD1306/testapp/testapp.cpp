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

using namespace jxglib;

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

void SetupScreen(SSD1306& oled, SSD1306::DrawMode drawMode)
{
	switch (drawMode) {
	case SSD1306::DrawMode::Set:
		oled.Clear();
		break;
	case SSD1306::DrawMode::Clear:
		oled.Clear(0xff);
		break;
	case SSD1306::DrawMode::Invert:
		oled.Clear();
		oled.DrawRectFill(16, 16, 96, 32);
		break;
	default:
		break;
	}
}

void Test_Flash(SSD1306& oled)
{
	for (int i = 0; i < 3; i++) {
		oled.Flash(true);
		::sleep_ms(500);
		oled.Flash(false);
		::sleep_ms(500);
	}
}

void Test_DrawPixel(SSD1306& oled)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		SetupScreen(oled, drawMode);
		int x = 40, y = 16;
		int xDir = +1, yDir = +1;
		for (int i = 0; i < 1000; i++) {
			oled.DrawPixel(x, y);
			if (x + xDir < 0) xDir = +1;
			if (x + xDir >= oled.GetWidth()) xDir = -1;
			if (y + yDir < 0) yDir = +1;
			if (y + yDir >= oled.GetHeight()) yDir = -1;
			x += xDir, y += yDir;
			if (i % 10 == 0) oled.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawHLine(SSD1306& oled)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int x = -64; x < oled.GetWidth(); x++) {
			SetupScreen(oled, drawMode);
			for (int i = 0; i < oled.GetHeight(); i++) {
				oled.DrawHLine(x, i, i);
			}
			oled.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawVLine(SSD1306& oled)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int y = -64; y < oled.GetHeight(); y++) {
			SetupScreen(oled, drawMode);
			for (int i = 0; i < oled.GetHeight(); i++) {
				oled.DrawVLine(i, y, i);
				oled.DrawVLine(i + oled.GetHeight() * 1, oled.GetHeight() - y - 1, -i);
			}
			oled.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawLine(SSD1306& oled)
{
	int xMid = oled.GetWidth() / 2;
	int yMid = oled.GetHeight() / 2;
	int xRight = oled.GetWidth() - 1;
	int yBottom = oled.GetHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int y = 0; y < oled.GetHeight(); y++) {
			SetupScreen(oled, drawMode);
			oled.DrawLine(xMid, yMid, xRight, y);
			oled.Refresh();
			::sleep_ms(10);
		}
		for (int x = xRight; x >= 0; x--) {
			SetupScreen(oled, drawMode);
			oled.DrawLine(xMid, yMid, x, yBottom);
			oled.Refresh();
			::sleep_ms(10);
		}
		for (int y = yBottom; y >= 0; y--) {
			SetupScreen(oled, drawMode);
			oled.DrawLine(xMid, yMid, 0, y);
			oled.Refresh();
			::sleep_ms(10);
		}
		for (int x = 0; x <= xRight; x++) {
			SetupScreen(oled, drawMode);
			oled.DrawLine(xMid, yMid, x, 0);
			oled.Refresh();
			::sleep_ms(10);
		}
	}
}

void Test_DrawRect(SSD1306& oled)
{
	int xRight = oled.GetWidth() - 1;
	int yBottom = oled.GetHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int i = 0; i < oled.GetHeight(); i++) {
			SetupScreen(oled, drawMode);
			oled.DrawRect(0, 0, i * 2, i);
			oled.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < oled.GetHeight(); i++) {
			SetupScreen(oled, drawMode);
			oled.DrawRect(xRight, yBottom, -i * 2, -i);
			oled.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawRectFill(SSD1306& oled)
{
	int xRight = oled.GetWidth() - 1;
	int yBottom = oled.GetHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int i = 0; i < oled.GetHeight(); i++) {
			SetupScreen(oled, drawMode);
			oled.DrawRectFill(0, 0, i * 2, i);
			oled.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < oled.GetHeight(); i++) {
			SetupScreen(oled, drawMode);
			oled.DrawRectFill(xRight, oled.GetHeight() - 1, -i * 2, -i);
			oled.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawBitmap(SSD1306& oled)
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
	oled.DrawBitmap(0, 0, bmp, 80, 64);
	oled.Refresh();
	::sleep_ms(2000);
}

void Test_DrawStringBBox(SSD1306& oled)
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
					iDrawMode++;
					if (iDrawMode > static_cast<int>(SSD1306::DrawMode::Invert)) {
						return;
					}
				}
			}
			p = strTbl[iStr++];
		}
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(iDrawMode);
		oled.SetDrawMode(drawMode);
		oled.SetFont(*fontSetTbl[iFont]);
		SetupScreen(oled, drawMode);
		p = oled.DrawStringBBox(0, 0, p);
		oled.Refresh();
		::sleep_ms(200);
	}
}

int main()
{
	::stdio_init_all();
	::i2c_init(i2c_default, 400000);
	::gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	::gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
	::gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	::gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
	SSD1306 oled(i2c_default);
	oled.Initialize();
	for (;;) {
		Test_Flash(oled);
		Test_DrawPixel(oled);
		Test_DrawHLine(oled);
		Test_DrawVLine(oled);
		Test_DrawLine(oled);
		Test_DrawRect(oled);
		Test_DrawRectFill(oled);
		Test_DrawBitmap(oled);
		Test_DrawStringBBox(oled);
	}
}
