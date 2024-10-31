// To generate font files:
//   gurax ../../Font/script/GenerateFont.gura -o Font shinonome12,shinonome14,shinonome16,shinonome18 *.cpp
#include <stdio.h>
#include <jxglib/SSD1306.h>
#include "Font/shinonome12.h"
#include "Font/shinonome14.h"
#include "Font/shinonome16.h"
#include "jxglib/Font/shinonome18.h"
#include "Font/sisd24x32.h"

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
		0b00011111,0b11011111,0b10000000,
		0b00111111,0b11011111,0b11000000,
		0b01011111,0b11011111,0b10100000,
		0b11111100,0b01110000,0b01110000,
		0b11111100,0b01110000,0b01110000,
		0b11111100,0b01110000,0b01110000,
		0b11101110,0b01110000,0b01110000,
		0b11101110,0b01110000,0b01110000,
		0b11100111,0b01110000,0b01110000,
		0b11100111,0b01110000,0b01110000,
		0b11100011,0b11110000,0b01110000,
		0b11100011,0b11110000,0b01110000,
		0b11100011,0b11110000,0b01110000,
		0b01011111,0b10101111,0b10100000,
		0b00111111,0b11011111,0b11000000,
		0b01011111,0b10101111,0b10100000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b11100000,0b01110000,0b01110000,
		0b01011111,0b11011111,0b10100000,
		0b00111111,0b11011111,0b11000000,
		0b00011111,0b11011111,0b10000000,
	};
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 5; col++) {
			oled.DrawBitmap(24 * col, row * 32, bmp, 20, 29);
		}
	}
	oled.Refresh();
	for (;;) ;
}

void Test_DrawString(SSD1306& oled)
{
	const char* strTbl[] = {
		" !\"#$%&'()*+,-./",
		"0123456789:;<=>?",
		"",
		"",
		"@ABCDEFGHIJKLMNO",
		"PQRSTUVWXYZ[\\]^_",
		"`abcdefghijklmno",
		"pqrstuvwxy{|}~",
		"The quick brown",
		"fox jumps over",
		"the lazy dog",
		"",
		"あいうえお",
		"かきくけこ",
		"さしすせそ",
		"たちつてと",
		"親譲りの無鉄砲で",
		"小供の時から",
		"損ばかりしている",
		"",
	};
	const FontSet* fontSetTbl[] = {
		&Font::shinonome12, &Font::shinonome14, &Font::shinonome16,
	};
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		oled.SetDrawMode(drawMode);
		for (int iStrTop = 0; iStrTop < ArrayNumberOf(strTbl); iStrTop += 4) {
			for (int iFont = 0; iFont < ArrayNumberOf(fontSetTbl); iFont++) {
				oled.SetFont(*fontSetTbl[iFont]);
				SetupScreen(oled, drawMode);
				int y = 0;
				for (int iStr = iStrTop; iStr < iStrTop + 4 && iStr < ArrayNumberOf(strTbl); iStr++, y += 16) {
					oled.DrawString(0, y, strTbl[iStr]);
				}
				oled.Refresh();
				::sleep_ms(1000);
			}
		}
	}
}

void Test_DrawStringSISD(SSD1306& oled)
{
	oled.SetFont(Font::sisd24x32);
	for (int i = 0; i < 100; i++) {
		char str[16];
		::sprintf(str, "%5d", 12345 + i);
		oled.Clear();
		oled.DrawString(0, 0, str);
		oled.Refresh();
		::sleep_ms(300);
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
		//Test_Flash(oled);
		//Test_DrawPixel(oled);
		//Test_DrawHLine(oled);
		//Test_DrawVLine(oled);
		//Test_DrawLine(oled);
		//Test_DrawRect(oled);
		//Test_DrawRectFill(oled);
		//Test_DrawBitmap(oled);
		//Test_DrawString(oled);
		Test_DrawStringSISD(oled);
	}
}
