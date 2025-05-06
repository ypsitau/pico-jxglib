//==============================================================================
// Flash.cpp
//==============================================================================
#include "jxglib/Util.h"
#include "jxglib/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flash::Test
//------------------------------------------------------------------------------
void Flash::Test()
{
	void* buff = nullptr;
	FlashDummy flash;
	flash.Read(0x00000000, buff, 1024);	
	flash.Read(0x00000f00, buff, 1024);	
	flash.Read(0x00001000, buff, 1024);	
	flash.Read(0x00002000, buff, 1024);	
	::printf("\n");
	flash.Write(0x00000000, buff, 1024);
	flash.Read(0x00000000, buff, 1024);	
	flash.Read(0x00000f00, buff, 1024);	
	flash.Read(0x00000f00, buff, 1024 * 5);	
	flash.Read(0x00001000, buff, 1024);	
	flash.Read(0x00002000, buff, 1024);	
	flash.Write(0x00000200, buff, 1024);
	flash.Write(0x00000400, buff, 1024);
	::printf("\n");
	flash.Write(0x00001000, buff, 1024);
	flash.Read(0x00000000, buff, 1024);	
	flash.Read(0x00000f00, buff, 1024);	
	flash.Read(0x00000f00, buff, 1024 * 5);	
	flash.Read(0x00001000, buff, 1024);	
	flash.Read(0x00002000, buff, 1024);	
	::printf("\n");
	flash.Write(0x00000f00, buff, 1024);
	::printf("\n");
	flash.Write(0x00000f00, buff, 1024);
	::printf("\n");
	flash.Write(0x00000e00, buff, 1024 * 8);
	::printf("\n");
	flash.Write(0x00000e00, buff, 1024 * 10);
	::printf("\n");
	flash.Write(0x00000e00, buff, 1024 * 14);
	::printf("\n");
	flash.Write(0x00001f00, buff, 1024);
	::printf("\n");
	flash.Write(0x00000e00, buff, 1024 * 14);
}

/*
Read(Flash:0x00000000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00000000 0x0400 bytes
Read(Flash:0x00000f00, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00000f00 0x0400 bytes
Read(Flash:0x00001000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00001000 0x0400 bytes
Read(Flash:0x00002000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00002000 0x0400 bytes

Write(Flash:0x00000000, 0x0400 bytes)
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000000 0x0400 bytes
Read(Flash:0x00000000, 0x0400 bytes)
    Copy    Read :0x00000000 Cache:0x00000000 0x0400 bytes
Read(Flash:0x00000f00, 0x0400 bytes)
    Copy    Read :0x00000000 Cache:0x00000f00 0x0100 bytes
    Copy    Read :0x00000100 Flash:0x00001000 0x0300 bytes
Read(Flash:0x00000f00, 0x1400 bytes)
    Copy    Read :0x00000000 Cache:0x00000f00 0x0100 bytes
    Copy    Read :0x00000100 Flash:0x00001000 0x1300 bytes
Read(Flash:0x00001000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00001000 0x0400 bytes
Read(Flash:0x00002000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00002000 0x0400 bytes
Write(Flash:0x00000200, 0x0400 bytes)
    Copy    Cache:0x00000200 Write:0x00000000 0x0400 bytes
Write(Flash:0x00000400, 0x0400 bytes)
    Copy    Cache:0x00000400 Write:0x00000000 0x0400 bytes

Write(Flash:0x00001000, 0x0400 bytes)
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00001000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000000 0x0400 bytes
Read(Flash:0x00000000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00000000 0x0400 bytes
Read(Flash:0x00000f00, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00000f00 0x0100 bytes
    Copy    Read :0x00000100 Cache:0x00000000 0x0300 bytes
Read(Flash:0x00000f00, 0x1400 bytes)
    Copy    Read :0x00000000 Flash:0x00000f00 0x0100 bytes
    Copy    Read :0x00000100 Cache:0x00000000 0x1000 bytes
    Copy    Read :0x00001100 Flash:0x00002000 0x0300 bytes
Read(Flash:0x00001000, 0x0400 bytes)
    Copy    Read :0x00000000 Cache:0x00000000 0x0400 bytes
Read(Flash:0x00002000, 0x0400 bytes)
    Copy    Read :0x00000000 Flash:0x00002000 0x0400 bytes

Write(Flash:0x00000f00, 0x0400 bytes)
    Copy    Cache:0x00000000 Write:0x00000100 0x0300 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000f00 Write:0x00000000 0x0100 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00001000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000100 0x0300 bytes

Write(Flash:0x00000f00, 0x0400 bytes)
    Copy    Cache:0x00000000 Write:0x00000100 0x0300 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000f00 Write:0x00000000 0x0100 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00001000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000100 0x0300 bytes

Write(Flash:0x00000e00, 0x2000 bytes)
    Copy    Cache:0x00000000 Write:0x00000200 0x1000 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000e00 Write:0x00000000 0x0200 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000200 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00002000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00001200 0x0e00 bytes

Write(Flash:0x00000e00, 0x2800 bytes)
    Copy    Cache:0x00000000 Write:0x00001200 0x1000 bytes
    Program Flash:0x00002000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000e00 Write:0x00000000 0x0200 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000200 0x1000 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00001200 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00003000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00002200 0x0600 bytes

Write(Flash:0x00000e00, 0x3800 bytes)
    Copy    Cache:0x00000000 Write:0x00002200 0x1000 bytes
    Program Flash:0x00003000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000e00 Write:0x00000000 0x0200 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000200 0x1000 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00001200 0x1000 bytes
    Program Flash:0x00002000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00002200 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00004000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00003200 0x0600 bytes

Write(Flash:0x00001f00, 0x0400 bytes)
    Program Flash:0x00004000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00001000 0x1000 bytes
    Copy    Cache:0x00000f00 Write:0x00000000 0x0100 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00002000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000100 0x0300 bytes

Write(Flash:0x00000e00, 0x3800 bytes)
    Copy    Cache:0x00000000 Write:0x00001200 0x1000 bytes
    Program Flash:0x00002000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00000000 0x1000 bytes
    Copy    Cache:0x00000e00 Write:0x00000000 0x0200 bytes
    Program Flash:0x00000000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00000200 0x1000 bytes
    Program Flash:0x00001000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00001200 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00002200 0x1000 bytes
    Program Flash:0x00003000 Cache:0x00000000 0x1000 bytes
    Copy    Cache:0x00000000 Flash:0x00004000 0x1000 bytes
    Copy    Cache:0x00000000 Write:0x00003200 0x0600 bytes
*/

}
