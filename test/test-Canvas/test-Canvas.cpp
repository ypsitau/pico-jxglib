#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"
#include "image/image_cat_240x320.h"
#include "Font/shinonome16.h"

using namespace jxglib;

int main()
{
	const char* str =
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。";
	stdio_init_all();
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	display.Initialize();
	Canvas canvas;
	canvas.AttachOutput(display);
#if 0
	canvas.AttachOutput(display, Canvas::ImageDir::Rotate0);
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	canvas.Refresh();
	//canvas.Clear();
#endif
#if 0
	for (;;) {
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate0);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate90);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate180);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate270);
		canvas.Refresh();
	}
#endif
#if 1
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	canvas.Refresh();
	for (;;) {
		canvas.ScrollHorz(DirHorz::Right, 1);
		canvas.Refresh();
	}
#endif
#if 0
	for (;;) {
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate0);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate90);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate180);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate270);
		canvas.Refresh();
	}
#endif
}
