//==============================================================================
// Drawable.cpp
//==============================================================================
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
void Drawable::DrawLine(int x0, int y0, int x1, int y1)
{
	if (x0 == x1) {
		DrawVLine(x0, y0, y1 - y0);
		return;
	} else if (y0 == y1) {
		DrawHLine(x0, y0, x1 - x0);
		return;
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
}

void Drawable::DrawRect(int x, int y, int width, int height)
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
}

void Drawable::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	DrawBitmap(x, y, fontEntry.data, fontEntry.width, fontEntry.height, context_.fontScaleX, context_.fontScaleY);
}

void Drawable::DrawChar(int x, int y, uint32_t code)
{
	if (!context_.pFontSet) return;
	const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
}

void Drawable::DrawString(int x, int y, const char* str, const char* strEnd)
{
	if (!context_.pFontSet) return;
	uint32_t code;
	UTF8Decoder decoder;
	for (const char* p = str; *p && p != strEnd; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		DrawChar(x, y, fontEntry);
		x += fontEntry.xAdvance * context_.fontScaleX;
	}
}

const char* Drawable::DrawStringWrap(int x, int y, int width, int height, const char* str, int htLine)
{
	if (!context_.pFontSet) return str;
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : width_;
	int yExceed = (height >= 0)? y + height : height_;
	int yAdvance = (htLine >= 0)? htLine : context_.pFontSet->yAdvance * context_.fontScaleY;
	const char* pDone = str;
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
		pDone = p + 1;
	}
	return pDone;
}

}
