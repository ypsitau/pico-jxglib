//==============================================================================
// Drawable.cpp
//==============================================================================
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
Size Drawable::CalcStringSize(const char* str) const
{
	if (!context_.pFontSet) return Size::Zero;
	Size size(0, 0);
	uint32_t code;
	UTF8::Decoder decoder;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int wdChar = fontEntry.xAdvance * context_.fontScaleWidth;
		if (*(p + 1)) wdChar *= context_.charWidthRatio;
		size.width += wdChar;
	}
	size.height = CalcFontHeight(); 
	return size;
}

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
	if (image.GetFormat().IsBitmap()) {
		GetDispatcher().DrawBitmap(x, y, image.GetPointer(), image.GetWidth(), image.GetHeight(),
			context_.GetColor(), &context_.GetColorBg(), 1, 1, rectClipAdj, drawDir);
	} else {
		GetDispatcher().DrawImage(x, y, image, rectClipAdj, drawDir);
	}
	return *this;
}

Drawable& Drawable::DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler)
{
	GetDispatcher().DrawImageFast(x, y, image, blockFlag, pHandler);
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

Drawable& Drawable::DrawChar(int x, int y, const FontEntry& fontEntry, bool transparentBgFlag, const Context* pContext)
{
	if (!pContext) pContext = &context_;
	if (!(capabilities_ & Capability::TransparentBg)) transparentBgFlag = false;
	DrawBitmap(x, y, fontEntry.data, fontEntry.width, fontEntry.height, transparentBgFlag,
			pContext->fontScaleWidth, pContext->fontScaleHeight, Rect::Empty, DrawDir::Normal, pContext);
	if (!transparentBgFlag && (fontEntry.width < fontEntry.xAdvance)) {
		DrawRectFill(x + fontEntry.width * pContext->fontScaleWidth, y,
			(fontEntry.xAdvance - fontEntry.width) * pContext->fontScaleWidth, fontEntry.height * pContext->fontScaleHeight,
			pContext->colorBg);
	}
	return *this;
}

Drawable& Drawable::DrawChar(int x, int y, uint32_t code, bool transparentBgFlag, const Context* pContext)
{
	if (!pContext) pContext = &context_;
	if (!pContext->pFontSet) return *this;
	if (!(capabilities_ & Capability::TransparentBg)) transparentBgFlag = false;
	const FontEntry& fontEntry = pContext->pFontSet->GetFontEntry(code);
	return DrawChar(x, y, fontEntry, transparentBgFlag, pContext);
}

Drawable& Drawable::DrawString(int x, int y, const char* str, bool transparentBgFlag)
{
	if (!context_.pFontSet) return *this;
	uint32_t code;
	UTF8::Decoder decoder;
	const char* p = str;
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		DrawChar(x, y, fontEntry, transparentBgFlag);
		x += xAdvance;
	}
	return *this;
}

Drawable& Drawable::DrawStringWrap(int x, int y, int width, int height, const char* str, bool transparentBgFlag)
{
	if (!context_.pFontSet) return *this;
	uint32_t code;
	UTF8::Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : width_;
	int yExceed = (height >= 0)? y + height : height_;
	int lineHeight = static_cast<int>(context_.pFontSet->yAdvance * context_.fontScaleHeight * context_.lineHeightRatio);
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		if (code == '\n') {
			x = xStart, y += lineHeight;
			if (y + lineHeight > yExceed) break;
		} else {
			const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
			int xAdvance = context_.CalcAdvanceX(fontEntry);
			if (x + fontEntry.width * context_.fontScaleWidth > xExceed) {
				x = xStart, y += lineHeight;
				if (y + lineHeight > yExceed) break;
			}
			DrawChar(x, y, fontEntry, transparentBgFlag);
			x += xAdvance;
		}
	}
	return *this;
}

Drawable& Drawable::DrawFormatV(int x, int y, const char* format, va_list args)
{
	char str[256];
	::vsnprintf(str, sizeof(str), format, args);
	return DrawString(x, y, str);
}

Drawable& Drawable::DrawFormat(int x, int y, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DrawFormatV(x, y, format, args);
	va_end(args);
	return *this;
}

Drawable& Drawable::DrawFormat(const Point& pt, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	DrawFormatV(pt, format, args);
	va_end(args);
	return *this;
}


Drawable& Drawable::DrawCross(int x, int y, int width, int height, int wdLine, int htLine)
{
	DrawRectFill(x - wdLine / 2, y - height / 2, wdLine, height);
	DrawRectFill(x - width / 2, y - htLine / 2, width, htLine);
	return *this;
}

//------------------------------------------------------------------------------
// Drawable::DispatcherNone
//------------------------------------------------------------------------------
Drawable::DispatcherNone Drawable::DispatcherNone::Instance;

}
