//==============================================================================
// Drawable_TestCase.cpp
//==============================================================================
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ST7789.h"
#include "jxglib/ST7735.h"
#include "jxglib/ILI9341.h"
#include "jxglib/sample/cat-240x240.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/cat-128x160.h"
#include "Font/shinonome16.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable_TestCase
//------------------------------------------------------------------------------
void Drawable_TestCase::DrawString(Drawable* drawableTbl[], int nDrawables)
{
	const char* str =
		"親譲りの無鉄砲で子供のときから損ばかりしている。小学校の時分学校の二階から飛び降りて一週間腰を"
		"抜かしたことがある。なぜそんなむやみなことをしたかと聞く人があるかもしれない。"
		"別段深い理由でもない。新築の二階から首を出していたら同級生の一人が冗談に幾ら威張っても、"
		"そこから飛び降りることはできまい。弱虫やーい。とはやしたからである。"
		"小使いに負ぶさって帰ってきたとき、親父が大きな眼をして二階から飛び降りて腰を抜かすやつが"
		"あるかといったから、この次は抜かさずに飛んでみせますと答えた。";
	for (int iDrawable = 0; iDrawable < nDrawables; iDrawable++) {
		Drawable& drawable = *drawableTbl[iDrawable];
		drawable.SetFont(Font::shinonome16);
		drawable.SetSpacingRatio(1.0, 1.2);
		drawable.DrawStringWrap(0, 0, str);
		drawable.Refresh();
	}
}

void Drawable_TestCase::RotateImage(Drawable* drawableTbl[], int nDrawables)
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
		for (int iDrawable = 0; iDrawable < nDrawables; iDrawable++) {
			Drawable& drawable = *drawableTbl[iDrawable];
			//const Image& image = (drawable.GetWidth() < 240)? image_cat_128x160 : image_cat_240x240;
			//const Image& image = image_cat_60x80;
			const Image& image = image_cat_240x320;
			//drawable.Clear().DrawImage(0, 0, image, Rect::Empty, testCase.dir).Refresh();
			drawable.Clear().DrawImage(40, 20, image, {80, 100, 500, 500}, testCase.dir).Refresh();
		}
		::getchar();
	}
}

}
