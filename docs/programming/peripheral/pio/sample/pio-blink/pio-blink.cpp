#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    PIO::Program program;
    program
    .program("blink")
        .pull()
        .out("y", 32)
    .wrap_target()
        .mov("x", "y")
        .set("pins", 1)         // Turn LED on
    .L("lp1")
        .jmp("x--", "lp1")      // Delay for (x + 1) cycles, x is a 32 bit number
        .mov("x", "y")
        .set("pins", 0)         // Turn LED off
    .L("lp2")
        .jmp("x--", "lp2")      // Delay for the same number of cycles again
    .wrap()                     // Blink forever!
    .end();
    PIO::StateMachine sm;
    sm.set_program(program);
    sm.config_pin_set(GPIO15);
    sm.init();
    sm.set_enabled();
    float freq = 2.;
    sm.put(static_cast<uint>((static_cast<float>(::clock_get_hz(clk_sys)) / (freq * 2))) - 3);
    for (;;) Tickable::Tick();
}
