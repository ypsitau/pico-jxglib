//==============================================================================
// Font
//==============================================================================
#include <memory.h>
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontEntry
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
const FontSet FontSet::None = { "none", FontSet::Format::None, 8, 0, 0, {} };
uint32_t FontSet::bytesProgramMax = PICO_FLASH_SIZE_BYTES;

const FontEntry& FontSet::GetFontEntry(uint32_t code) const
{
	if (IsNone() || code < 32) return GetFontEntry_Invalid();
	if (code <= 126) return GetFontEntry_Basic(code - 32);
	if (nFontEntries_Extra > 0) {
		int idxTop = 0, idxBtm = nFontEntries_Extra - 1;
		for (;;) {
			int idx = (idxTop + idxBtm) / 2;
			const FontEntry& fontEntry = GetFontEntry_Extra(idx);
			if (fontEntry.code == code) return fontEntry;
			if (idxTop == idxBtm) break;
			if (code < fontEntry.code) {
				idxBtm = idx;
			} else { // code > fontEntry.code
				idxTop = idx;
			}
			if (idxTop + 1 == idxBtm) {
				const FontEntry& fontEntryTop = GetFontEntry_Extra(idxTop);
				if (fontEntryTop.code == code) return fontEntryTop;
				const FontEntry& fontEntryBtm = GetFontEntry_Extra(idxBtm);
				if (fontEntryBtm.code == code) return fontEntryBtm;
				break;
			}
		}
	}
	return GetFontEntry_Invalid();
}

uint32_t FontSet::GetInstanceAddrTop(int iFont)
{
	uint32_t addrTop = XIP_BASE + bytesProgramMax;
	for (int i = 0; ; ++i) {
		const char* footer = reinterpret_cast<const char*>(addrTop - (8 + 4));
		if (::memcmp(footer, "LABOFONT", 8) != 0) return 0;
		uint32_t bytes = *reinterpret_cast<const uint32_t*>(footer + 8);
		if (bytes > bytesProgramMax) return 0;
		addrTop -= bytes;
		const char* header = reinterpret_cast<const char*>(addrTop);
		if (::memcmp(header, "LABOFONT", 8) != 0) return 0;
		if (i == iFont) break;
	}
	return addrTop;
}

const FontSet& FontSet::GetInstance(int iFont)
{
	uint32_t addrTop = GetInstanceAddrTop(iFont);
	return (addrTop == 0)? FontSet::None : *reinterpret_cast<const FontSet*>(addrTop + 8);
}

}
