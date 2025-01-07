//==============================================================================
// Drawable_TestCase.cpp
//==============================================================================
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/sample/cat-60x80.h"
#include "jxglib/sample/cat-128x160.h"
#include "jxglib/sample/cat-240x240.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/cat-320x480.h"
#include "jxglib/Font/misaki_gothic.h"
#include "jxglib/Font/shinonome12-level1.h"
#include "jxglib/Font/shinonome14-level1.h"
#include "jxglib/Font/shinonome16-level1.h"
#include "jxglib/Font/shinonome18.h"
#include "jxglib/Font/unifont-level1.h"
#include "jxglib/Font/sisd8x16.h"
#include "jxglib/sample/Text_Botchan.h"

namespace jxglib {

template<typename T> T LimitNum(T num, T numMin, T numMax) { return (num < numMin)? numMin : (num > numMax)? numMax : num; }

//------------------------------------------------------------------------------
// Drawable_TestCase
//------------------------------------------------------------------------------
void Drawable_TestCase::DrawFonts(Drawable* drawableTbl[], int nDrawables)
{
	const FontSet* fontSetTbl[] = {
		&Font::misaki_gothic, &Font::shinonome12, &Font::shinonome14, &Font::shinonome16, &Font::shinonome18, &Font::unifont, &Font::sisd8x16,
	};
	for (int iDrawable = 0; iDrawable < nDrawables; iDrawable++) {
		Drawable& drawable = *drawableTbl[iDrawable];
		drawable.Clear();
		int y = 0;
		for (int i = 0; i < count_of(fontSetTbl); i++) {
			const FontSet& fontSet = *fontSetTbl[i];
			drawable.SetFont(fontSet);
			drawable.SetColor(Color::red);
			drawable.DrawString(0, y, fontSet.name);
			drawable.SetColor(Color::white);
			const char* str = "0123456789!\"#$%";
			drawable.DrawString(drawable.GetWidth() - fontSet.CalcStringWidth(str), y, str);
			y += fontSet.yAdvance;
			drawable.DrawString(0, y, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
			y += fontSet.yAdvance;
			drawable.DrawString(0, y, "abcdefghijklmnopqrstuvwxyz");
			y += fontSet.yAdvance;
		}
		drawable.Refresh();
	}
}

void Drawable_TestCase::DrawString(Drawable* drawableTbl[], int nDrawables)
{
	const char* msg =
		"[J], [K] .. change font\n"
		"[U], [I] .. change font scale\n"
		"[N], [M] .. change line height\n";
		"[<], [>] .. change character width\n";
	const FontSet* fontSetTbl[] = { &Font::shinonome12, &Font::shinonome14, &Font::shinonome16 };
	int iFont = 2;
	int fontScale = 1;
	float charWidthRatio = 1.0, lineHeightRatio = 1.2;
	for (;;) {
		const FontSet& fontSet = *fontSetTbl[iFont];
		for (int iDrawable = 0; iDrawable < nDrawables; iDrawable++) {
			Drawable& drawable = *drawableTbl[iDrawable];
			drawable.Clear()
				.SetFont(fontSet)
				.SetFontScale(fontScale)
				.SetSpacingRatio(charWidthRatio, lineHeightRatio)
				.DrawStringWrap(0, 0, Text_Botchan)
				.Refresh();
		}
		::printf(msg);
		switch (::getchar()) {
		case 'j': iFont = LimitNum(iFont - 1, 0, static_cast<int>(count_of(fontSetTbl)) - 1); break;
		case 'k': iFont = LimitNum(iFont + 1, 0, static_cast<int>(count_of(fontSetTbl)) - 1); break;
		case 'u': fontScale = LimitNum(fontScale - 1, 1, 3); break;
		case 'i': fontScale = LimitNum(fontScale + 1, 1, 3); break;
		case 'n': lineHeightRatio = LimitNum(lineHeightRatio - .1, 1., 3.); break;
		case 'm': lineHeightRatio = LimitNum(lineHeightRatio + .1, 1., 3.); break;
		case ',': charWidthRatio = LimitNum(charWidthRatio - .1, 1., 3.); break;
		case '.': charWidthRatio = LimitNum(charWidthRatio + .1, 1., 3.); break;
		default: break;
		}
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
			const Image& image =
				(drawable.GetWidth() < 240)? image_cat_128x160 :
				(drawable.GetWidth() < 320)? image_cat_240x320 :
				image_cat_320x480;
			//const Image& image = image_cat_60x80;
			drawable.Clear().DrawImage(0, 0, image, Rect::Empty, testCase.dir).Refresh();
			//drawable.Clear().DrawImage(40, 20, image, {80, 100, 500, 500}, testCase.dir).Refresh();
		}
		::getchar();
	}
}

#if 0
void Test_DrawPixel(Drawable& drawable)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.Fill(blackDrawFlag? Color::white : Color::black);
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		int x = 40, y = 16;
		int xDir = +1, yDir = +1;
		for (int i = 0; i < 1000; i++) {
			drawable.DrawPixel(x, y);
			if (x + xDir < 0) xDir = +1;
			if (x + xDir >= drawable.GetWidth()) xDir = -1;
			if (y + yDir < 0) yDir = +1;
			if (y + yDir >= drawable.GetHeight()) yDir = -1;
			x += xDir, y += yDir;
			if (i % 10 == 0) drawable.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawHLine(Drawable& drawable)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int x = -64; x <= drawable.GetWidth(); x++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			for (int i = 0; i < drawable.GetHeight(); i++) {
				drawable.DrawHLine(x, i, i);
			}
			drawable.Refresh();
		}
		::sleep_ms(1000);
	}
}

