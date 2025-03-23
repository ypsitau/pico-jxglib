//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Editable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
class Terminal : public Printable, public Editable {
private:
	Keyboard* pKeyboard_;
public:
	Terminal(int bytesHistoryBuff, Keyboard& keyboard);
public:
	void AttachInput(Keyboard& keyboard) { pKeyboard_ = &keyboard; }
	Keyboard& GetKeyboard() { return *pKeyboard_; }
	char GetChar() { return GetKeyboard().GetChar(); }
	bool GetKeyDataNB(KeyData* pKeyData) { return GetKeyboard().GetKeyDataNB(pKeyData); }
	int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) {
		return GetKeyboard().SenseKeyData(keyDataTbl, nKeysMax);
	}
};

}

#endif
