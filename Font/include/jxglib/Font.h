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
	uint16_t code;
	uint8_t width;
	uint8_t height;
	uint8_t xAdvance;
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
	static const int nFontEntries_Basic = 0x5f;
	static const FontSet None;
public:
	const char* name;
	Format format;
	uint8_t yAdvance;
	const FontEntry* pFontEntry_Invalid;
	const FontEntry* pFontEntryTbl_Basic[nFontEntries_Basic];
	int nFontEntries_Extra;
	const FontEntry* pFontEntryTbl_Extra[];
public:
	bool IsNone() const { return format == Format::None; }
	bool IsBitmap() const { return format == Format::Bitmap; }
	bool IsGray() const { return format == Format::Gray; }
	bool HasExtraFont() const { return nFontEntries_Extra > 0; }
	const FontEntry& GetFontEntry(uint32_t code) const;
};

}

#endif
