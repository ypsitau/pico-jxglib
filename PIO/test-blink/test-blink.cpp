#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	for (;;) ::tight_loop_contents();
}
