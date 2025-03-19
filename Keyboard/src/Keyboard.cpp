//==============================================================================
// Keyboard.cpp
//==============================================================================
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
KeyboardDumb KeyboardDumb::Instance;

//------------------------------------------------------------------------------
// KeyboardStdio
//------------------------------------------------------------------------------
KeyboardStdio KeyboardStdio::Instance;

bool KeyboardStdio::GetKeyData(KeyData& keyData)
{
	int ch;
	while ((ch = ::stdio_getchar_timeout_us(0)) > 0) decoder_.FeedChar(ch);
	return decoder_.GetKeyData(keyData);
}

int KeyboardStdio::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	return (nKeysMax > 0 && GetKeyData(keyDataTbl[0]))? 1 : 0;
}

}
