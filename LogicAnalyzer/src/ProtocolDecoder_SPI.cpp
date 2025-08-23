//==============================================================================
// ProtocolDecoder_SPI.cpp
//==============================================================================
#include "ProtocolDecoder_SPI.h"
#include <cstdlib>

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolDecoder_SPI
//------------------------------------------------------------------------------
ProtocolDecoder_SPI::Factory ProtocolDecoder_SPI::factory_;

ProtocolDecoder_SPI::ProtocolDecoder_SPI(const LogicAnalyzer& logicAnalyzer, const char* name) :
	ProtocolDecoder(logicAnalyzer, name), annotator_(prop_), prop_{-1, GPIO::InvalidPin, GPIO::InvalidPin, GPIO::InvalidPin}
{}

bool ProtocolDecoder_SPI::EvalSubcmd(Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (Shell::Arg::GetAssigned(subcmd, "mode", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= 3) {
			terr.Printf("invalid SPI mode\n");
			return false;
		}
		prop_.mode = num;
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "mosi", &value)) {
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
	}
	terr.Printf("unknown sub-command: %s\n", subcmd);
	return false;
}

bool ProtocolDecoder_SPI::CheckValidity(Printable& terr)
{
    bool rtn = true;
	if (prop_.mode < 0) {
		terr.Printf("specify SPI mode (0-3)\n");
		rtn = false;
	}
	if (prop_.pinMOSI == GPIO::InvalidPin && prop_.pinMISO == GPIO::InvalidPin) {
		terr.Printf("specify MOSI or MISO pin number\n");
		rtn = false;
	}
	if (prop_.pinSCK == GPIO::InvalidPin) {
		terr.Printf("specify SCK pin number\n");
		rtn = false;
	}
	return rtn;
}

void ProtocolDecoder_SPI::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void ProtocolDecoder_SPI::AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol)
{
	//annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

//------------------------------------------------------------------------------
// ProtocolDecoder_SPI::Core
//------------------------------------------------------------------------------
ProtocolDecoder_SPI::Core::Core(const Core& core) : prop_{core.prop_}, stat_{core.stat_},
	nBitsAccum_{core.nBitsAccum_}, bitAccumMOSI_{core.bitAccumMOSI_}, bitAccumMISO_{core.bitAccumMISO_}, signalSCKPrev_{core.signalSCKPrev_}
{}

ProtocolDecoder_SPI::Core::Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForIdle},
	nBitsAccum_{0}, bitAccumMOSI_{0}, bitAccumMISO_{0}, signalSCKPrev_{false}
{}

void ProtocolDecoder_SPI::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
{
	bool bitAccumBeginFlag = false;
	bool signalSCK = event.IsPinHigh(prop_.pinSCK);
	switch (stat_) {
	case Stat::WaitForIdle: {
		if (((prop_.mode == 0 || prop_.mode == 1) && !signalSCK) || ((prop_.mode == 2 || prop_.mode == 3) && signalSCK)) {
			nBitsAccum_ = 0;
			bitAccumMOSI_ = 0x00;
			bitAccumMISO_ = 0x00;
			bitAccumBeginFlag = true;
			stat_ = Stat::BitAccum_SCK;
		}
		break;
	}
	case Stat::BitAccum_SCK: {
		bool bitAccumBeginFlag = false;
		bool signalSCK = event.IsPinHigh(prop_.pinSCK);
		if (signalSCK != signalSCKPrev_ && (((prop_.mode == 0 || prop_.mode == 3) & signalSCK)) ||
											((prop_.mode == 1 || prop_.mode == 2) & !signalSCK)) {
			OnBit(nBitsAccum_, event);
			if (IsValidPin(prop_.pinMOSI)) {
				bitAccumMOSI_ = (bitAccumMOSI_ << 1) | (event.IsPinHigh(prop_.pinMOSI)? 1 : 0);
			}
			if (IsValidPin(prop_.pinMISO)) {
				bitAccumMISO_ = (bitAccumMISO_ << 1) | (event.IsPinHigh(prop_.pinMISO)? 1 : 0);
			}
			nBitsAccum_++;
			if (nBitsAccum_ == 8) {
				OnBitAccumComplete();
				nBitsAccum_ = 0;
				bitAccumMOSI_ = 0x00;
				bitAccumMISO_ = 0x00;
				bitAccumBeginFlag = true;
			}
		}
		break;
	}
	default: break;
	}
	signalSCKPrev_ = signalSCK;
	if (bitAccumBeginFlag) OnBitAccumBegin(eventIter);
}

//------------------------------------------------------------------------------
// ProtocolDecoder_SPI::Core_Annotator
//------------------------------------------------------------------------------
ProtocolDecoder_SPI::Core_Annotator::Core_Annotator(const Property& prop) :
		Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}, adv_{false, 0, 0}
{}

void ProtocolDecoder_SPI::Core_Annotator::ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol)
{
	buffLine_ = buffLine;
	lenBuffLine_ = lenBuffLine;
	piCol_ = piCol;
	Core::ProcessEvent(eventIter, event);
}

void ProtocolDecoder_SPI::Core_Annotator::OnBitAccumBegin(const EventIterator& eventIter)
{
	Core_BitAccumAdv coreAdv(*this);
	EventIterator eventIterAdv(eventIter);
	Event eventAdv;
	while (eventIterAdv.Next(eventAdv)) {
		coreAdv.ProcessEvent(eventIterAdv, eventAdv);
		if (coreAdv.IsComplete()) break;
	}
	adv_.validFlag = coreAdv.IsComplete();
	adv_.bitAccumMOSI = coreAdv.GetBitAccumMOSI();
	adv_.bitAccumMISO = coreAdv.GetBitAccumMISO();
}

void ProtocolDecoder_SPI::Core_Annotator::OnBit(int iBit, const Event& event)
{
	int& iCol = *piCol_;
	if (IsValidPin(prop_.pinMOSI)) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %d", event.IsPinHigh(prop_.pinMOSI)? 1 : 0);
	}
	if (IsValidPin(prop_.pinMISO)) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %d", event.IsPinHigh(prop_.pinMISO)? 1 : 0);
	}
	if (iBit == 3 && adv_.validFlag) {
		if (IsValidPin(prop_.pinMOSI)) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MOSI:0x%02x", adv_.bitAccumMOSI);
		}
		if (IsValidPin(prop_.pinMISO)) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MISO:0x%02x", adv_.bitAccumMISO);
		}
	}
}

void ProtocolDecoder_SPI::Core_Annotator::OnBitAccumComplete()
{
	adv_.validFlag = false;
}

}
