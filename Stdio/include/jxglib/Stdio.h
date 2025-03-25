//==============================================================================
// jxglib/Stdio.h
//==============================================================================
#ifndef PICO_JXGLIB_STDIO_H
#define PICO_JXGLIB_STDIO_H
#include "pico/stdlib.h"
#include "jxglib/Keyboard.h"
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------
class Stdio {
public:
	class Keyboard : public jxglib::Keyboard {
	private:
		VT100::Decoder decoder_;
	public:
		static Keyboard Instance;
	public:
		virtual bool GetKeyDataNB(KeyData* pKeyData) override;
	};
public:
	Stdio() {}
};

}

#endif
