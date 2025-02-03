#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	BYTE buff[512];
	disk_initialize(0);
	for (int i = 0; i < 16; i++) {
		disk_read(0, buff, i, 1);
		::printf("%d\n", i);
		Dump.Ascii()(buff, 512);
	}
}
