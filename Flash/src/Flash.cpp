//==============================================================================
// Flash.cpp
//==============================================================================
#include "jxglib/Util.h"
#include "jxglib/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------
Flash Flash::Instance;

void Flash::Read_(uint32_t offsetXIP, void* buff, uint32_t bytes)
{
	if (offsetXIPCached_ == UINT32_MAX) {
		CopyMemory(buff, 0, XIP_BASE, offsetXIP, bytes);
	} else if (offsetXIP < offsetXIPCached_) {
		// ----------[buffCache]----------
		// ---rrrrr-----------------------
		// ---rrrrrrrrrrrr----------------
		// ---rrrrrrrrrrrrrrrrrrrrrrr-----
		uint32_t offsetBuff = 0;
		uint32_t bytesRest = bytes;
		uint32_t bytesToCopy = ChooseMin(bytesRest, offsetXIPCached_ - offsetXIP);
		CopyMemory(buff, offsetBuff, XIP_BASE, offsetXIP, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		bytesToCopy = ChooseMin(bytesRest, sizeof(buffCache_));
		CopyMemory(buff, offsetBuff, buffCache_, 0, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		CopyMemory(buff, offsetBuff, XIP_BASE, offsetXIP, bytesRest);
	} else if (offsetXIP < offsetXIPCached_ + sizeof(buffCache_)) {
		// ----------[buffCache]----------
		// -----------rrrrr---------------
		// -----------rrrrrrrrrrrr--------
		uint32_t offsetBuff = 0;
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		uint32_t bytesRest = bytes;
		uint32_t bytesToCopy = ChooseMin(bytesRest, sizeof(buffCache_) - offsetInCache);
		CopyMemory(buff, offsetBuff, buffCache_, offsetInCache, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		CopyMemory(buff, offsetBuff, XIP_BASE, offsetXIP, bytesRest);
	} else {
		// ----------[buffCache]----------
		// -----------------------rrrrr---
		CopyMemory(buff, 0, XIP_BASE, offsetXIP, bytes);
	}
}

void Flash::Write_(uint32_t offsetXIP, const void* buff, uint32_t bytes)
{
	if (offsetXIPCached_ == UINT32_MAX) {
		offsetXIPCached_ = offsetXIP & ~(sizeof(buffCache_) - 1);
		CopyMemory(buffCache_, 0, XIP_BASE, offsetXIPCached_, sizeof(buffCache_));
	}
	if (offsetXIP + bytes < offsetXIPCached_ || offsetXIPCached_ + sizeof(buffCache_) <= offsetXIP) {
		// ----------[buffCache]----------
		// ---wwwwww----------------------
		// ----------------------wwwwww---
		EraseAndProgram(offsetXIPCached_, buffCache_, sizeof(buffCache_));
	} else if (offsetXIP < offsetXIPCached_) {
		// ----------[buffCache]----------
		// ---wwwwwwwwwwww----------------
		// ---wwwwwwwwwwwwwwwwwwwwwwwww---
		uint32_t bytesToCopy = ChooseMin(offsetXIP + bytes - offsetXIPCached_, sizeof(buffCache_));
		CopyMemory(buffCache_, 0, buff, offsetXIPCached_ - offsetXIP, bytesToCopy); 
		EraseAndProgram(offsetXIPCached_, buffCache_, sizeof(buffCache_));
	} else if (offsetXIP + bytes <= offsetXIPCached_ + sizeof(buffCache_)) {
		// ----------[buffCache]----------
		// -----------wwwwww--------------
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		CopyMemory(buffCache_, offsetInCache, buff, 0, bytes); 
		return;	// no need of flash-programming
	} else {
		// ----------[buffCache]----------
		// -----------wwwwwwwwwww---------
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		CopyMemory(buffCache_, offsetInCache, buff, 0, sizeof(buffCache_) - offsetInCache); 
		EraseAndProgram(offsetXIPCached_, buffCache_, sizeof(buffCache_));
	}
	uint32_t offsetXIPToSkip = offsetXIPCached_;
	offsetXIPCached_ = offsetXIP & ~(sizeof(buffCache_) - 1);
	uint32_t offsetBuff = 0;
	uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
	uint32_t bytesRest = bytes;
	for (;;) {
		uint32_t bytesToCopy = bytesRest;
		if (bytesRest < sizeof(buffCache_) - offsetInCache) {
			CopyMemory(buffCache_, 0, XIP_BASE, offsetXIPCached_, sizeof(buffCache_));
		} else {
			bytesToCopy = sizeof(buffCache_) - offsetInCache;
		}
		CopyMemory(buffCache_, offsetInCache, buff, offsetBuff, bytesToCopy);
		if (offsetXIPCached_ != offsetXIPToSkip) {
			EraseAndProgram(offsetXIPCached_, buffCache_, sizeof(buffCache_));
		}
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) break;
		offsetInCache = 0;
		offsetXIPCached_ += sizeof(buffCache_), offsetBuff += bytesToCopy;
	}
}

void Flash::Synchronize_()
{
	EraseAndProgram(offsetXIPCached_, buffCache_, sizeof(buffCache_));
}

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
}

//------------------------------------------------------------------------------
// Flash::Stream
//------------------------------------------------------------------------------
Flash::Stream::Stream(uint32_t offsetXIP) : offsetXIPStart_{offsetXIP}, offsetXIPCached_{offsetXIP}, bytesBuffPage_{0}
{
}

bool Flash::Stream::Read(void* buff, int bytesBuff, int* pBytesRead)
{
	return false;
}

bool Flash::Stream::Write(const void* buff, int bytesBuff)
{
	const uint8_t* buffp = reinterpret_cast<const uint8_t*>(buff);
	uint32_t bytesRest = bytesBuff;
	while (bytesRest > 0) {
		uint32_t bytesToCopy = sizeof(buffPage_) - bytesBuffPage_;
		if (bytesToCopy > bytesRest) bytesToCopy = bytesRest;
		::memcpy(buffPage_ + bytesBuffPage_, buffp, bytesToCopy);
		buffp += bytesToCopy;
		bytesRest -= bytesToCopy;
		bytesBuffPage_ += bytesToCopy;
		if (bytesBuffPage_ == sizeof(buffPage_)) {
			Flash::Instance.Program(offsetXIPCached_, buffPage_, bytesBuffPage_);
			offsetXIPCached_ += bytesBuffPage_;
			bytesBuffPage_ = 0;
		}
	}
	return true;
}

void Flash::Stream::Flush()
{
	if (bytesBuffPage_ > 0) {
		::memset(buffPage_ + bytesBuffPage_, 0xff, sizeof(buffPage_) - bytesBuffPage_);
		Flash::Instance.Program(offsetXIPCached_, buffPage_, bytesBuffPage_);
	}
}

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
	//::printf("  Erase   Flash:0x%08x                  0x%04x bytes\n", offsetXIP, bytes);
}

void FlashDummy::Program(uint32_t offsetXIP, const void* data, uint32_t bytes)
{
	::printf("  Program Flash:0x%08x Cache:0x%08x 0x%04x bytes\n", offsetXIP, 0, bytes);
}

void FlashDummy::CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes)
{
	const char* nameDst = (dst == buffCache_)? "Cache" : (dst == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Read ";
	const char* nameSrc = (src == buffCache_)? "Cache" : (src == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Write";
	::printf("  Copy    %s:0x%08x %s:0x%08x 0x%04x bytes\n", nameDst, offsetDst, nameSrc, offsetSrc, bytes);}
}
