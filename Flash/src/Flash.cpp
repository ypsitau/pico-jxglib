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

void Flash::Read_(uint32_t offset, void* buff, uint32_t bytes)
{
	if (offsetCached_ == -1) {
		CopyMemory(buff, 0, XIP_BASE, offset, bytes);
		return;
	}
	uint32_t bytesRest = bytes;
	uint32_t offsetSrc = offset;
	uint32_t offsetDst = 0;
	if (offset < offsetCached_) {
		// ---------########----------
		// ---rrrrr-------------------
		// ---rrrrrr====--------------
		// ---rrrrrr===========-------
		uint32_t bytesToCopy = ChooseMin(static_cast<uint32_t>(offsetCached_ - offsetSrc), bytesRest);
		CopyMemory(buff, offsetDst, XIP_BASE, offsetSrc, bytesToCopy);
		offsetSrc += bytesToCopy;
		offsetDst += bytesToCopy;
		bytesRest -= bytesToCopy;
	}
	if (bytesRest == 0) return;
	if (offsetSrc < offsetCached_ + sizeof(buffCache_)) {
		// ---------########----------
		// ----------rrrr-------------
		// ----------rrrrrrr===-------
		uint32_t bytesToCopy = ChooseMin(static_cast<uint32_t>(offsetCached_ + sizeof(buffCache_) - offsetSrc), bytesRest);
		CopyMemory(buff, offsetDst, buffCache_, offsetSrc - offsetCached_, bytesToCopy);
		offsetSrc += bytesToCopy;
		offsetDst += bytesToCopy;
		bytesRest -= bytesToCopy;
	}
	if (bytesRest > 0) {
		// ---------########----------
		// ------------------rrrrrr---
		CopyMemory(buff, offsetDst, XIP_BASE, offsetSrc, bytesRest);
	}
}

void Flash::Write_(uint32_t offset, const void* buff, uint32_t bytes)
{
	if (offsetCached_ == -1) {
		uint32_t offsetSrc = 0;
		uint32_t offsetDst = offset;
		offsetCached_ = offsetDst & ~(sizeof(buffCache_) - 1);
		CopyMemory(buffCache_, 0, XIP_BASE, offsetCached_, sizeof(buffCache_));
	}
	uint32_t offsetCachedOrg = offsetCached_;
	if (offset < offsetCached_ + sizeof(buffCache_)) {
		// ---------########----------
		// -------==wwwwww------------
		// ----------wwww-------------
		// ----------wwwwwww===-------
		uint32_t offsetHit = ChooseMax(offset, offsetCached_);
		uint32_t offsetBuff = offsetHit - offset;
		uint32_t bytesToCopy = ChooseMin(static_cast<uint32_t>(offsetCached_ + sizeof(buffCache_) - offsetHit), bytes - offsetBuff);
		CopyMemory(buffCache_, offsetHit - offsetCached_, buff, offsetBuff, bytesToCopy);
	}
	if (offset < offsetCached_) {
		// ---------########----------
		// ---wwwww-------------------
		// ---wwwwww====--------------
		// ---wwwwww===========-------
		uint32_t offsetSrc = 0;
		uint32_t offsetDst = offset;
		uint32_t bytesRest = bytes;
		while (bytesRest > 0 && offset < offsetCached_) {
			Erase(offsetCached_, sizeof(buffCache_));
			Program(offsetCached_, buffCache_, sizeof(buffCache_));
			offsetCached_ = offsetDst & ~(sizeof(buffCache_) - 1);
			CopyMemory(buffCache_, 0, XIP_BASE, offsetCached_, sizeof(buffCache_));
			uint32_t bytesToCopy = ChooseMin(static_cast<uint32_t>(offsetCached_ + sizeof(buffCache_) - offsetDst), bytesRest);
			CopyMemory(buffCache_, offsetDst - offsetCached_, buff, offsetSrc, bytesToCopy);
			offsetSrc += bytesToCopy;
			offsetDst += bytesToCopy;
			bytesRest -= bytesToCopy;
		}
	}
	// ************************************************
	if (offset + bytes > offsetCachedOrg + sizeof(buffCache_)) {
		// ---------########----------
		// -------==========wwwwwww---
		// --------------===wwwwwww---
		// ------------------wwwwww---
		uint32_t offsetSrc = 0;
		uint32_t offsetDst = offset;
		uint32_t bytesRest = bytes;
		if (offset < offsetCachedOrg + sizeof(buffCache_)) {
			uint32_t bytesSkip = offsetCachedOrg + sizeof(buffCache_) - offset;
			offsetSrc += bytesSkip;
			offsetDst += bytesSkip;
			bytesRest -= bytesSkip;
		}
		while (bytesRest > 0) {
			Erase(offsetCached_, sizeof(buffCache_));
			Program(offsetCached_, buffCache_, sizeof(buffCache_));
			offsetCached_ = offset & ~(sizeof(buffCache_) - 1);
			CopyMemory(buffCache_, 0, XIP_BASE, offsetCached_, sizeof(buffCache_));
			uint32_t bytesToCopy = ChooseMin(static_cast<uint32_t>(offsetCached_ + sizeof(buffCache_) - offsetDst), bytesRest);
			if (bytesToCopy == 0) break;
			CopyMemory(buffCache_, offsetDst - offsetCached_, buff, 0, bytesToCopy);
			offsetSrc += bytesToCopy;
			offsetDst += bytesToCopy;
			bytesRest -= bytesToCopy;
		}
	}
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
Flash::Stream::Stream(uint32_t offset) : offsetStart_{offset}, offsetCached_{offset}, bytesBuffPage_{0}
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
			Flash::Instance.Program(offsetCached_, buffPage_, bytesBuffPage_);
			offsetCached_ += bytesBuffPage_;
			bytesBuffPage_ = 0;
		}
	}
	return true;
}

void Flash::Stream::Flush()
{
	if (bytesBuffPage_ > 0) {
		::memset(buffPage_ + bytesBuffPage_, 0xff, sizeof(buffPage_) - bytesBuffPage_);
		Flash::Instance.Program(offsetCached_, buffPage_, bytesBuffPage_);
	}
}

//------------------------------------------------------------------------------
// FlashDummy
//------------------------------------------------------------------------------
void FlashDummy::Read(uint32_t offset, void* buff, uint32_t bytes)
{
	::printf("Read(Flash:0x%08x, 0x%04x bytes)\n", offset, bytes);
	Read_(offset, buff, bytes);
}

void FlashDummy::Write(uint32_t offset, const void* buff, uint32_t bytes)
{
	::printf("Write(Flash:0x%08x, 0x%04x bytes)\n", offset, bytes);
	Write_(offset, buff, bytes);
}

void FlashDummy::Erase(uint32_t offset, uint32_t bytes)
{
	::printf("  Erase   Flash:0x%08x                  0x%04x bytes\n", offset, bytes);
}

void FlashDummy::Program(uint32_t offset, const void* data, uint32_t bytes)
{
	::printf("  Program Flash:0x%08x Cache:0x%08x 0x%04x bytes\n", offset, 0, bytes);
}

void FlashDummy::CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes)
{
	const char* nameDst = (dst == buffCache_)? "Cache" : (dst == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Read ";
	const char* nameSrc = (src == buffCache_)? "Cache" : (src == reinterpret_cast<const void*>(XIP_BASE))? "Flash" : "Write";
	::printf("  Copy    %s:0x%08x %s:0x%08x 0x%04x bytes\n", nameDst, offsetDst, nameSrc, offsetSrc, bytes);}
}
