#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
    ::stdio_init_all();
    LABOPlatform::Instance.Initialize();
    for (;;) {
        // Your code here
        Tickable::Tick();
    }
}
