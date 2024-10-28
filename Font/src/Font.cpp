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
		const FontEntry* pFontEntry = pFontEntries_Extra[i];
		if (pFontEntry->code == code) return *pFontEntry;
	}
	return *pFontEntry_Invalid;
}

}
