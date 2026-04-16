#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

ShellCmd(argtest, "tests command line arguments")
{
    for (int i = 0; i < argc; i++) {
        tout.Printf("argv[%d] \"%s\"\n", i, argv[i]);
    }
    return Result::Success;
}

int main(void)
{
    ::stdio_init_all();
	Serial::Terminal terminal;
	terminal.Initialize();
	Shell::AttachTerminal(terminal);
    for (;;) {
        Tickable::Tick();
    }
}