void Test_DrawVLine(Drawable& drawable)
{
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = -64; y <= drawable.GetHeight(); y++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			for (int i = 0; i < drawable.GetHeight(); i++) {
				drawable.DrawVLine(i, y, i);
				drawable.DrawVLine(i + drawable.GetHeight() * 1, drawable.GetHeight() - y - 1, -i);
			}
			drawable.Refresh();
		}
		::sleep_ms(1000);
	}
}
#endif

void Drawable_TestCase::DrawLine(Drawable& drawable)
{
	int xMid = drawable.GetWidth() / 2;
	int yMid = drawable.GetHeight() / 2;
	int xRight = drawable.GetWidth() - 1;
	int yBottom = drawable.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int y = 0; y < drawable.GetHeight(); y++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawLine(xMid, yMid, xRight, y);
			drawable.Refresh();
			//::sleep_ms(10);
		}
		for (int x = xRight; x >= 0; x--) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawLine(xMid, yMid, x, yBottom);
			drawable.Refresh();
			//::sleep_ms(10);
		}
		for (int y = yBottom; y >= 0; y--) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawLine(xMid, yMid, 0, y);
			drawable.Refresh();
			//::sleep_ms(10);
		}
		for (int x = 0; x <= xRight; x++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawLine(xMid, yMid, x, 0);
			drawable.Refresh();
			//::sleep_ms(10);
		}
	}
}

