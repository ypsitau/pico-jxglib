//==============================================================================
// Flash.cpp
//==============================================================================
#include <memory.h>
#include "jxglib/Util.h"
#include "jxglib/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------
Flash Flash::Instance;

void Flash::Read_(uint32_t offset, void* buff, size_t bytes)
{
	uint8_t* buffp = reinterpret_cast<uint8_t*>(buff);
	if (offset < offset_) {
		// ---------########----------
		// ---rrrrr-------------------
		// ---rrrrrr====--------------
		// ---rrrrrr===========-------
		size_t bytesToCopy = ChooseMin(static_cast<size_t>(offset_ - offset), bytes);
		::memcpy(buffp, GetPointer(offset), bytesToCopy);
		buffp += bytesToCopy;
		offset += bytesToCopy;
		bytes -= bytesToCopy;
	}
	if (bytes == 0) return;
	if (offset < offset_ + sizeof(buffSector_)) {
		// ---------########----------
		// ----------rrrr-------------
		// ----------rrrrrrr===-------
		size_t bytesToCopy = ChooseMin(static_cast<size_t>(offset_ + sizeof(buffSector_) - offset), bytes);
		::memcpy(buffp, buffSector_ + offset - offset_, bytesToCopy);
		buffp += bytesToCopy;
		offset += bytesToCopy;
		bytes -= bytesToCopy;
	}
	if (bytes > 0) {
		// ---------########----------
		// ------------------rrrrrr---
		::memcpy(buffp, GetPointer(offset), bytes);
	}
}

void Flash::Write_(uint32_t offset, const void* buff, size_t bytes)
{
	const uint8_t* buffp = reinterpret_cast<const uint8_t*>(buff);
	if (offset < offset_ + sizeof(buffSector_)) {
		// ---------########----------
		// -------==wwwwww------------
		// ----------wwww-------------
		// ----------wwwwwww===-------
		uint32_t offsetHit = ChooseMax(offset, offset_);
		size_t bytesToCopy = ChooseMin(static_cast<size_t>(offset_ + sizeof(buffSector_) - offsetHit), bytes);
		::memcpy(buffSector_ + offsetHit - offset_, buffp + offsetHit - offset_, bytesToCopy);
	}
	size_t offsetOrg = offset_;
	if (offset < offset_) {
		// ---------########----------
		// ---wwwww-------------------
		// ---wwwwww====--------------
		// ---wwwwww===========-------
		const uint8_t* buffpWk = buffp;
		size_t offsetWk = offset;
		size_t bytesWk = bytes;
		while (bytes > 0 && offset < offset_) {
			EraseSafe(offset_, sizeof(buffSector_));
			ProgramSafe(offset_, buffSector_, sizeof(buffSector_));
			offset_ = offsetWk & ~(sizeof(buffSector_) - 1);
			::memcpy(buffSector_, GetPointer(offset_), sizeof(buffSector_));
			size_t bytesToCopy = ChooseMin(static_cast<size_t>(offset_ + sizeof(buffSector_) - offsetWk), bytesWk);
			::memcpy(buffSector_ + offsetWk - offset_, buffpWk, bytesToCopy);
			buffpWk += bytesToCopy;
			offsetWk += bytesToCopy;
			bytesWk -= bytesToCopy;
		}
	}
	if (offset + bytes > offsetOrg + sizeof(buffSector_)) {
		// ---------########----------
		// -------==========wwwwwww---
		// --------------===wwwwwww---
		// ------------------wwwwww---
		const uint8_t* buffpWk = buffp;
		size_t offsetWk = offset;
		size_t bytesWk = bytes;
		if (offset < offsetOrg + sizeof(buffSector_)) {
			size_t bytesSkip = offsetOrg + sizeof(buffSector_) - offset;
			buffpWk += bytesSkip;
			offsetWk += bytesSkip;
			bytesWk -= bytesSkip;
		}
		while (bytesWk > 0) {
			EraseSafe(offset_, sizeof(buffSector_));
			ProgramSafe(offset_, buffSector_, sizeof(buffSector_));
			offset_ = offset & ~(sizeof(buffSector_) - 1);
			::memcpy(buffSector_, GetPointer(offset_), sizeof(buffSector_));
			size_t bytesToCopy = ChooseMin(static_cast<size_t>(offset_ + sizeof(buffSector_) - offsetWk), bytesWk);
			::memcpy(buffSector_ + offsetWk - offset_, buffp, bytesToCopy);
			buffpWk += bytesToCopy;
			offsetWk += bytesToCopy;
			bytesWk -= bytesToCopy;
		}
	}
}

//------------------------------------------------------------------------------
// Flash::Stream
//------------------------------------------------------------------------------
Flash::Stream::Stream(uint32_t offset) : offsetStart_{offset}, offset_{offset}, bytesBuffPage_{0}
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
			ProgramSafe(offset_, buffPage_, bytesBuffPage_);
			offset_ += bytesBuffPage_;
			bytesBuffPage_ = 0;
		}
	}
	return true;
}

void Flash::Stream::Flush()
{
	if (bytesBuffPage_ > 0) {
		::memset(buffPage_ + bytesBuffPage_, 0xff, sizeof(buffPage_) - bytesBuffPage_);
		ProgramSafe(offset_, buffPage_, bytesBuffPage_);
	}
}

}
