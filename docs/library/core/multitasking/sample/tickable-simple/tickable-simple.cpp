#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

TickableEntry(task, 1000)
{
    static int count = 0;
    printf("Tick: %d\n", count++);
}

int main()
{
    ::stdio_init_all();
    Serial::Terminal terminal;
    terminal.Initialize();
    Shell::AttachTerminal(terminal);
    for (;;) {
        Tickable::Tick();
    }
}
