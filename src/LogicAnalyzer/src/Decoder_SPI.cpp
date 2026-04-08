//==============================================================================
// Decoder_SPI.cpp
//==============================================================================
#include "Decoder_SPI.h"
#include <cstdlib>

namespace jxglib::LogicAnalyzerExt {

//------------------------------------------------------------------------------
// Decoder_SPI
//------------------------------------------------------------------------------
Decoder_SPI::Factory Decoder_SPI::factory_;

Decoder_SPI::Decoder_SPI(const LogicAnalyzer& logicAnalyzer, const char* name) :
	LogicAnalyzer::Decoder(logicAnalyzer, name), annotator_(prop_), prop_{-1, GPIO::InvalidPin, GPIO::InvalidPin, GPIO::InvalidPin}
{}

bool Decoder_SPI::EvalSubcmd(Printable& tout, Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (::strcasecmp(subcmd, "print") == 0) {
		if (CheckValidity(terr)) PrintEvent(tout);
	} else if (Shell::Arg::GetAssigned(subcmd, "mode", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num > 3) {
			terr.Printf("invalid SPI mode\n");
			return false;
		}
		prop_.mode = num;
	} else if (Shell::Arg::GetAssigned(subcmd, "mosi", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid MOSI pin number\n");
			return false;
		}
		prop_.pinMOSI = static_cast<uint>(num);
	} else if (Shell::Arg::GetAssigned(subcmd, "miso", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid MISO pin number\n");
			return false;
		}
		prop_.pinMISO = static_cast<uint>(num);
	} else if (Shell::Arg::GetAssigned(subcmd, "sck", &value) ||
				Shell::Arg::GetAssigned(subcmd, "scl", &value) ||
				Shell::Arg::GetAssigned(subcmd, "sclk", &value) ||
				Shell::Arg::GetAssigned(subcmd, "clk", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid SCK pin number\n");
			return false;
		}
		prop_.pinSCK = static_cast<uint>(num);
	} else {
		terr.Printf("unknown sub-command: %s\n", subcmd);
		return false;
	}
	return true;
}

bool Decoder_SPI::CheckValidity(Printable& terr)
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

int Decoder_SPI::GetColsAnnotation() const
{
	return (prop_.pinMOSI != GPIO::InvalidPin && prop_.pinMISO != GPIO::InvalidPin)? 10 + 2 + 10 : 10;
}

void Decoder_SPI::DoAnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void Decoder_SPI::DoAnnotateWaveStreak(double timeStamp, char* buffLine, int lenBuffLine, int* piCol)
{
}

//------------------------------------------------------------------------------
// Decoder_SPI::Core
//------------------------------------------------------------------------------
Decoder_SPI::Core::Core(const Core& core) : prop_{core.prop_}, stat_{core.stat_},
	nBitsAccum_{core.nBitsAccum_}, bitAccumMOSI_{core.bitAccumMOSI_}, bitAccumMISO_{core.bitAccumMISO_}, signalSCKPrev_{core.signalSCKPrev_}
{}

Decoder_SPI::Core::Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForIdle},
	nBitsAccum_{0}, bitAccumMOSI_{0}, bitAccumMISO_{0}, signalSCKPrev_{false}
{}

void Decoder_SPI::Core::Reset()
{
	stat_ = Stat::WaitForIdle;
	nBitsAccum_ = 0;
	bitAccumMOSI_ = 0;
	bitAccumMISO_ = 0;
	signalSCKPrev_ = false;
}

void Decoder_SPI::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
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
		if (signalSCK != signalSCKPrev_ && (((prop_.mode == 0 || prop_.mode == 3) & signalSCK)) ||
											((prop_.mode == 1 || prop_.mode == 2) & !signalSCK)) {
			if (IsValidPin(prop_.pinMOSI)) {
				bitAccumMOSI_ = (bitAccumMOSI_ << 1) | (event.IsPinHigh(prop_.pinMOSI)? 1 : 0);
			}
			if (IsValidPin(prop_.pinMISO)) {
				bitAccumMISO_ = (bitAccumMISO_ << 1) | (event.IsPinHigh(prop_.pinMISO)? 1 : 0);
			}
			OnBit(nBitsAccum_, event);
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
// Decoder_SPI::Core_Annotator
//------------------------------------------------------------------------------
Decoder_SPI::Core_Annotator::Core_Annotator(const Property& prop) :
		Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}, adv_{false, 0, 0}
{}

void Decoder_SPI::Core_Annotator::ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol)
{
	buffLine_ = buffLine;
	lenBuffLine_ = lenBuffLine;
	piCol_ = piCol;
	Core::ProcessEvent(eventIter, event);
}

void Decoder_SPI::Core_Annotator::OnBitAccumBegin(const EventIterator& eventIter)
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

void Decoder_SPI::Core_Annotator::OnBit(int iBit, const Event& event)
{
	int& iCol = *piCol_;
	if (IsValidPin(prop_.pinMOSI)) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "%d", event.IsPinHigh(prop_.pinMOSI)? 1 : 0);
		if (iBit == 0) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " msb      ");
		} else if (iBit == 7) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " lsb      ");
		} else if (iBit == 3 && adv_.validFlag) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MOSI:0x%02x", adv_.bitAccumMOSI);
		} else {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "          ");
		}
	}
	if (IsValidPin(prop_.pinMISO)) {
		if (IsValidPin(prop_.pinMOSI)) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "  ");
		}
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "%d", event.IsPinHigh(prop_.pinMISO)? 1 : 0);
		if (iBit == 0) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " msb      ");
		} else if (iBit == 7) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " lsb      ");
		} else if (iBit == 3 && adv_.validFlag) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " MISO:0x%02x", adv_.bitAccumMISO);
		} else {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "          ");
		}
	}
}

void Decoder_SPI::Core_Annotator::OnBitAccumComplete()
{
	adv_.validFlag = false;
}

//------------------------------------------------------------------------------
// Decoder_SPI::Core_BitAccumAdv
//------------------------------------------------------------------------------
void Decoder_SPI::Core_BitAccumAdv::OnBitAccumComplete()
{
	completeFlag_ = true;
	saved_.bitAccumMOSI = bitAccumMOSI_;
	saved_.bitAccumMISO = bitAccumMISO_;
}

}
