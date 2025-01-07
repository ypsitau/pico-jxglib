//==============================================================================
// jxglib/Drawable_TestCase.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_TESTCASE_H
#define PICO_JXGLIB_DRAWABLE_TESTCASE_H
#include <stdio.h>
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable_TestCase
//------------------------------------------------------------------------------
class Drawable_TestCase {
public:
	static void DrawString(Drawable* drawableTbl[], int nDrawables);
	static void DrawString(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; DrawString(drawableTbl, 1); }
	static void RotateImage(Drawable* drawableTbl[], int nDrawables);
	static void RotateImage(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; RotateImage(drawableTbl, 1); }
	static void DrawFonts(Drawable* drawableTbl[], int nDrawables);
	static void DrawFonts(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; DrawFonts(drawableTbl, 1); }
	static void DrawLine(Drawable& drawable);
};

}

#endif
