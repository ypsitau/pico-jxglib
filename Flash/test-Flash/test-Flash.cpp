#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Flash.h"

using namespace jxglib;

class FlashDummy : public Flash {
public:
	void Read(uint32_t offsetXIP, void* buff, uint32_t bytes);
	void Write(uint32_t offsetXIP, const void* buff, uint32_t bytes);
public:
	virtual void Erase(uint32_t offsetXIP, uint32_t bytes) override;
	virtual void Program(uint32_t offsetXIP, const void* data, uint32_t bytes) override;
	virtual void CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes) override;
};

int main()
{
	::stdio_init_all();
	::printf("--------------------------------------------------------------------------------\n");
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
	for (;;) ::tight_loop_contents();
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

//------------------------------------------------------------------------------
// FlashDummy
//------------------------------------------------------------------------------
void FlashDummy::Read(uint32_t offsetXIP, void* buff, uint32_t bytes)
{
	::printf("Read(Flash:0x%08x, 0x%04x bytes)\n", offsetXIP, bytes);
	Read_(offsetXIP, buff, bytes);
}

void FlashDummy::Write(uint32_t offsetXIP, const void* buff, uint32_t bytes)
{
	::printf("Write(Flash:0x%08x, 0x%04x bytes)\n", offsetXIP, bytes);
	Write_(offsetXIP, buff, bytes);
}

void FlashDummy::Erase(uint32_t offsetXIP, uint32_t bytes)
{
	//::printf("    Erase   Flash:0x%08x                  0x%04x bytes\n", offsetXIP, bytes);
}

void FlashDummy::Program(uint32_t offsetXIP, const void* data, uint32_t bytes)
{
	::printf("    Program Flash:0x%08x Cache:0x%08x 0x%04x bytes\n", offsetXIP, 0, bytes);
}

void FlashDummy::CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes)
{
	const char* nameDst = (dst == buffCache_)? "Cache" : (dst == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Read ";
	const char* nameSrc = (src == buffCache_)? "Cache" : (src == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Write";
	::printf("    Copy    %s:0x%08x %s:0x%08x 0x%04x bytes\n", nameDst, offsetDst, nameSrc, offsetSrc, bytes);
}
