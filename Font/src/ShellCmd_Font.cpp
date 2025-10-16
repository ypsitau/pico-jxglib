//==============================================================================
// ShellCmd_Basic.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Shell.h"
#include "jxglib/Font.h"

const jxglib::FontSet& ShellCmd_Font_GetFontSet();

namespace jxglib::ShellCmd_Font {

//-----------------------------------------------------------------------------
// font
//-----------------------------------------------------------------------------
ShellCmd(font, "prints information about installed font")
{
	const FontSet& fontSet = ShellCmd_Font_GetFontSet();
	if (fontSet.IsNone()) {
		terr.Printf("no font installed\n");
		return Result::Error;
	}
	tout.Printf("%s y-advance:%d basic-chars:%d extra-chars:%d\n", fontSet.name, fontSet.yAdvance, fontSet.nFontEntries_Basic, fontSet.nFontEntries_Extra);
	return Result::Success;
}

}