#if 0
void Test_DrawRect(Drawable& drawable)
{
	int xRight = drawable.GetWidth() - 1;
	int yBottom = drawable.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < drawable.GetHeight(); i++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawRect(0, 0, i * 2, i);
			drawable.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < drawable.GetHeight(); i++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawRect(xRight, yBottom, -i * 2, -i);
			drawable.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_DrawRectFill(Drawable& drawable)
{
	int xRight = drawable.GetWidth() - 1;
	int yBottom = drawable.GetHeight() - 1;
	bool blackDrawFlag = false;
	for (int i = 0; i < 2; i++, blackDrawFlag = !blackDrawFlag) {
		drawable.SetColor(blackDrawFlag? Color::black : Color::white);
		for (int i = 0; i < drawable.GetHeight(); i++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawRectFill(0, 0, i * 2, i);
			drawable.Refresh();
			::sleep_ms(100);
		}
		for (int i = 0; i < drawable.GetHeight(); i++) {
			drawable.Fill(blackDrawFlag? Color::white : Color::black);
			drawable.DrawRectFill(xRight, drawable.GetHeight() - 1, -i * 2, -i);
			drawable.Refresh();
			::sleep_ms(100);
		}
	}
}

void Test_BouncingBall(Drawable& drawable)
{
	int x = 10, y = 10;
	int xDir = 1, yDir = 1;
	int wdBall = 50, htBall = 30;
	for (;;) {
		drawable.SetColor(Color::white);
		drawable.DrawRectFill(x, y, wdBall, htBall);
		if (x + xDir < 0) xDir = 1;
		if (x + xDir + wdBall > drawable.GetWidth()) xDir = -1;
		if (y + yDir < 0) yDir = 1;
		if (y + yDir + htBall > drawable.GetHeight()) yDir = -1;
		::sleep_ms(1);
		drawable.SetColor(Color::black);
		drawable.DrawRectFill(x, y, wdBall, htBall);
		x += xDir, y += yDir;
	}
}

void Test_DrawString(Drawable& drawable)
{
	drawable.Clear();
	drawable.SetFont(Font::shinonome16);
	drawable.SetFontScale(2, 2);
	for (int i = 0; ; i++) {
		drawable.DrawString(0, 0, "Hello World");
		//drawable.SetFontScale(1, 1);
		char str[32];
		::sprintf(str, "こんにちは:%d", i);
		drawable.DrawString(0, 32, str);
		::sprintf(str, "こんにちは:%d", i + 1);
		drawable.DrawString(0, 32 * 2, str);
		::sprintf(str, "こんにちは:%d", i + 2);
		drawable.DrawString(0, 32 * 3, str);
		::sprintf(str, "こんにちは:%d", i + 3);
		drawable.DrawString(0, 32 * 4, str);
		::sprintf(str, "こんにちは:%d", i + 4);
		drawable.DrawString(0, 32 * 5, str);
	}
}

void Test_DrawImage(Drawable* displayTbl[], int nDisplays)
{
	static const Drawable::ImageDir imageDirTbl[] = {
		Drawable::ImageDir::Rotate0, Drawable::ImageDir::Rotate90,
		Drawable::ImageDir::Rotate180, Drawable::ImageDir::Rotate270,
	};
	int iDir = 0;
	//for (;;) {
	//	for (int iDir = 0; iDir < count_of(imageDirTbl); iDir++) {
			Drawable::ImageDir imageDir = imageDirTbl[iDir];
			for (int iDisplay = 0; iDisplay < nDisplays; iDisplay++) {
				Drawable& drawable = *displayTbl[iDisplay];
				int width = drawable.GetWidth();
				const Image& image = (width <= 128)? image_cat_128x160 : image_cat_240x320;
				drawable.Clear().DrawImage(0, 0, image, nullptr, imageDir);
			}
	//		::sleep_ms(500);
	//	}
	//}
}

void Test_DrawStringWrap(Drawable* displayTbl[], int nDisplays)
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
		Drawable& drawable = *displayTbl[iDisplay];
		drawable.SetColor(Color::white);
		drawable.SetColorBg(Color::black);
		drawable.SetSpacingRatio(1.0, 1.2);
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
			Drawable& drawable = *displayTbl[iDisplay];
			//int fontScale = (drawable.GetWidth() >= 240)? 2 : 1;
			int fontScale = 1;
			drawable.SetFont(*fontSetTbl[iFont], fontScale);
			drawable.Clear();
			drawable.DrawStringWrap(0, 0, p);
			drawable.Refresh();
		}
		::sleep_ms(1000);
	}
}

#endif

}
