#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

LABOPlatform laboPlatform;

int main()
{
	::stdio_init_all();
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
