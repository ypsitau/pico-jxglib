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
    static void RotateImage(Drawable* drawableTbl[], int nDrawables);
};

}

#endif
