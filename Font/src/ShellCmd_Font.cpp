//==============================================================================
// ShellCmd_Basic.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Shell.h"
#include "jxglib/Font.h"

namespace jxglib::ShellCmd_Font {

//-----------------------------------------------------------------------------
// font
//-----------------------------------------------------------------------------
ShellCmd(font, "prints information about installed font")
{
    
    return Result::Success;
}

}
