#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"
#include "jxglib/sample/cat-240x240.h"
#include "jxglib/sample/cat-240x320.h"
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
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display1(spi0, 240, 240, GPIO7, GPIO8, GPIO9);
	ST7789 display2(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	display1.Initialize();
	display2.Initialize();
	Canvas canvas1;
	Canvas canvas2;
#if 0
	canvas.AttachOutput(display, Canvas::ImageDir::Rotate270);
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	canvas.Refresh();
#endif
#if 1
	canvas1.AttachOutput(display1);
	canvas2.AttachOutput(display2);
	static const Canvas::ImageDir imageDirTbl[] = {
		Canvas::ImageDir::Rotate0, Canvas::ImageDir::Rotate90, Canvas::ImageDir::Rotate180, Canvas::ImageDir::Rotate270,
	};
	for (int i = 0; ; i++) {
		if (i >= count_of(imageDirTbl)) i = 0;
		canvas1.DrawImage(0, 0, image_cat_240x240, nullptr, imageDirTbl[i]);
		canvas1.Refresh();
		canvas2.DrawImage(0, 0, image_cat_240x320, nullptr, imageDirTbl[i]);
		canvas2.Refresh();
		::sleep_ms(1000);
	}
#endif
#if 0
	//canvas.AttachOutput(display, Rect(40, 40, 100, 150), Canvas::ImageDir::Rotate90);
	canvas.AttachOutput(display);
	canvas.SetFont(Font::shinonome16);
	canvas.SetSpacingRatio(1.0, 1.2);
	canvas.DrawStringWrap(0, 0, str);
	//canvas.Refresh();
	absolute_time_t absTimeStart = ::get_absolute_time();
	canvas.Refresh();
	::printf("%lld usec\n", ::absolute_time_diff_us(absTimeStart, ::get_absolute_time()));
#endif
}
