#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"
#include "jxglib/ST7735.h"
#include "jxglib/ILI9341.h"
#include "jxglib/sample/cat-240x240.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/cat-128x160.h"
#include "Font/shinonome16.h"

using namespace jxglib;

void DrawImage(Drawable* drawableTbl[], int nDisplays)
{
	struct TestCase {
		const char* name;
		Drawable::DrawDir dir;
	} testCaseTbl[] = {
		{ "Rotate 0", Drawable::DrawDir::Rotate0 },
		{ "Rotate 90", Drawable::DrawDir::Rotate90 },
		{ "Rotate 180", Drawable::DrawDir::Rotate180 },
		{ "Rotate 270", Drawable::DrawDir::Rotate270 },
	};
	for (int iTestCase = 0; ; iTestCase++) {
		if (iTestCase >= count_of(testCaseTbl)) iTestCase = 0;
		TestCase& testCase = testCaseTbl[iTestCase];
		::printf("%-20s Press Any Key\n", testCase.name);
		for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
			Drawable& drawable = *drawableTbl[iDisplay];
			//const Image& image = (drawable.GetWidth() < 240)? image_cat_128x160 : image_cat_240x240;
			//const Image& image = image_cat_60x80;
			const Image& image = image_cat_240x320;
			//drawable.Clear().DrawImage(0, 0, image, Rect::Empty, testCase.dir).Refresh();
			drawable.Clear().DrawImage(40, 20, image, {80, 100, 500, 500}, testCase.dir).Refresh();
		}
		::getchar();
	}
}

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	ILI9341 display(spi1, 240, 320, GPIO11, GPIO12, GPIO13, GPIO::None);
	Display::Dir displayDir = Display::Dir::Rotate0;
	display.Initialize(displayDir);
	Canvas canvas;
	canvas.AttachOutput(display, Canvas::DrawDir::Rotate0);
	//canvas.AttachOutput(display, {50, 50, 100, 150}, Canvas::AttachDir::Rotate0);
#if 0
	const char* str =
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。";
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	canvas.Refresh();
#endif
#if 1
	Drawable* drawableTbl[] = { &canvas };
	DrawImage(drawableTbl, count_of(drawableTbl));
	//canvas.DrawImage(0, 0, image_cat_128x160, {10, 10, 100, 60}, Canvas::DrawDir::Rotate0);
	//canvas.Refresh();
#endif
}

#if 0
int main()
{
	const char* str =
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。";
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_I2C0_SDA();
	GPIO5.set_function_I2C0_SCL();
	GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	ST7789 display1(spi0, 240, 240, GPIO6, GPIO7, GPIO::None);
	ST7789 display2(spi1, 240, 320, GPIO8, GPIO9, GPIO10, GPIO::None);
	ILI9341 display3(spi1, 240, 320, GPIO11, GPIO12, GPIO13, GPIO::None);
	ST7735 display4(spi1, 80, 160, GPIO16, GPIO17, GPIO18, GPIO::None);
	ST7735::TypeB display5(spi1, 128, 160, GPIO19, GPIO20, GPIO21, GPIO::None);
	Display::Dir displayDir = Display::Dir::Rotate0;
	//Display::Dir displayDir = Display::Dir::Rotate90;
	//Display::Dir displayDir = Display::Dir::Rotate180;
	//Display::Dir displayDir = Display::Dir::Rotate270;
	display1.Initialize(displayDir);
	display2.Initialize(displayDir);
	display3.Initialize(displayDir);
	display4.Initialize(displayDir);
	display5.Initialize(displayDir);
	Canvas canvas;
#if 0
	canvas.AttachOutput(display, Canvas::DrawDir::Rotate270);
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	canvas.Refresh();
#endif
#if 1
	canvas.AttachOutput(display3, {50, 50, 100, 150}, Canvas::AttachDir::Rotate0);
	//canvas.AttachOutput(display2);
	//canvas.AttachOutput(display3);
	//canvas.AttachOutput(display4);
	//canvas.AttachOutput(display5);
	//static const Canvas::DrawDir* drawDirTbl[] = {
	//	&Canvas::DrawDir::Rotate0, &Canvas::DrawDir::Rotate90, &Canvas::DrawDir::Rotate180, &Canvas::DrawDir::Rotate270,
	//};
	//for (int i = 0; ; i++) {
	//	if (i >= count_of(drawDirTbl)) i = 0;
	//	//canvas1.DrawImage(0, 0, image_cat_240x240, nullptr, drawDirTbl[i]);
	//	//canvas1.Refresh();
	//	canvas.DrawImage(0, 0, image_cat_240x320, nullptr, *drawDirTbl[i]);
	//	canvas.Refresh();
	//	::sleep_ms(1000);
	//}
	//canvas.DrawImage(0, 0, image_cat_128x160, nullptr, Canvas::DrawDir::Rotate0);
	canvas.DrawImage(0, 0, image_cat_128x160, Rect::Empty, Canvas::DrawDir::Rotate0);
	canvas.Refresh();
#endif
#if 0
	//canvas.AttachOutput(display, Rect(40, 40, 100, 150), Canvas::DrawDir::Rotate90);
	canvas1.AttachOutput(display1);
	canvas.AttachOutput(display2);
	canvas1.SetFont(Font::shinonome16);
	canvas.SetFont(Font::shinonome16);
	canvas1.SetSpacingRatio(1.0, 1.2);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas1.DrawStringWrap(0, 0, str);
	canvas.DrawStringWrap(0, 0, str);
	//canvas.Refresh();
	//absolute_time_t absTimeStart = ::get_absolute_time();
	canvas1.Refresh();
	canvas.Refresh();
	//::printf("%lld usec\n", ::absolute_time_diff_us(absTimeStart, ::get_absolute_time()));
#endif
}
#endif
