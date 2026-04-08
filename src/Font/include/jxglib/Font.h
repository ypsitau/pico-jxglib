//==============================================================================
// jxglib/Font.h
//==============================================================================
#ifndef PICO_JXGLIB_FONT_H
#define PICO_JXGLIB_FONT_H
#include "pico/stdlib.h"
#include "jxglib/Size.h"
#include "jxglib/UTF8.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontEntry
//------------------------------------------------------------------------------
struct __attribute__((packed)) FontEntry {
	uint16_t code;
	uint8_t width;
	uint8_t height;
	uint8_t xAdvance;
	uint8_t padding;
	uint8_t data[];
	static const int BytesHeader = 6;
};

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
struct __attribute__((packed)) FontSet {
public:
	enum class Format : uint8_t { None, Bitmap, Gray };
public:
	static const int nFontEntries_Basic = 0x5f;
	static const FontSet None;
	static uint32_t flashAddrBtm;
public:
	char name[32];
	Format format;
	uint8_t yAdvance;
	uint16_t bytesPerFont;
	uint16_t nFontEntries_Extra;
	char data[];
public:
	bool IsNone() const { return format == Format::None; }
	bool IsBitmap() const { return format == Format::Bitmap; }
	bool IsGray() const { return format == Format::Gray; }
	bool HasExtraFont() const { return nFontEntries_Extra > 0; }
	const FontEntry& GetFontEntry(uint32_t code) const;
public:
	static bool GetRange(uint32_t* pFlashAddr, uint32_t* pFlashBytes);
	static bool GetInstanceRange(int iFont, uint32_t* pFlashAddr, uint32_t* pFlashBytes);
	static const FontSet& GetInstance(int iFont, uint32_t* pFlashAddr = nullptr, uint32_t* pFlashBytes = nullptr);
	static const FontSet& GetInstance(const char* name, uint32_t* pFlashAddr = nullptr, uint32_t* pFlashBytes = nullptr);
public:
	const FontEntry& GetFontEntry_Invalid() const { return *reinterpret_cast<const FontEntry*>(data); }
	const FontEntry& GetFontEntry_Basic(int idx) const {
		return *reinterpret_cast<const FontEntry*>(data + (FontEntry::BytesHeader + bytesPerFont) * (1 + idx));
	}
	const FontEntry& GetFontEntry_Extra(int idx) const {
		return *reinterpret_cast<const FontEntry*>(data + (FontEntry::BytesHeader + bytesPerFont) * (1 + nFontEntries_Basic + idx));
	}
};

}

#endif
