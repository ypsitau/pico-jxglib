#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"

using namespace jxglib;

WiFi wifi;

WiFi& ShellCmd_WiFi_GetWiFi() { return wifi; }

int main()
{
    stdio_init_all();
    jxglib_labo_init(false);
    while (true) {
        jxglib_tick();
    }
}
