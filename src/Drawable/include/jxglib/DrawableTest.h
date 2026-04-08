//==============================================================================
// jxglib/DrawableTest.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLETEST_H
#define PICO_JXGLIB_DRAWABLETEST_H
#include <stdio.h>
#include "jxglib/Drawable.h"

namespace jxglib::DrawableTest {

void DrawString(Drawable* drawableTbl[], int nDrawables);
inline void DrawString(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; DrawString(drawableTbl, 1); }

void RotateImage(Drawable* drawableTbl[], int nDrawables);
inline void RotateImage(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; RotateImage(drawableTbl, 1); }

void DrawFonts(Drawable* drawableTbl[], int nDrawables);
inline void DrawFonts(Drawable& drawable) { Drawable* drawableTbl[] = {&drawable}; DrawFonts(drawableTbl, 1); }

void DrawLine(Drawable& drawable);

}

#endif
