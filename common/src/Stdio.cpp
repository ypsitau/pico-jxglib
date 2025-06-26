//==============================================================================
// Stdio.cpp
//==============================================================================
#include "jxglib/Stdio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------
Stdio Stdio::Instance;

int Stdio::Read(void* buff, int bytesBuff)
{
	if (bytesBuff <= 0) return 0;
	
	uint8_t* pBuff = static_cast<uint8_t*>(buff);
	int bytesRead = 0;
	
	// First, copy any available data from internal buffer
	while (bytesRead < bytesBuff && bytesAvailable_ > 0) {
		pBuff[bytesRead++] = buffRead_[bytesRead_++];
		bytesAvailable_--;
	}
	
	// If we still need more data, try to fill the internal buffer
	while (bytesRead < bytesBuff) {
		// Reset buffer position
		bytesRead_ = 0;
		bytesAvailable_ = 0;
		
		// Fill internal buffer with available data
		while (bytesAvailable_ < sizeof(buffRead_)) {
			int ch = getchar_timeout_us(0);
			if (ch < 0) break; // no more data or error
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
		int ch = putchar_raw(pBuff[bytesWritten++]);
		if (ch < 0) break; // error
	}
	return bytesWritten;
}

void Stdio::ClearReadBuffer()
{
	bytesRead_ = 0;
	bytesAvailable_ = 0;
}

//------------------------------------------------------------------------------
// Stdio::Keyboard
//------------------------------------------------------------------------------
Stdio::Keyboard Stdio::Keyboard::Instance;

int Stdio::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax, bool includeModifiers)
{
	if (nKeysMax == 0) return 0;
	int nKeys = 0;
	KeyData keyData;
	if (GetKeyDataNB(&keyData) && keyData.IsKeyCode()) keyCodeTbl[nKeys++] = keyData.GetKeyCode();
	return nKeys;
}

int Stdio::Keyboard::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	return (nKeysMax > 0 && GetKeyDataNB(&keyDataTbl[0]))? 1 : 0;
}

bool Stdio::Keyboard::GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier)
{
	KeyData keyData;
	if (!GetKeyDataNB(&keyData)) {
		// nothing to do
	} else if (keyData.IsKeyCode()) {
		return keyData.GetKeyCode();
	} else {
		char ch = keyData.GetChar();
		if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') ||
				ch == VK_BACK || ch == VK_TAB || ch == VK_RETURN || ch == VK_ESCAPE || ch == VK_SPACE) {
			*pKeyCode = static_cast<uint8_t>(ch);
			return true;
		} else if ('a' <= ch && ch <= 'z') {
			*pKeyCode = static_cast<uint8_t>(ch - 'a' + 'A');
			return true;
		}
	}
	return false;
}

bool Stdio::Keyboard::GetKeyDataNB(KeyData* pKeyData)
{
	int ch;
	while ((ch = ::stdio_getchar_timeout_us(0)) > 0 && !decoder_.FeedChar(ch)) ;
	return decoder_.GetKeyData(pKeyData);
}

}
