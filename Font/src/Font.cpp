//==============================================================================
// Font
//==============================================================================
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
const FontEntry& FontSet::GetFontEntry(uint32_t code) const
{
	if (code < 32) return *pFontEntry_Invalid;
	if (code <= 126) return *pFontEntryTbl_Basic[code - 32];
	for (int i = 0; i < nFontEntries_Extra; i++) {
		const FontEntry* pFontEntry = pFontEntryTbl_Extra[i];
		if (pFontEntry->code == code) return *pFontEntry;
	}
	return *pFontEntry_Invalid;
}

int FontSet::CalcStringWidth(const char* str) const
{
	uint32_t code;
	UTF8Decoder decoder;
	int width = 0;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = GetFontEntry(code);
		width += fontEntry.xAdvance;
	}
	return width;
}

const char* FontSet::SplitString(const char* str, int widthToSplit, int* pWidthSplit) const
{
	uint32_t code;
	UTF8Decoder decoder;
	int width = 0;
	const char* p = str;
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = GetFontEntry(code);
		if (width + fontEntry.xAdvance > widthToSplit) break;
		width += fontEntry.xAdvance;
	}
	if (pWidthSplit) *pWidthSplit = width;
	return p;
}

}
