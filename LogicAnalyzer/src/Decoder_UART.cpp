//==============================================================================
// Decoder_UART.cpp
//==============================================================================
#include "Decoder_UART.h"
#include <cstdlib>

namespace jxglib::LogicAnalyzerExt {

//------------------------------------------------------------------------------
// Decoder_UART
//------------------------------------------------------------------------------
Decoder_UART::Factory Decoder_UART::factory_;

Decoder_UART::Decoder_UART(const LogicAnalyzer& logicAnalyzer, const char* name) :
		LogicAnalyzer::Decoder(logicAnalyzer, name), annotatorTX_(prop_, "TX"), annotatorRX_{prop_, "RX"},
		prop_{115200, 8, Parity::None, 1}
{}

bool Decoder_UART::EvalSubcmd(Printable& tout, Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (::strcasecmp(subcmd, "print") == 0) {
		if (CheckValidity(terr)) PrintEvent(tout);
	} else if (Shell::Arg::GetAssigned(subcmd, "tx", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid TX pin number\n");
			return false;
		}
		annotatorTX_.SetPin(static_cast<uint>(num));
	} else if (Shell::Arg::GetAssigned(subcmd, "rx", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid RX pin number\n");
			return false;
		}
		annotatorRX_.SetPin(static_cast<uint>(num));
	} else if (Shell::Arg::GetAssigned(subcmd, "baudrate", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num <= 0) {
			terr.Printf("invalid baudrate\n");
			return false;
		}
		prop_.baudrate = num;
	} else if (Shell::Arg::GetAssigned(subcmd, "frame", &value)) {
		// frame: e.g. 8n1, 7e2, 8o1
		if (::strlen(value) != 3) {
			terr.Printf("invalid frame format\n");
			return false;
		}
		char ch = value[0];
		if ('5' <= ch && ch <= '9') {
			prop_.dataBits = ch - '0';
		} else {
			terr.Printf("invalid data bits in frame\n");
			return false;
		}
		ch = ::tolower(value[1]);
		if (ch == 'n') {
			prop_.parity = Parity::None;
		} else if (ch == 'e') {
			prop_.parity = Parity::Even;
		} else if (ch == 'o') {
			prop_.parity = Parity::Odd;
		} else {
			terr.Printf("invalid parity in frame\n");
			return false;
		}
		ch = value[2];
		if ('1' <= ch && ch <= '2') {
			prop_.stopBits = ch - '0';
		} else {
			terr.Printf("invalid stop bits in frame\n");
			return false;
		}
	} else {
		terr.Printf("unknown sub-command: %s\n", subcmd);
		return false;
	}
	return true;
}

bool Decoder_UART::CheckValidity(Printable& terr)
{
    bool rtn = true;
	if (!annotatorTX_.IsValid() && !annotatorRX_.IsValid()) {
		terr.Printf("specify TX or RX pin number\n");
		rtn = false;
	}
	return rtn;
}

void Decoder_UART::Reset()
{
	prop_.timeStampFactor = 1.0 / logicAnalyzer_.GetSampleRate();
	if (annotatorTX_.IsValid()) annotatorTX_.Reset();
	if (annotatorRX_.IsValid()) annotatorRX_.Reset();
}

int Decoder_UART::GetColsAnnotation() const
{
	return annotatorTX_.IsValid() && annotatorRX_.IsValid()? 7 + 2 + 7 : 7;
}

void Decoder_UART::DoAnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	int& iCol = *piCol;
	if (annotatorTX_.IsValid()) {
		annotatorTX_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
	}
	if (annotatorRX_.IsValid()) {
		if (annotatorTX_.IsValid()) iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, "  ");
		annotatorRX_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
	}
}

void Decoder_UART::DoAnnotateWaveStreak(double timeStamp, char* buffLine, int lenBuffLine, int* piCol)
{
}

//------------------------------------------------------------------------------
// Decoder_UART::Core
//------------------------------------------------------------------------------
Decoder_UART::Core::Core(const Core& core) : prop_{core.prop_}, pin_{core.pin_}, stat_{core.stat_},
			signalPrev_{core.signalPrev_}, timeStartNext_{core.timeStartNext_}
{}

Decoder_UART::Core::Core(const Property& prop) : prop_{prop}, pin_{GPIO::InvalidPin}, stat_{Stat::WaitForIdle},
			signalPrev_{false}, timeStartNext_{0.}
{}

void Decoder_UART::Core::Reset()
{
	stat_ = Stat::WaitForIdle;
	signalPrev_ = false;
	timeStartNext_ = 0.0;
}

void Decoder_UART::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
{
	switch (stat_) {
	case Stat::WaitForIdle: {
		if (event.IsPinHigh(pin_)) stat_ = Stat::DataAccum;
		break;
	}
	case Stat::DataAccum: {
		double timeEvent = event.GetTimeStamp() * prop_.timeStampFactor;
		if (signalPrev_ && event.IsPinLow(pin_) && timeEvent > timeStartNext_) {
			double timePerBit = 1.0 / prop_.baudrate;
			EventIterator eventIterAdv(eventIter);
			uint16_t bitAccum = 0;
			uint8_t bitValue = 0;
			int nBitsTotal = prop_.dataBits;
			if (prop_.parity != Parity::None) nBitsTotal++;
			timeStartNext_ = timeEvent + timePerBit * (nBitsTotal + .5);
			double timeProbe = timeEvent + timePerBit * 1.5;
			int nBitsAccum = 0;
			for (Event eventAdv; eventIterAdv.Next(eventAdv); ) {
				double timeEvent = eventAdv.GetTimeStamp() * prop_.timeStampFactor;
				for ( ; timeProbe < timeEvent && nBitsAccum < nBitsTotal; timeProbe += timePerBit) {
					bitAccum |= (bitValue << nBitsAccum);
					nBitsAccum++;
				}
				bitValue = eventAdv.IsPinHigh(pin_)? 1 : 0;
			}
			for ( ; nBitsAccum < nBitsTotal; nBitsAccum++) {
				bitAccum |= (bitValue << nBitsAccum);
			}
			uint8_t data = static_cast<uint8_t>(bitAccum & ((1 << prop_.dataBits) - 1));
			uint8_t parity = (bitAccum >> prop_.dataBits) & 1;
			OnStartBit(data, parity);
		}
		break;
	}
	default: break;
	}
	signalPrev_ = event.IsPinHigh(pin_);
}

//------------------------------------------------------------------------------
// Decoder_UART::Core_Annotator
//------------------------------------------------------------------------------
Decoder_UART::Core_Annotator::Core_Annotator(const Property& prop, const char* name) :
		Core(prop), name_{name}, buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}
{}

void Decoder_UART::Core_Annotator::ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol)
{
	buffLine_ = buffLine;
	lenBuffLine_ = lenBuffLine;
	piCol_ = piCol;
	Core::ProcessEvent(eventIter, event);
}

void Decoder_UART::Core_Annotator::OnStartBit(uint8_t data, uint8_t parity)
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "%s:0x%02x", name_, data);
}

}
