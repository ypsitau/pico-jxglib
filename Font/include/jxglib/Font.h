//==============================================================================
// jxglib/Font.h
//==============================================================================
#ifndef PICO_JXGLIB_FONT_H
#define PICO_JXGLIB_FONT_H
#include "pico/stdlib.h"
#include "jxglib/Size.h"
#include "jxglib/UTF8Decoder.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontEntry
//------------------------------------------------------------------------------
struct FontEntry {
	uint32_t code;
	int width;
	int height;
	int xAdvance;
	uint8_t data[];
public:
	static const FontEntry Invalid8x8;
};

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
struct FontSet {
public:
	enum class Format { None, Bitmap, Gray };
public:
	Format format;
	int yAdvance;
	const FontEntry* pFontEntry_Invalid;
	const FontEntry* pFontEntryTbl_Basic[96];
	int nFontEntries_Extra;
	const FontEntry* pFontEntryTbl_Extra[];
public:
	static const FontSet None;
public:
	bool IsNone() const { return format == Format::None; }
	bool IsBitmap() const { return format == Format::Bitmap; }
	bool IsGray() const { return format == Format::Gray; }
	bool HasExtraFont() const { return nFontEntries_Extra > 0; }
	const FontEntry& GetFontEntry(uint32_t code) const;
	int CalcStringWidth(const char* str) const;
	const char* SplitString(const char* str, int widthToSplit, int* pWidthSplit = nullptr) const;
};

}

#endif
