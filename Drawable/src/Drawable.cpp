//==============================================================================
// Drawable.cpp
//==============================================================================
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
const Drawable::DispatcherNone dispacherNone;

Drawable& Drawable::DrawLine(int x0, int y0, int x1, int y1, const Color& color)
{
	if (x0 == x1) {
		DrawVLine(x0, y0, y1 - y0, color);
		return *this;
	} else if (y0 == y1) {
		DrawHLine(x0, y0, x1 - x0, color);
		return *this;
	}
	int dx, dy, sx, sy;
	if (x0 < x1) {
		dx = x1 - x0;
		sx = +1;
	} else {
		dx = x0 - x1;
		sx = -1;
	}
	if (y0 < y1) {
		dy = y0 - y1;
		sy = +1;
	} else {
		dy = y1 - y0;
		sy = -1;
	}
	int err = dx + dy;
	int x = x0, y = y0;
	for (;;) {
		DrawPixel(x, y, color);
		if (x == x1 && y == y1) break;
		int err2 = 2 * err;
		if (err2 >= dy) {
			err += dy;
			x += sx;
		}
		if (err2 <= dx) {
			err += dx;
			y += sy;
		}
	}
	return *this;
}

Drawable& Drawable::DrawRect(int x, int y, int width, int height, const Color& color)
{
	if (width < 0) {
		x += width + 1;
		width = -width;
	}
	if (height < 0) {
		y += height + 1;
		height = -height;
	}
	DrawRectFill(x, y, width, 1, color);
	DrawRectFill(x, y + height - 1, width, 1, color);
	DrawRectFill(x, y, 1, height, color);
	DrawRectFill(x + width - 1, y, 1, height, color);
	return *this;
}

Drawable& Drawable::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
	Rect rectClipAdj;
	if (rectClip.IsEmpty()) {
		rectClipAdj = Rect(0, 0, image.GetWidth(), image.GetHeight());
	} else {
		rectClipAdj = rectClip;
		if (!rectClipAdj.Adjust({0, 0, image.GetWidth(), image.GetHeight()})) return *this;
	}
	GetDispatcher().DrawImage(x, y, image, rectClipAdj, drawDir);
	return *this;
}

Drawable& Drawable::DrawImageFast(int x, int y, const Image& image)
{
	GetDispatcher().DrawImageFast(x, y, image);
	return *this;
}

Drawable& Drawable::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	Rect rectClipAdj;
	if (rectClip.IsEmpty()) {
		rectClipAdj = Rect(0, 0, GetWidth(), GetHeight());
	} else {
		rectClipAdj = rectClip;
		if (!rectClipAdj.Adjust({0, 0, GetWidth(), GetHeight()})) return *this;
	}
	GetDispatcher().ScrollHorz(dirHorz, wdScroll, rectClipAdj);
	return *this;
}

Drawable& Drawable::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	Rect rectClipAdj;
	if (rectClip.IsEmpty()) {
		rectClipAdj = Rect(0, 0, GetWidth(), GetHeight());
	} else {
		rectClipAdj = rectClip;
		if (!rectClipAdj.Adjust({0, 0, GetWidth(), GetHeight()})) return *this;
	}
	GetDispatcher().ScrollVert(dirVert, htScroll, rectClipAdj);
	return *this;
}

Drawable& Drawable::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	DrawBitmap(x, y, fontEntry.data, fontEntry.width, fontEntry.height,
					true, context_.fontScaleWidth, context_.fontScaleHeight);
	return *this;
}

Drawable& Drawable::DrawChar(int x, int y, uint32_t code)
{
	if (!context_.pFontSet) return *this;
	const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
	return *this;
}

Drawable& Drawable::DrawString(int x, int y, const char* str, StringCont* pStringCont)
{
	if (!context_.pFontSet) {
		if (pStringCont) pStringCont->Update({x, y}, str);
		return *this;
	}
	uint32_t code;
	UTF8Decoder decoder;
	const char* p = str;
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		DrawChar(x, y, fontEntry);
		x += fontEntry.xAdvance * context_.fontScaleWidth;
	}
	if (pStringCont) pStringCont->Update({x, y}, p);
	return *this;
}

Drawable& Drawable::DrawStringWrap(int x, int y, int width, int height, const char* str, StringCont* pStringCont)
{
	const char* strDone = str;
	if (!context_.pFontSet) {
		if (pStringCont) pStringCont->Update({x, y}, strDone);
		return *this;
	}
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : width_;
	int yExceed = (height >= 0)? y + height : height_;
	int lineHeight = static_cast<int>(context_.pFontSet->yAdvance * context_.fontScaleHeight * context_.lineHeightRatio);
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int xAdvance = CalcAdvanceX(fontEntry);
		if (x + fontEntry.width * context_.fontScaleWidth > xExceed) {
			x = xStart, y += lineHeight;
			if (y + lineHeight > yExceed) break;
		}
		DrawChar(x, y, fontEntry);
		x += xAdvance;
		strDone = p + 1;
	}
	if (pStringCont) pStringCont->Update({x, y}, strDone);
	return *this;
}

Drawable& Drawable::DrawCross(int x, int y, int width, int height, int wdLine, int htLine)
{
	DrawRectFill(x - wdLine / 2, y - height / 2, wdLine, height);
	DrawRectFill(x - width / 2, y - htLine / 2, width, htLine);
	return *this;
}

}
