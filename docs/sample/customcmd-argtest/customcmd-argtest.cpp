#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

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
    LABOPlatform::Instance.Initialize();
    for (;;) {
        Tickable::Tick();
    }
}