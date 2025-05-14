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
		CopyMemory_(buff, 0, XIP_BASE, offsetXIP, bytes);
	} else if (offsetXIP < offsetXIPCached_) {
		// ----------[buffCache]----------
		// ---rrrrr-----------------------
		// ---rrrrrrrrrrrr----------------
		// ---rrrrrrrrrrrrrrrrrrrrrrr-----
		uint32_t offsetBuff = 0;
		uint32_t bytesRest = bytes;
		uint32_t bytesToCopy = ChooseMin(bytesRest, offsetXIPCached_ - offsetXIP);
		CopyMemory_(buff, offsetBuff, XIP_BASE, offsetXIP, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		bytesToCopy = ChooseMin(bytesRest, bytesCache_);
		CopyMemory_(buff, offsetBuff, buffCache_, 0, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		CopyMemory_(buff, offsetBuff, XIP_BASE, offsetXIP, bytesRest);
	} else if (offsetXIP < offsetXIPCached_ + bytesCache_) {
		// ----------[buffCache]----------
		// -----------rrrrr---------------
		// -----------rrrrrrrrrrrr--------
		uint32_t offsetBuff = 0;
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		uint32_t bytesRest = bytes;
		uint32_t bytesToCopy = ChooseMin(bytesRest, bytesCache_ - offsetInCache);
		CopyMemory_(buff, offsetBuff, buffCache_, offsetInCache, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) return;
		offsetBuff += bytesToCopy, offsetXIP += bytesToCopy;
		CopyMemory_(buff, offsetBuff, XIP_BASE, offsetXIP, bytesRest);
	} else {
		// ----------[buffCache]----------
		// -----------------------rrrrr---
		CopyMemory_(buff, 0, XIP_BASE, offsetXIP, bytes);
	}
}

void Flash::Write_(uint32_t offsetXIP, const void* buff, uint32_t bytes)
{
	if (offsetXIPCached_ == UINT32_MAX) {
		offsetXIPCached_ = offsetXIP & ~(bytesCache_ - 1);
		CopyMemory_(buffCache_, 0, XIP_BASE, offsetXIPCached_, bytesCache_);
	}
	if (offsetXIP + bytes < offsetXIPCached_ || offsetXIPCached_ + bytesCache_ <= offsetXIP) {
		// ----------[buffCache]----------
		// ---wwwwww----------------------
		// ----------------------wwwwww---
		EraseAndProgram_(offsetXIPCached_, buffCache_, bytesCache_);
	} else if (offsetXIP < offsetXIPCached_) {
		// ----------[buffCache]----------
		// ---wwwwwwwwwwww----------------
		// ---wwwwwwwwwwwwwwwwwwwwwwwww---
		uint32_t bytesToCopy = ChooseMin(offsetXIP + bytes - offsetXIPCached_, bytesCache_);
		CopyMemory_(buffCache_, 0, buff, offsetXIPCached_ - offsetXIP, bytesToCopy); 
		EraseAndProgram_(offsetXIPCached_, buffCache_, bytesCache_);
	} else if (offsetXIP + bytes <= offsetXIPCached_ + bytesCache_) {
		// ----------[buffCache]----------
		// -----------wwwwww--------------
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		CopyMemory_(buffCache_, offsetInCache, buff, 0, bytes); 
		return;	// no need of flash-programming
	} else {
		// ----------[buffCache]----------
		// -----------wwwwwwwwwww---------
		uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
		CopyMemory_(buffCache_, offsetInCache, buff, 0, bytesCache_ - offsetInCache); 
		EraseAndProgram_(offsetXIPCached_, buffCache_, bytesCache_);
	}
	uint32_t offsetXIPToSkip = offsetXIPCached_;
	offsetXIPCached_ = offsetXIP & ~(bytesCache_ - 1);
	uint32_t offsetBuff = 0;
	uint32_t offsetInCache = offsetXIP - offsetXIPCached_;
	uint32_t bytesRest = bytes;
	for (;;) {
		if (offsetInCache > 0 || bytesRest < bytesCache_) {
			CopyMemory_(buffCache_, 0, XIP_BASE, offsetXIPCached_, bytesCache_);
		}
		uint32_t bytesToCopy = ChooseMin(bytesRest, bytesCache_ - offsetInCache);
		CopyMemory_(buffCache_, offsetInCache, buff, offsetBuff, bytesToCopy);
		bytesRest -= bytesToCopy;
		if (bytesRest == 0) break;
		if (offsetXIPCached_ != offsetXIPToSkip) {
			EraseAndProgram_(offsetXIPCached_, buffCache_, bytesCache_);
		}
		offsetInCache = 0;
		offsetXIPCached_ += bytesCache_, offsetBuff += bytesToCopy;
	}
}

void Flash::Sync_()
{
	EraseAndProgram_(offsetXIPCached_, buffCache_, bytesCache_);
}

}
