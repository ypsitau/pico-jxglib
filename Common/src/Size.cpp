//==============================================================================
// Size.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Size.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Size
//------------------------------------------------------------------------------
const Size Size::Zero { 0, 0 };

bool Size::Parse(const char* str)
{
    if (!str || *str == '\0') return false;
    char* endPtr;
    int widthTmp = static_cast<int>(::strtol(str, &endPtr, 0));
    if (endPtr == str || widthTmp < 0) return false;
    if (*endPtr != 'x' && *endPtr != 'X') return false;
    ++endPtr;
    int heightTmp = static_cast<int>(::strtol(endPtr, &endPtr, 0));
    if (endPtr == str || heightTmp < 0) return false;
    if (*endPtr != '\0') return false;
    width = widthTmp;
    height = heightTmp;
    return true;
}

}
