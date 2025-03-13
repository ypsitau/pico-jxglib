//==============================================================================
// VT100.cpp
//==============================================================================
#include <string.h>
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VT100
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VT100::Decoder
//------------------------------------------------------------------------------
VT100::Decoder::Decoder() : stat_{Stat::First}
{
}

void VT100::Decoder::FeedChar(char ch)
{
	bool contFlag = false;
	//::printf("%02x %c\n", ch, (ch == 0x1b)? 'e' : ch);
	do {
		contFlag = false;
		switch (stat_) {
		case Stat::First: {
			switch (ch) {
			case 0x08: {
				buff_.WriteData(VK_BACK);
				break;
			}
			case 0x0d: {
				buff_.WriteData(VK_RETURN);
				break;
			}
			case 0x1b: {
				stat_ = Stat::Escape;
				break;
			}
			case 0x7f: {
				buff_.WriteData(VK_DELETE);
				break;
			}
			default: {
				buff_.WriteData(OffsetForAscii + ch);
				break;
			}
			}
			break;
		}
		case Stat::Escape: {
			switch (ch) {
			case 0x1b: {
				buff_.WriteData(0x1b);
				stat_ = Stat::First;
				break;
			}
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
				iBuffParameter_ = iBuffIntermediate_ = 0;
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
				if (iBuffParameter_ < count_of(buffParameter_) - 1) buffParameter_[iBuffParameter_++] = ch;
			} else {
				buffParameter_[iBuffParameter_] = '\0';
				contFlag = true;
				stat_ = Stat::CSI_Intermediate;
			}
			break;
		}
		case Stat::CSI_Intermediate: {
			if (0x20 <= ch && ch <= 0x2f) {
				if (iBuffIntermediate_ < count_of(buffIntermediate_) - 1) buffIntermediate_[iBuffIntermediate_++] = ch;
			} else {
				buffIntermediate_[iBuffIntermediate_] = '\0';
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
				buff_.WriteData(VK_RIGHT);
				break;
			}
			case 'D': {
				buff_.WriteData(VK_LEFT);
				break;
			}
			case '~': {
				if (::strcmp(buffParameter_, "1") == 0) {
					buff_.WriteData(VK_HOME);
				} else if (::strcmp(buffParameter_, "4") == 0) {
					buff_.WriteData(VK_END);
				} else if (::strcmp(buffParameter_, "5") == 0) {
					buff_.WriteData(VK_PRIOR);
				} else if (::strcmp(buffParameter_, "6") == 0) {
					buff_.WriteData(VK_NEXT);
				} else {
				}
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

bool VT100::Decoder::GetKeyData(Keyboard::KeyData& keyData)
{
	if (HasKeyData()) {
		int code = buff_.ReadData();
		bool keyCodeFlag = (code < OffsetForAscii);
		if (!keyCodeFlag) code -= OffsetForAscii;
		const uint8_t modifier = 0;
		keyData = Keyboard::KeyData(code, keyCodeFlag, modifier);
		return true;
	}
	return false;
}

}
