//==============================================================================
// ProtocolAnalyzer_SPI.cpp
//==============================================================================
#include "ProtocolAnalyzer_SPI.h"
#include <cstdlib>

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolAnalyzer_SPI
//------------------------------------------------------------------------------
ProtocolAnalyzer_SPI::Factory ProtocolAnalyzer_SPI::factory_;

ProtocolAnalyzer_SPI::ProtocolAnalyzer_SPI(const LogicAnalyzer& logicAnalyzer, const char* name) :
	ProtocolAnalyzer(logicAnalyzer, name), annotator_(prop_), prop_{GPIO::InvalidPin, GPIO::InvalidPin, GPIO::InvalidPin, GPIO::InvalidPin}
{}

bool ProtocolAnalyzer_SPI::EvalSubcmd(Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (Shell::Arg::GetAssigned(subcmd, "mosi", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid MOSI pin number\n");
			return false;
		}
		prop_.pinMOSI = static_cast<uint>(num);
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "miso", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid MISO pin number\n");
			return false;
		}
		prop_.pinMISO = static_cast<uint>(num);
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "sck", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid SCK pin number\n");
			return false;
		}
		prop_.pinSCK = static_cast<uint>(num);
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "cs", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid CS pin number\n");
			return false;
		}
		prop_.pinCS = static_cast<uint>(num);
		return true;
	}
	terr.Printf("unknown sub-command: %s\n", subcmd);
	return false;
}

bool ProtocolAnalyzer_SPI::CheckValidity(Printable& terr)
{
    bool rtn = true;
	if (prop_.pinMOSI == GPIO::InvalidPin) {
		terr.Printf("specify MOSI pin number\n");
		rtn = false;
	}
	if (prop_.pinMISO == GPIO::InvalidPin) {
		terr.Printf("specify MISO pin number\n");
		rtn = false;
	}
	if (prop_.pinSCK == GPIO::InvalidPin) {
		terr.Printf("specify SCK pin number\n");
		rtn = false;
	}
	if (prop_.pinCS == GPIO::InvalidPin) {
		terr.Printf("specify CS pin number\n");
		rtn = false;
	}
	return rtn;
}

void ProtocolAnalyzer_SPI::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void ProtocolAnalyzer_SPI::AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol)
{
	//annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

//------------------------------------------------------------------------------
// ProtocolAnalyzer_SPI::Core
//------------------------------------------------------------------------------
ProtocolAnalyzer_SPI::Core::Core(const Core& core) : prop_{core.prop_}, stat_{core.stat_}, field_{core.field_},
	nBitsAccum_{core.nBitsAccum_}, bitAccum_{core.bitAccum_}, csPrev_{core.csPrev_}, sckPrev_{core.sckPrev_}
{}

ProtocolAnalyzer_SPI::Core::Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForStable}, field_{Field::Data},
	nBitsAccum_{0}, bitAccum_{0}, csPrev_{true}, sckPrev_{false}
{}

void ProtocolAnalyzer_SPI::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
{
	// Read current SPI pin states
	bool cs = event.IsPinHigh(prop_.pinCS);
	bool sck = event.IsPinHigh(prop_.pinSCK);
	bool mosi = event.IsPinHigh(prop_.pinMOSI);
	bool miso = event.IsPinHigh(prop_.pinMISO);
	switch (stat_) {
	case Stat::WaitForStable: {
		// Wait for CS to go low (start of SPI transaction)
		if (!cs) {
			stat_ = Stat::Start_CS_Fall;
		}
		break;
	}
	case Stat::Start_CS_Fall: {
		// If CS goes high, stop transaction
		if (!csPrev_ && cs) {
			OnStop();
			stat_ = Stat::WaitForStable;
		} else if (!cs && sck && !sckPrev_) {
			// On SCK high (rising edge) with CS low, start bit accumulation
			OnStart();
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Data;
			stat_ = Stat::BitAccum_SCK_Edge;
		}
		break;
	}
	case Stat::BitAccum_SCK_Edge: {
		// If CS goes high, stop transaction
		if (cs) {
			OnStop();
			stat_ = Stat::WaitForStable;
		} else if (sck && !sckPrev_) {
			// On SCK rising edge, sample MOSI/MISO and accumulate bits
			OnBit(field_, nBitsAccum_, mosi, miso);
			bitAccum_ = (bitAccum_ << 1) | (mosi ? 1 : 0);
			nBitsAccum_++;
			if (nBitsAccum_ == 8) {
				// 8 bits accumulated, call completion handler
				OnBitAccumComplete(field_, bitAccum_, 0); // TODO: Add MISO data if needed
				nBitsAccum_ = 0;
				bitAccum_ = 0x000;
			}
		}
		break;
	}
	default: break;
	}
	// Store previous pin states for edge detection
	csPrev_ = cs;
	sckPrev_ = sck;
}

//------------------------------------------------------------------------------
// ProtocolAnalyzer_SPI::Core_Annotator
//------------------------------------------------------------------------------
ProtocolAnalyzer_SPI::Core_Annotator::Core_Annotator(const Property& prop) :
		Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}, adv_{false, 0, 0}
{}

void ProtocolAnalyzer_SPI::Core_Annotator::ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol)
{
	buffLine_ = buffLine;
	lenBuffLine_ = lenBuffLine;
	piCol_ = piCol;
	Core::ProcessEvent(eventIter, event);
}

void ProtocolAnalyzer_SPI::Core_Annotator::OnStart()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Start");
}

void ProtocolAnalyzer_SPI::Core_Annotator::OnBitAccumBegin(const EventIterator& eventIter)
{
	// 省略: 必要に応じて実装
}

void ProtocolAnalyzer_SPI::Core_Annotator::OnStop()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Stop");
}

void ProtocolAnalyzer_SPI::Core_Annotator::OnBit(Field field, int iBit, bool bitMOSI, bool bitMISO)
{
	int& iCol = *piCol_;
	if (iBit == 7) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MOSI:0x%02X", bitMOSI ? 1 : 0);
	}
	// 必要に応じてMISOも表示
}

void ProtocolAnalyzer_SPI::Core_Annotator::OnBitAccumComplete(Field field, uint16_t bitAccumMOSI, uint16_t bitAccumMISO)
{
	adv_.validFlag = false;
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MOSI:0x%02X", bitAccumMOSI & 0xFF);
	// 必要に応じてMISOも表示
}

}
