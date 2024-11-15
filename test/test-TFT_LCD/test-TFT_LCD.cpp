#include <stdio.h>
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/Common.h"
#include "Font/shinonome12.h"
#include "Font/shinonome14.h"
#include "Font/shinonome16.h"
#include "Font/shinonome18.h"
#include "Font/sisd8x16.h"
#include "Font/sisd24x32.h"
#include "image/image_cat_128x170.h"
#include "image/image_cat_240x320.h"

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

using namespace jxglib;

void Test_DrawPixel(Display& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.Fill(blackDrawFlag? Color::white : Color::black);
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

void Test_DrawHLine(Display& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int x = -64; x <= display.GetWidth(); x++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			for (int i = 0; i < display.GetHeight(); i++) {
				display.DrawHLine(x, i, i);
			}
			display.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawVLine(Display& display)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = -64; y <= display.GetHeight(); y++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			for (int i = 0; i < display.GetHeight(); i++) {
				display.DrawVLine(i, y, i);
				display.DrawVLine(i + display.GetHeight() * 1, display.GetHeight() - y - 1, -i);
			}
			display.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawLine(Display& display)
{
	int xMid = display.GetWidth() / 2;
	int yMid = display.GetHeight() / 2;
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = 0; y < display.GetHeight(); y++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawLine(xMid, yMid, xRight, y);
			display.Refresh();
			//::sleep_ms(10);
		}
		for (int x = xRight; x >= 0; x--) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawLine(xMid, yMid, x, yBottom);
			display.Refresh();
			//::sleep_ms(10);
		}
		for (int y = yBottom; y >= 0; y--) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawLine(xMid, yMid, 0, y);
			display.Refresh();
			//::sleep_ms(10);
		}
		for (int x = 0; x <= xRight; x++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawLine(xMid, yMid, x, 0);
			display.Refresh();
			//::sleep_ms(10);
		}
	}
}

void Test_DrawRect(Display& display)
{
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawRect(0, 0, i * 2, i);
			display.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawRect(xRight, yBottom, -i * 2, -i);
			display.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawRectFill(Display& display)
{
	int xRight = display.GetWidth() - 1;
	int yBottom = display.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		display.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawRectFill(0, 0, i * 2, i);
			display.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < display.GetHeight(); i++) {
			display.Fill(blackDrawFlag? Color::white : Color::black);
			display.DrawRectFill(xRight, display.GetHeight() - 1, -i * 2, -i);
			display.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_BouncingBall(Display& display)
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

void Test_DrawString(Display& display)
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

//void Test_DrawStringWrap(Display& display, int fontScale)
void Test_DrawStringWrap(Display* displayTbl[], int nDisplays, int fontScale)
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
	for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
		Display& display = *displayTbl[iDisplay];
		display.SetColor(Color::white);
		display.SetColorBg(Color::black);
	}
	for (;;) {
		//if (!*p) {
		//	if (iStr >= ArrayNumberOf(strTbl) || ((*strTbl[iStr] & 0x80 != 0) && !fontSetTbl[iFont]->HasExtraFont())) {
		//		iStr = 0;
		//		iFont++;
		//		if (iFont >= ArrayNumberOf(fontSetTbl)) {
		//			iFont = 0;
		//		}
		//	}
		//	p = strTbl[iStr++];
		//}
		p = strTbl[iStr++];
		if (iStr >= ArrayNumberOf(strTbl)) iStr = 0;
		for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
			Display& display = *displayTbl[iDisplay];
			display.SetFont(*fontSetTbl[iFont], fontScale);
			display.Clear();
			display.DrawStringWrap(0, 0, p);
			display.Refresh();
		}
		::sleep_ms(200);
	}
}

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display1(spi0, 240, 240, GPIO4, GPIO5, GPIO6);
	ST7789 display2(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	ILI9341 display3(spi1, 240, 320, GPIO16, GPIO17, GPIO18, GPIO19);
	ST7735::TypeB display4(spi1, 130, 161, GPIO20, GPIO21, GPIO22, GPIO23);
	ST7735 display5(spi1, 128, 160, GPIO24, GPIO25, GPIO26, GPIO27);
	Display* displayTbl[] = { &display1, &display2, &display3, &display4, &display5 };
	display1.Initialize();
	display2.Initialize();
	display3.Initialize();
	display4.Initialize();
	display5.Initialize();
	//Test_BouncingBall(display);
	//Test_WriteBuffer(display);
	//Test_DrawString(display);
	//Test_DrawStringWrap(display);
	display1.Clear().DrawImage(0, 0, image_cat_240x320);
	display2.Clear().DrawImage(0, 0, image_cat_240x320);
	display3.Clear().DrawImage(0, 0, image_cat_240x320);
	display4.Clear().DrawImage(0, 0, image_cat_128x170);
	display5.Clear().DrawImage(0, 0, image_cat_128x170);
	//Test_DrawLine(display2);
	//Test_DrawStringWrap(displayTbl, ArrayNumberOf(displayTbl), 1);
}
