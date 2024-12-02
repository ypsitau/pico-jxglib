#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TEK4010.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();

	TEK4010 tek(UART::Default);
	tek.Test();
}
