//==============================================================================
// Flash.cpp
//==============================================================================
#include <memory.h>
#include "jxglib/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------

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
