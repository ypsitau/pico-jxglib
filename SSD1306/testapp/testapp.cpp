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

void SetupScreen(SSD1306& screen, SSD1306::DrawMode drawMode)
{
	switch (drawMode) {
	case SSD1306::DrawMode::Set:
		screen.Clear();
		break;
	case SSD1306::DrawMode::Clear:
		screen.Clear(0xff);
		break;
	case SSD1306::DrawMode::Invert:
		screen.Clear();
		screen.DrawRectFill(16, 16, 96, 32);
		break;
	default:
		break;
	}
}

void Test_Flash(SSD1306& screen)
{
	for (int i = 0; i < 3; i++) {
		screen.Flash(true);
		::sleep_ms(500);
		screen.Flash(false);
		::sleep_ms(500);
	}
}

void Test_DrawPixel(SSD1306& screen)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		SetupScreen(screen, drawMode);
		int x = 40, y = 16;
		int xDir = +1, yDir = +1;
		for (int i = 0; i < 1000; i++) {
			screen.DrawPixel(x, y);
			if (x + xDir < 0) xDir = +1;
			if (x + xDir >= screen.GetScreenWidth()) xDir = -1;
			if (y + yDir < 0) yDir = +1;
			if (y + yDir >= screen.GetScreenHeight()) yDir = -1;
			x += xDir, y += yDir;
			if (i % 10 == 0) screen.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawHLine(SSD1306& screen)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		for (int x = -64; x <= screen.GetScreenWidth(); x++) {
			SetupScreen(screen, drawMode);
			for (int i = 0; i < screen.GetScreenHeight(); i++) {
				screen.DrawHLine(x, i, i);
			}
			screen.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawVLine(SSD1306& screen)
{
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		for (int y = -64; y <= screen.GetScreenHeight(); y++) {
			SetupScreen(screen, drawMode);
			for (int i = 0; i < screen.GetScreenHeight(); i++) {
				screen.DrawVLine(i, y, i);
				screen.DrawVLine(i + screen.GetScreenHeight() * 1, screen.GetScreenHeight() - y - 1, -i);
			}
			screen.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawLine(SSD1306& screen)
{
	int xMid = screen.GetScreenWidth() / 2;
	int yMid = screen.GetScreenHeight() / 2;
	int xRight = screen.GetScreenWidth() - 1;
	int yBottom = screen.GetScreenHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		for (int y = 0; y < screen.GetScreenHeight(); y++) {
			SetupScreen(screen, drawMode);
			screen.DrawLine(xMid, yMid, xRight, y);
			screen.Refresh();
			::sleep_ms(10);
		}
		for (int x = xRight; x >= 0; x--) {
			SetupScreen(screen, drawMode);
			screen.DrawLine(xMid, yMid, x, yBottom);
			screen.Refresh();
			::sleep_ms(10);
		}
		for (int y = yBottom; y >= 0; y--) {
			SetupScreen(screen, drawMode);
			screen.DrawLine(xMid, yMid, 0, y);
			screen.Refresh();
			::sleep_ms(10);
		}
		for (int x = 0; x <= xRight; x++) {
			SetupScreen(screen, drawMode);
			screen.DrawLine(xMid, yMid, x, 0);
			screen.Refresh();
			::sleep_ms(10);
		}
	}
}

void Test_DrawRect(SSD1306& screen)
{
	int xRight = screen.GetScreenWidth() - 1;
	int yBottom = screen.GetScreenHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		for (int i = 0; i < screen.GetScreenHeight(); i++) {
			SetupScreen(screen, drawMode);
			screen.DrawRect(0, 0, i * 2, i);
			screen.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < screen.GetScreenHeight(); i++) {
			SetupScreen(screen, drawMode);
			screen.DrawRect(xRight, yBottom, -i * 2, -i);
			screen.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawRectFill(SSD1306& screen)
{
	int xRight = screen.GetScreenWidth() - 1;
	int yBottom = screen.GetScreenHeight() - 1;
	for (int i = 0; i < 3; i++) {
		SSD1306::DrawMode drawMode = static_cast<SSD1306::DrawMode>(i);
		screen.SetDrawMode(drawMode);
		for (int i = 0; i < screen.GetScreenHeight(); i++) {
			SetupScreen(screen, drawMode);
			screen.DrawRectFill(0, 0, i * 2, i);
			screen.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < screen.GetScreenHeight(); i++) {
			SetupScreen(screen, drawMode);
			screen.DrawRectFill(xRight, screen.GetScreenHeight() - 1, -i * 2, -i);
			screen.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawBitmap(SSD1306& screen)
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
	screen.DrawBitmap(0, 0, bmp, 80, 64);
	screen.Refresh();
	::sleep_ms(2000);
}

void Test_DrawStringWrap(SSD1306& screen)
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
		screen.SetDrawMode(drawMode);
		screen.SetFont(*fontSetTbl[iFont]);
		SetupScreen(screen, drawMode);
		p = screen.DrawStringWrap(0, 0, p);
		screen.Refresh();
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
	SSD1306 screen(i2c_default);
	screen.Initialize();
	for (;;) {
		//Test_Flash(screen);
		//Test_DrawPixel(screen);
		Test_DrawHLine(screen);
		Test_DrawVLine(screen);
		//Test_DrawLine(screen);
		Test_DrawRect(screen);
		Test_DrawRectFill(screen);
		Test_DrawBitmap(screen);
		Test_DrawStringWrap(screen);
	}
}
