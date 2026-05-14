#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    PIO::Program program;
    //-------------------------------------------------------------------------
    program
    .program("blink")
        .pull().block()
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
    //-------------------------------------------------------------------------
    PIO::StateMachine sm;
    sm.set_program(program)
        .reserve_set_pin(GPIO14)
        .init()
        .put((::clock_get_hz(clk_sys) / (1 * 2)) - 3);
    sm.set_enabled();
    //-------------------------------------------------------------------------
    for (;;) Tickable::Tick();
}
