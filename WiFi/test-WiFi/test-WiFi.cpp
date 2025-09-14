#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"

using namespace jxglib;

ShellCmd(scan, "Scan for WiFi networks")
{
    WiFi::Scan(tout);
    return Result::Success;
}

int main()
{
    stdio_init_all();
    jxglib_labo_init(false);
    while (true) {
        jxglib_tick();
    }
}
