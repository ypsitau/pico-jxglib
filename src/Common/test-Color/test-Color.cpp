#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

template<typename Color_T> void PrintRGB()
{
	struct Entry { const char* name; const Color_T* pColor; };
	static const Entry entryTbl[] = {
		{ "zero",		&Color_T::zero },
		{ "black",		&Color_T::black },
		{ "silver",		&Color_T::silver },
		{ "gray",		&Color_T::gray },
		{ "white",		&Color_T::white },
		{ "maroon",		&Color_T::maroon },
		{ "red",		&Color_T::red },
		{ "purple",		&Color_T::purple },
		{ "fuchsia",	&Color_T::fuchsia },
		{ "green",		&Color_T::green },
		{ "lime",		&Color_T::lime },
		{ "olive",		&Color_T::olive },
		{ "yellow",		&Color_T::yellow },
		{ "navy",		&Color_T::navy },
		{ "blue",		&Color_T::blue },
		{ "teal",		&Color_T::teal },
		{ "aqua",		&Color_T::aqua },
	};
	for (int i = 0; i < count_of(entryTbl); i++) {
		const Entry& entry = entryTbl[i];
		const Color_T& color = *entry.pColor;
		::printf("%-8s %02x, %02x, %02x\n", entry.name, color.GetR(), color.GetG(), color.GetB());
	}
}

void PrintHexColor()
{
	static const char* hexStrTbl[] = {
		"#000000",
		"#ff0000",
		"#00ff00",
		"#ffff00",
		"#0000ff",
		"#ff00ff",
		"#00ffff",
		"#ffffff",
		"#010000",
		"#000100",
		"#010100",
		"#000001",
		"#010001",
		"#000101",
		"#010101",
		"#800000",
		"#008000",
		"#808000",
		"#000080",
		"#800080",
		"#008080",
		"#808080",
		"#000",
		"#f00",
		"#0f0",
		"#ff0",
		"#00f",
		"#f0f",
		"#0ff",
		"#fff",
		"#100",
		"#010",
		"#110",
		"#001",
		"#101",
		"#011",
		"#111",
		"#800",
		"#080",
		"#880",
		"#008",
		"#808",
		"#088",
		"#888",
	};
	for (int i = 0; i < count_of(hexStrTbl); i++) {
		const char* hexStr = hexStrTbl[i];
		Color color;
		color.Parse(hexStr);
		::printf("%-8s %02x, %02x, %02x\n", hexStr, color.GetR(), color.GetG(), color.GetB());
	}
}

int main()
{
	::stdio_init_all();
	::printf("\nColor\n");
	PrintRGB<Color>();
	::printf("\nColorA\n");
	PrintRGB<ColorA>();
	::printf("\nColorRGB565\n");
	PrintRGB<ColorRGB565>();
	::printf("\nColorBGR565\n");
	PrintRGB<ColorBGR565>();
	::printf("\nColorRGB555\n");
	PrintRGB<ColorRGB555>();
	::printf("\nColorBGR555\n");
	PrintRGB<ColorBGR555>();
	::printf("\nHexadecimal\n");
	PrintHexColor();
	for (;;) ;
}
