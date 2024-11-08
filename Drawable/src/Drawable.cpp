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
	int xExceed = (width >= 0)? x + width : GetScreenWidth();
	int yExceed = (height >= 0)? y + height : GetScreenHeight();
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
