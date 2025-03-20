//==============================================================================
// Keyboard.cpp
//==============================================================================
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Keyboard
//------------------------------------------------------------------------------
char Keyboard::GetChar()
{
	KeyData keyData;
	for (;;) {
		if (GetKeyDataNB(&keyData) && keyData.IsChar()) break;
		Tickable::Tick();
	}
	return keyData.GetChar();
}

uint8_t Keyboard::GetKeyCode()
{
	KeyData keyData;
	for (;;) {
		if (GetKeyDataNB(&keyData) && keyData.IsKeyCode()) break;
		Tickable::Tick();
	}
	return keyData.GetKeyCode();
}

//------------------------------------------------------------------------------
// KeyboardDumb
//------------------------------------------------------------------------------
KeyboardDumb KeyboardDumb::Instance;

//------------------------------------------------------------------------------
// KeyboardStdio
//------------------------------------------------------------------------------
KeyboardStdio KeyboardStdio::Instance;

bool KeyboardStdio::GetKeyDataNB(KeyData* pKeyData)
{
	int ch;
	while ((ch = ::stdio_getchar_timeout_us(0)) > 0 && !decoder_.FeedChar(ch)) ;
	return decoder_.GetKeyData(pKeyData);
}

int KeyboardStdio::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	return (nKeysMax > 0 && GetKeyDataNB(&keyDataTbl[0]))? 1 : 0;
}

}
