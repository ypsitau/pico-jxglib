#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

#define ArrayNumberOf(x) (sizeof(x) / sizeof(x[0]))

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
	for (int i = 0; i < ArrayNumberOf(entryTbl); i++) {
		const Entry& entry = entryTbl[i];
		const Color_T& color = *entry.pColor;
		::printf("%-8s %02x, %02x, %02x\n", entry.name, color.GetR(), color.GetG(), color.GetB());
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
}
