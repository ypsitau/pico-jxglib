//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
Terminal::Terminal(int bytesHistoryBuff, Keyboard& keyboard) : Editable(bytesHistoryBuff), pKeyboard_{&keyboard}
{
}

char Terminal::GetChar()
{
	KeyData keyData;
	for (;;) {
		if (GetKeyData(keyData) && keyData.IsCharCode()) break;
		Tickable::Tick();
	}
	return keyData.GetCharCode();
}

uint8_t Terminal::GetKeyCode()
{
	KeyData keyData;
	for (;;) {
		if (GetKeyData(keyData) && keyData.IsKeyCode()) break;
		Tickable::Tick();
	}
	return keyData.GetKeyCode();
}

}
