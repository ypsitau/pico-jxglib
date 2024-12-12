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
#include "jxglib/sample/cat-128x160.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/cat-240x240.h"

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

void Test_DrawImage(Display* displayTbl[], int nDisplays)
{
	static const Display::ImageDir imageDirTbl[] = {
		Display::ImageDir::Rotate0, Display::ImageDir::Rotate90,
		Display::ImageDir::Rotate180, Display::ImageDir::Rotate270,
	};
	int iDir = 0;
	//for (;;) {
	//	for (int iDir = 0; iDir < count_of(imageDirTbl); iDir++) {
			Display::ImageDir imageDir = imageDirTbl[iDir];
			for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
				Display& display = *displayTbl[iDisplay];
				int width = display.GetWidth();
				const Image& image = (width <= 128)? image_cat_128x160 : image_cat_240x320;
				display.Clear().DrawImage(0, 0, image, nullptr, imageDir);
			}
	//		::sleep_ms(500);
	//	}
	//}
}

void Test_DrawStringWrap(Display* displayTbl[], int nDisplays)
{
	const char* strTbl[] = {
		" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxy{|}~",
		"The quick brown fox jumps over the lazy dog",
		"あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん",
		"アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン",
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。"
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
		display.SetSpacingRatio(1.0, 1.2);
	}
	for (;;) {
		//if (!*p) {
		//	if (iStr >= count_of(strTbl) || ((*strTbl[iStr] & 0x80 != 0) && !fontSetTbl[iFont]->HasExtraFont())) {
		//		iStr = 0;
		//		iFont++;
		//		if (iFont >= count_of(fontSetTbl)) {
		//			iFont = 0;
		//		}
		//	}
		//	p = strTbl[iStr++];
		//}
		p = strTbl[iStr++];
		if (iStr >= count_of(strTbl)) iStr = 0;
		for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
			Display& display = *displayTbl[iDisplay];
			//int fontScale = (display.GetWidth() >= 240)? 2 : 1;
			int fontScale = 1;
			display.SetFont(*fontSetTbl[iFont], fontScale);
			display.Clear();
			display.DrawStringWrap(0, 0, p);
			display.Refresh();
		}
		::sleep_ms(1000);
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
	//GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	//GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO16.set_function_I2C0_SDA();
	GPIO17.set_function_I2C0_SCL();
	ST7789 display1(spi0, 240, 240, GPIO4, GPIO5, GPIO6);
	ST7789 display2(spi1, 240, 320, GPIO7, GPIO8, GPIO9, GPIO::None);
	ILI9341 display3(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO::None);
	ST7735 display4(spi1, 80, 160, GPIO18, GPIO19, GPIO20, GPIO::None);
	ST7735::TypeB display5(spi1, 128, 160, GPIO21, GPIO22, GPIO23, GPIO::None);
	Display* displayTbl[] = { &display1, &display2, &display3, &display4, &display5 };
	//Display* displayTbl[] = { &display1 };
	Display::DisplayDir displayDir = Display::DisplayDir::Normal;
	//Display::DisplayDir displayDir = Display::DisplayDir::Rotate90;
	//Display::DisplayDir displayDir = Display::DisplayDir::Rotate180;
	display1.Initialize(displayDir);
	display2.Initialize(displayDir);
	display3.Initialize(displayDir);
	display4.Initialize(displayDir);
	display5.Initialize(displayDir);
	//Test_BouncingBall(display);
	//Test_WriteBuffer(display);
	//Test_DrawString(display);
	//Test_DrawStringWrap(display);
	//Test_DrawImage(displayTbl, count_of(displayTbl));
	//Test_DrawLine(display2);
	//Test_DrawStringWrap(displayTbl, count_of(displayTbl));
	//display2.raw.VerticalScrollingDefinition(0, 320, 0);
	//display2.raw.VerticalScrollStartAddressToRAM(100);
	//display2.SetFont(Font::shinonome16, 3);
	//display2.DrawString(0, 0, "Hello");
	//for (int i = 0; i < 320; i++) {
	//	display2.raw.VerticalScrollStartAddressToRAM(i);
	//	::sleep_ms(100);
	//}
}
