#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Stream& stream = Stdio::Instance;
	for (;;) {
		char buff[256];
		for (int i = 0; i < sizeof(buff); ) {
			i += stream.Read(buff + i, sizeof(buff) - i);
		}
		Dump(buff, sizeof(buff));
	}
}
