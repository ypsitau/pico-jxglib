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
		static const int OffsetForAscii = 0x100;
	private:
		Stat stat_;
		FIFOBuff<int, 32> buff_;
		int iBuffParameter_;
		int iBuffIntermediate_;
		char buffParameter_[32];
		char buffIntermediate_[32];
	public:
		Decoder();
	public:
		bool HasKeyData() const { return buff_.HasData(); }
		bool GetKeyData(int* pKeyData);
	public:
		void FeedChar(char ch);
	};
};

}

#endif
