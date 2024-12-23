//==============================================================================
// VT100.cpp
//==============================================================================
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VT100::Decoder
//------------------------------------------------------------------------------
VT100::Decoder::Decoder() : stat_{Stat::First}
{
}

void VT100::Decoder::FeedChar(char ch)
{
	bool contFlag = false;
	::printf("%02x %c\n", ch, (ch == 0x1b)? 'e' : ch);
	do {
		contFlag = false;
		switch (stat_) {
		case Stat::First: {
			if (ch == 0x1b) {
				stat_ = Stat::Escape;
			} else {
				buff_.WriteData(ch);
			}
			break;
		}
		case Stat::Escape: {
			switch (ch) {
			case 'N': {
				stat_ = Stat::SS2;
				break;
			}
			case 'O': {
				stat_ = Stat::SS3;
				break;
			}
			case 'P': {
				stat_ = Stat::DCS;
				break;
			}
			case '[': {
				stat_ = Stat::CSI_Parameter;
				break;
			}
			case '\\': {
				stat_ = Stat::ST;
				break;
			}
			case ']': {
				stat_ = Stat::OSC;
				break;
			}
			case 'X': {
				stat_ = Stat::SOS;
				break;
			}
			case '^': {
				stat_ = Stat::PM;
				break;
			}
			case '_': {
				stat_ = Stat::APC;
				break;
			}
			default: {
				stat_ = Stat::First;
				break;
			}
			}
			break;
		}
		case Stat::SS2: {
			stat_ = Stat::First;
			break;
		}
		case Stat::SS3: {
			switch (ch) {
			default: {
				stat_ = Stat::First;
				break;
			}
			}
			break;
		}
		case Stat::DCS: {
			stat_ = Stat::First;
			break;
		}
		case Stat::CSI_Parameter: {
			if (0x30 <= ch && ch <= 0x3f) {
			} else {
				contFlag = true;
				stat_ = Stat::CSI_Intermediate;
			}
			break;
		}
		case Stat::CSI_Intermediate: {
			if (0x20 <= ch && ch <= 0x2f) {
			} else {
				contFlag = true;
				stat_ = Stat::CSI_Final;
			}
			break;
		}
		case Stat::CSI_Final: {
			switch (ch) {
			case 'A': {
				buff_.WriteData(VK_UP);
				break;
			}
			case 'B': {
				buff_.WriteData(VK_DOWN);
				break;
			}
			case 'C': {
				buff_.WriteData(VK_LEFT);
				break;
			}
			case 'D': {
				buff_.WriteData(VK_RIGHT);
				break;
			}
			}
			stat_ = Stat::First;
			break;
		}
		case Stat::ST: {
			stat_ = Stat::First;
			break;
		}
		case Stat::OSC: {
			stat_ = Stat::First;
			break;
		}
		case Stat::SOS: {
			stat_ = Stat::First;
			break;
		}
		case Stat::PM: {
			stat_ = Stat::First;
			break;
		}
		case Stat::APC: {
			stat_ = Stat::First;
			break;
		}
		default: {
			stat_ = Stat::First;
			break;
		}
		}
	} while (contFlag);
}

}
