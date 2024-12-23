//==============================================================================
// jxglib/VT100.h
//==============================================================================
#ifndef PICO_JXGLIB_VT100_H
#define PICO_JXGLIB_VT100_H
#include "pico/stdlib.h"
#include "jxglib/KeyCode.h"
#include "jxglib/FIFOBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VT100
//------------------------------------------------------------------------------
class VT100 {
public:
	class Decoder {
	public:
		enum class Stat {
			First, Escape,
			SS2,
			SS3,
			DCS,
			CSI_Parameter, CSI_Intermediate, CSI_Final,
			ST,
			OSC,
			SOS,
			PM,
			APC,
		};
	private:
		Stat stat_;
		FIFOBuff<int, 32> buff_;
	public:
		Decoder();
	public:
		bool HasKeyData() const { return buff_.HasData(); }
		int GetKeyData() { return buff_.ReadData(); }
	public:
		void FeedChar(char ch);
	};
};

}

#endif
