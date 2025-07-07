//==============================================================================
// Stdio.cpp
//==============================================================================
#include "jxglib/Stdio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------
Stdio Stdio::Instance;

int Stdio::ReadChar()
{
	return ::getchar_timeout_us(0);
}

int Stdio::Read(void* buff, int bytesBuff)
{
	if (bytesBuff <= 0) return 0;
	uint8_t* pBuff = static_cast<uint8_t*>(buff);
	int bytesRead = 0;
	while (bytesRead < bytesBuff && bytesAvailable_ > 0) {
		pBuff[bytesRead++] = buffRead_[bytesRead_++];
		bytesAvailable_--;
	}
	bool contFlag = true;
	while (bytesRead < bytesBuff && contFlag) {
		// Reset buffer position
		bytesRead_ = 0;
		bytesAvailable_ = 0;
		// Fill internal buffer with available data
		while (bytesAvailable_ < sizeof(buffRead_)) {
			int ch = ::getchar_timeout_us(0);
			if (ch < 0) {
				contFlag = false; // no more data or error
				break;
			}
			buffRead_[bytesAvailable_++] = static_cast<uint8_t>(ch);
		}
		// Copy from internal buffer to output buffer
		while (bytesRead < bytesBuff && bytesAvailable_ > 0) {
			pBuff[bytesRead++] = buffRead_[bytesRead_++];
			bytesAvailable_--;
		}
	}
	return bytesRead;
}

int Stdio::Write(const void* buff, int bytesBuff)
{
	if (bytesBuff <= 0) return 0;
	const uint8_t* pBuff = static_cast<const uint8_t*>(buff);
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		int ch = ::putchar_raw(pBuff[bytesWritten++]);
		if (ch < 0) break; // error
	}
	return bytesWritten;
}

void Stdio::ClearReadBuffer()
{
	bytesRead_ = 0;
	bytesAvailable_ = 0;
}

}
