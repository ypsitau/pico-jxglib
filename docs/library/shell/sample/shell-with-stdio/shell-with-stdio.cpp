#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Serial::Terminal is a Terminal class that can be attached to a serial interface.
    // By default, it uses stdio as the serial interface.
    Serial::Terminal terminal;
    terminal.Initialize();
    // Attach the terminal.
    Shell::AttachTerminal(terminal);
    for (;;) {
        Tickable::Tick();
    }
}
