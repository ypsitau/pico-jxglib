//==============================================================================
// Drawable.cpp
//==============================================================================
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
Drawable& Drawable::DrawLine(int x0, int y0, int x1, int y1)
{
	if (x0 == x1) {
		DrawVLine(x0, y0, y1 - y0);
		return *this;
	} else if (y0 == y1) {
		DrawHLine(x0, y0, x1 - x0);
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
		DrawPixel(x, y);
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

Drawable& Drawable::DrawRect(int x, int y, int width, int height)
{
	if (width < 0) {
		x += width + 1;
		width = -width;
	}
	if (height < 0) {
		y += height + 1;
		height = -height;
	}
	DrawRectFill(x, y, width, 1);
	DrawRectFill(x, y + height - 1, width, 1);
	DrawRectFill(x, y, 1, height);
	DrawRectFill(x + width - 1, y, 1, height);
	return *this;
}

Drawable& Drawable::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	DrawBitmap(x, y, fontEntry.data, fontEntry.width, fontEntry.height,
					true, context_.fontScaleX, context_.fontScaleY);
	return *this;
}

Drawable& Drawable::DrawChar(int x, int y, uint32_t code)
{
	if (!context_.pFontSet) return *this;
	const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
	return *this;
}

Drawable& Drawable::DrawString(int x, int y, const char* str, const char* strEnd, StringCont* pStringCont)
{
	if (!context_.pFontSet) {
		if (pStringCont) {
			pStringCont->x = x, pStringCont->y = y;
			pStringCont->str = str;
		}
		return *this;
	}
	uint32_t code;
	UTF8Decoder decoder;
	const char* p = str;
	for ( ; *p && p != strEnd; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		DrawChar(x, y, fontEntry);
		x += fontEntry.xAdvance * context_.fontScaleX;
	}
	if (pStringCont) {
		pStringCont->x = x, pStringCont->y = y;
		pStringCont->str = p;
	}
	return *this;
}

Drawable& Drawable::DrawStringWrap(int x, int y, int width, int height, const char* str, int htLine, StringCont* pStringCont)
{
	const char* strDone = str;
	if (!context_.pFontSet) {
		if (pStringCont) {
			pStringCont->x = x, pStringCont->y = y;
			pStringCont->str = strDone;
		}
		return *this;
	}
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : width_;
	int yExceed = (height >= 0)? y + height : height_;
	int yAdvance = (htLine >= 0)? htLine : context_.pFontSet->yAdvance * context_.fontScaleY;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int xAdvance = fontEntry.xAdvance * context_.fontScaleX;
		if (x + fontEntry.width * context_.fontScaleX > xExceed) {
			x = xStart, y += yAdvance;
			if (y + yAdvance > yExceed) break;
		}
		DrawChar(x, y, fontEntry);
		x += xAdvance;
		strDone = p + 1;
	}
	if (pStringCont) {
		pStringCont->x = x, pStringCont->y = y;
		pStringCont->str = strDone;
	}
	return *this;
}

}
