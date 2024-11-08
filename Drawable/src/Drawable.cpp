//==============================================================================
// Drawable.cpp
//==============================================================================
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
void Drawable::DrawChar(int x, int y, uint32_t code)
{
	if (!pFontSet_) return;
	const FontEntry& fontEntry = pFontSet_->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
}

void Drawable::DrawString(int x, int y, const char* str, const char* strEnd)
{
	if (!pFontSet_) return;
	uint32_t code;
	UTF8Decoder decoder;
	for (const char* p = str; *p && p != strEnd; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = pFontSet_->GetFontEntry(code);
		DrawChar(x, y, fontEntry);
		x += fontEntry.xAdvance * fontScaleX_;
	}
}

const char* Drawable::DrawStringWrap(int x, int y, int width, int height, const char* str, int htLine)
{
	if (!pFontSet_) return str;
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : width_;
	int yExceed = (height >= 0)? y + height : height_;
	int yAdvance = (htLine >= 0)? htLine : pFontSet_->yAdvance * fontScaleY_;
	const char* pDone = str;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = pFontSet_->GetFontEntry(code);
		int xAdvance = fontEntry.xAdvance * fontScaleX_;
		if (x + fontEntry.width * fontScaleX_ > xExceed) {
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
