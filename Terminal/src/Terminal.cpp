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

//------------------------------------------------------------------------------
// TerminalDumb
//------------------------------------------------------------------------------
TerminalDumb TerminalDumb::Instance;

TerminalDumb::TerminalDumb() : Terminal(0, KeyboardDumb::Instance)
{
}

}
