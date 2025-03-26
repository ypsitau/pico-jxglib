//==============================================================================
// Stdio.cpp
//==============================================================================
#include "jxglib/Stdio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Stdio::Printable
//------------------------------------------------------------------------------
Stdio::Printable Stdio::Printable::Instance;

//------------------------------------------------------------------------------
// Stdio::Keyboard
//------------------------------------------------------------------------------
Stdio::Keyboard Stdio::Keyboard::Instance;

int Stdio::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
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

bool Stdio::Keyboard::GetKeyDataNB(KeyData* pKeyData)
{
	int ch;
	while ((ch = ::stdio_getchar_timeout_us(0)) > 0 && !decoder_.FeedChar(ch)) ;
	return decoder_.GetKeyData(pKeyData);
}

}
