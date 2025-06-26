#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"

using namespace jxglib;

int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
