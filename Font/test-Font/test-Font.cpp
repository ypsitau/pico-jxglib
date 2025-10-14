#include <stdio.h>
#include "pico/stdlib.h"
//#include "jxglib/Font/misaki_gothic-japanese-level2.h"
//#include "jxglib/Font/naga10-japanese-level2.h"
//#include "jxglib/Font/shinonome12-japanese-level2.h"
#include "jxglib/Font/shinonome14-japanese-level2.h"
//#include "jxglib/Font/shinonome16-japanese-level2.h"
//#include "jxglib/Font/shinonome18.h"
//#include "jxglib/Font/sisd24x32.h"
//#include "jxglib/Font/sisd8x16.h"
//#include "jxglib/Font/unifont-japanese-level2.h"

using namespace jxglib;

void CheckFontSet(const FontSet& fontSet)
{
	::printf("%s\n", fontSet.name);
	do {
		bool successFlag = true;
		for (int i = 0; i < fontSet.nFontEntries_Basic; i++) {
			const FontEntry& fontEntry = fontSet.GetFontEntry_Basic(i);
			const FontEntry& fontEntryFound = fontSet.GetFontEntry(fontEntry.code);
			if (&fontEntry != &fontEntryFound) {
				::printf("%04x was not found\n", fontEntry.code);
				successFlag = false;
				break;
			}
		}
		::printf("  Basic Font (%d characters): %s\n", fontSet.nFontEntries_Basic, successFlag? "success" : "**NG**");
	} while (0);
	if (fontSet.nFontEntries_Extra > 0) {
		bool successFlag = true;
		for (int i = 0; i < fontSet.nFontEntries_Extra; i++) {
			const FontEntry& fontEntry = fontSet.GetFontEntry_Extra(i);
			const FontEntry& fontEntryFound = fontSet.GetFontEntry(fontEntry.code);
			if (&fontEntry != &fontEntryFound) {
				::printf("%04x was not found\n", fontEntry.code);
				successFlag = false;
				break;
			}
		}
		::printf("  Extra Font (%d characters): %s\n", fontSet.nFontEntries_Extra, successFlag? "success" : "**NG**");
	}
}

int main()
{
	::stdio_init_all();
	//CheckFontSet(Font::misaki_gothic);
	//CheckFontSet(Font::naga10);
	//CheckFontSet(Font::shinonome12);
	CheckFontSet(Font::shinonome14);
	//CheckFontSet(Font::shinonome16);
	//CheckFontSet(Font::shinonome18);
	//CheckFontSet(Font::sisd24x32);
	//CheckFontSet(Font::sisd8x16);
	//CheckFontSet(Font::unifont);
	for (;;) ::tight_loop_contents();
}
