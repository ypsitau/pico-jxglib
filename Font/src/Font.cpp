//==============================================================================
// Font
//==============================================================================
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FontEntry
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FontSet
//------------------------------------------------------------------------------
const FontSet FontSet::None = { "none", FontSet::Format::None, 8, 0, 0, {} };

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

}
