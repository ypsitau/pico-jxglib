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
uint32_t FontSet::flashAddrBtm = XIP_BASE + PICO_FLASH_SIZE_BYTES;

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

bool FontSet::GetRange(uint32_t* pFlashAddr, uint32_t* pFlashBytes)
{
	uint32_t flashAddr = 0, flashBytes = 0;
	for (int iFont = 0; ; ++iFont) {
		if (!GetInstanceRange(iFont, &flashAddr, &flashBytes)) break;
	}
	if (flashAddr == 0) return false;
	*pFlashAddr = flashAddr;
	*pFlashBytes = flashAddrBtm - flashAddr;
	return true;
}

bool FontSet::GetInstanceRange(int iFont, uint32_t* pFlashAddr, uint32_t* pFlashBytes)
{
	uint32_t flashAddr = flashAddrBtm;
	uint32_t flashBytes = 0;
	for (int iFontIter = 0; ; ++iFontIter) {
		const char* footer = reinterpret_cast<const char*>(flashAddr - (4 + 8));
		if (::memcmp(footer + 4, "LABOFONT", 8) != 0) return false;
		flashBytes = *reinterpret_cast<const uint32_t*>(footer);
		if (flashBytes > PICO_FLASH_SIZE_BYTES || flashAddr - flashBytes < XIP_BASE) return false;
		flashAddr -= flashBytes;
		const char* header = reinterpret_cast<const char*>(flashAddr);
		if (::memcmp(header, "LABOFONT", 8) != 0) return false;
		if (iFontIter == iFont) break;
	}
	*pFlashAddr = flashAddr;
	*pFlashBytes = flashBytes;
	return true;
}

const FontSet& FontSet::GetInstance(int iFont, uint32_t* pFlashAddr, uint32_t* pFlashBytes)
{
	uint32_t flashAddr, flashBytes;
	if (!GetInstanceRange(iFont, &flashAddr, &flashBytes)) return FontSet::None;
	if (pFlashAddr) *pFlashAddr = flashAddr;
	if (pFlashBytes) *pFlashBytes = flashBytes;
	return *reinterpret_cast<const FontSet*>(flashAddr + 8);
}

const FontSet& FontSet::GetInstance(const char* name, uint32_t* pFlashAddr, uint32_t* pFlashBytes)
{
	for (int iFont = 0; ; ++iFont) {
		uint32_t flashAddr, flashBytes;
		const FontSet& fontSet = GetInstance(iFont, &flashAddr, &flashBytes);
		if (fontSet.IsNone()) break;
		if (::strcasecmp(fontSet.name, name) == 0) {
			if (pFlashAddr) *pFlashAddr = flashAddr;
			if (pFlashBytes) *pFlashBytes = flashBytes;
			return fontSet;
		}
	}
	return FontSet::None;
}

}
