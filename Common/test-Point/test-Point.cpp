#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Point.h"

using namespace jxglib;

void test_Point()
{
}

int main()
{
	stdio_init_all();
	test_Point();
	for (;;) ::tight_loop_contents();
}
