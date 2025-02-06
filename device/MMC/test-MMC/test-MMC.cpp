#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"

extern "C" DRESULT disk_readp (
    BYTE* buff,        /* Pointer to the destination object */
    DWORD sector,    /* Sector number (LBA) */
    UINT offset,    /* Offset in the sector */
    UINT count        /* Byte count (bit15:destination) */
);

using namespace jxglib;

int main()
{
	::stdio_init_all();
	BYTE buff[512];
	::memset(buff, 0x00, sizeof(buff));
	disk_initialize(0);
	for (int i = 0; i < 2; i++) {
		disk_read(0, buff, i, 1);
		//disk_readp(buff, i, 0, 1);
		::printf("%d\n", i);
		Dump.Ascii()(buff, 512);
	}
}
