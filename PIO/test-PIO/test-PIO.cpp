#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "test-PIO.h"

using namespace jxglib;

int main()
{
    PIOBox pioBox();
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
