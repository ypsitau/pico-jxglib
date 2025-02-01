//==============================================================================
// Font
//==============================================================================
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontEntry
//------------------------------------------------------------------------------
const FontEntry FontEntry::Invalid8x8 = { 0x0000, 8, 8, 8, {
0xfe,0x82,0x82,0x82,0x82,0x82,0xfe, 0x00,
} };

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
const FontSet FontSet::None = { "none", FontSet::Format::None, 8, &FontEntry::Invalid8x8, {}, 0, {} };

const FontEntry& FontSet::GetFontEntry(uint32_t code) const
{
	if (IsNone() || code < 32) return *pFontEntry_Invalid;
	if (code <= 126) return *pFontEntryTbl_Basic[code - 32];
	if (nFontEntries_Extra > 0) {
		int idxTop = 0, idxBtm = nFontEntries_Extra - 1;
		for (;;) {
			int idx = (idxTop + idxBtm) / 2;
			const FontEntry* pFontEntry = pFontEntryTbl_Extra[idx];
			if (pFontEntry->code == code) return *pFontEntry;
			if (idxTop == idxBtm) break;
			if (code < pFontEntry->code) {
				idxBtm = idx;
			} else { // code > pFontEntry->code
				idxTop = idx;
			}
			if (idxTop + 1 == idxBtm) {
				const FontEntry* pFontEntry = pFontEntryTbl_Extra[idxTop];
				if (pFontEntry->code == code) return *pFontEntry;
				pFontEntry = pFontEntryTbl_Extra[idxBtm];
				if (pFontEntry->code == code) return *pFontEntry;
				break;
			}
		}
	}
	return *pFontEntry_Invalid;
}

}
