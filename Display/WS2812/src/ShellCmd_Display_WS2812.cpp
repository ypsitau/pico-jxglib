#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Display/WS2812.h"

namespace jxglib::ShellCmd_Display_WS2812 {

ShellCmd_Named(display_ws2812, "display-ws2812", "initialize WS2812 display")
{
    return Result::Success;
}

}

