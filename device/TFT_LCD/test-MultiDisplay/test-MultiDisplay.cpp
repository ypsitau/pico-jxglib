#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/sample/cat-128x160.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/cat-240x240.h"
#include "Font/shinonome12.h"
#include "Font/shinonome16.h"

using namespace jxglib;

void DrawImage(Display* displayTbl[], int nDisplays)
{
	for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
		Display& display = *displayTbl[iDisplay];
		const Image& image = (display.GetWidth() < 240)? image_cat_128x160 : image_cat_240x240;
		display.Clear().DrawImage(0, 0, image, nullptr, Display::Dir::Rotate0).Refresh();
	}
}

void DrawText(Display* displayTbl[], int nDisplays)
{
	const char* str =
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。";
	for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
		Display& display = *displayTbl[iDisplay];
		const FontSet& fontSet = (display.GetWidth() < 240)? Font::shinonome12 : Font::shinonome16;
		display.SetFont(fontSet);
		display.SetColor(Color::white).SetColorBg(Color::black).SetSpacingRatio(1.0, 1.2);
		display.DrawStringWrap(0, 0, str).Refresh();
	}
}

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO16.set_function_I2C0_SDA();
	GPIO17.set_function_I2C0_SCL();
	ST7789 display1(spi0, 240, 240, GPIO4, GPIO5, GPIO6);
	ST7789 display2(spi1, 240, 320, GPIO7, GPIO8, GPIO9, GPIO::None);
	ILI9341 display3(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO::None);
	ST7735 display4(spi1, 80, 160, GPIO18, GPIO19, GPIO20, GPIO::None);
	ST7735::TypeB display5(spi1, 128, 160, GPIO21, GPIO22, GPIO23, GPIO::None);
	//Display::Dir displayDir = Display::Dir::Normal;
	//Display::Dir displayDir = Display::Dir::Rotate90;
	//Display::Dir displayDir = Display::Dir::Rotate180;
	Display::Dir displayDir = Display::Dir::Rotate270;
	display1.Initialize(displayDir);
	display2.Initialize(displayDir);
	display3.Initialize(displayDir);
	display4.Initialize(displayDir);
	display5.Initialize(displayDir);
	Display* displays[] = { &display1, &display2, &display3, &display4, &display5 };
	//DrawImage(displays, count_of(displays));
	DrawText(displays, count_of(displays));
}
