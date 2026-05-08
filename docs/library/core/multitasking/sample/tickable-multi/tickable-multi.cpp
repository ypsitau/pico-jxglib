#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

TickableEntry(task1, 1000)
{
    static int count = 0;
    printf("task1: %d\n", count++);
}

TickableEntry(task2, 2000)
{
    static int count = 0;
    printf("task2: %d\n", count++);
}

TickableEntry(task3, 3000)
{
    static int count = 0;
    printf("task3: %d\n", count++);
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
