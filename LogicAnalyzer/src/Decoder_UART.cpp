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
		LogicAnalyzer::Decoder(logicAnalyzer, name), annotatorTX_(prop_), annotatorRX_{prop_},
		prop_{115200, 8, Parity::None, 1}
{}

bool Decoder_UART::EvalSubcmd(Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (Shell::Arg::GetAssigned(subcmd, "tx", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid TX pin number\n");
			return false;
		}
		annotatorTX_.SetPin(static_cast<uint>(num));
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "rx", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid RX pin number\n");
			return false;
		}
		annotatorRX_.SetPin(static_cast<uint>(num));
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "baud", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num <= 0) {
			terr.Printf("invalid baudrate\n");
			return false;
		}
		prop_.baudrate = num;
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "frame", &value)) {
		// frame: e.g. 8n1, 7e2, 8o1
		int len = (int)strlen(value);
		if (len < 3 || len > 4) {
			terr.Printf("invalid frame format\n");
			return false;
		}
		int databits = value[0] - '0';
		if (databits < 5 || databits > 9) {
			terr.Printf("invalid data bits in frame\n");
			return false;
		}
		char parity = value[1];
		int stopbits = 1;
		if (len == 3) {
			stopbits = value[2] - '0';
		} else if (len == 4) {
			// e.g. 10n1
			databits = (value[0] - '0') * 10 + (value[1] - '0');
			parity = value[2];
			stopbits = value[3] - '0';
		}
		if (stopbits != 1 && stopbits != 2) {
			terr.Printf("invalid stop bits in frame\n");
			return false;
		}
		prop_.dataBits = databits;
		prop_.stopBits = stopbits;
		if (parity == 'n' || parity == 'N') {
			prop_.parity = Parity::None;
		} else if (parity == 'e' || parity == 'E') {
			prop_.parity = Parity::Even;
		} else if (parity == 'o' || parity == 'O') {
			prop_.parity = Parity::Odd;
		} else {
			terr.Printf("invalid parity in frame\n");
			return false;
		}
		return true;
	}
	terr.Printf("unknown sub-command: %s\n", subcmd);
	return false;
}

bool Decoder_UART::CheckValidity(Printable& terr)
{
    bool rtn = true;
	if (annotatorTX_.GetPin() == GPIO::InvalidPin && annotatorRX_.GetPin() == GPIO::InvalidPin) {
		terr.Printf("specify TX or RX pin number\n");
		rtn = false;
	}
	return rtn;
}

void Decoder_UART::Reset()
{
	prop_.timeStampFactor = 1.0 / logicAnalyzer_.GetSampleRate();
	annotatorTX_.Reset();
	annotatorRX_.Reset();
}

void Decoder_UART::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotatorTX_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
	annotatorRX_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void Decoder_UART::AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol)
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
	bool rx = event.IsPinHigh(pin_);
	switch (stat_) {
	case Stat::WaitForIdle: {
		if (event.IsPinHigh(pin_)) {
			signalPrev_ = true;
			stat_ = Stat::DataAccum;
		}
		break;
	}
	case Stat::DataAccum: {
		double timeEvent = event.GetTimeStamp() * prop_.timeStampFactor;
		if (signalPrev_ && event.IsPinLow(pin_) && timeEvent > timeStartNext_) {
			double timePerBit = 1.0 / prop_.baudrate;
			timeStartNext_ = timeEvent + timePerBit * (prop_.dataBits + 1.5);
			if (prop_.parity != Parity::None) timeStartNext_ += timePerBit;	// skip parity bit
			EventIterator eventIterAdv(eventIter);
			Event eventAdv;
			uint16_t bitAccum = 0;
			int nBitsAccum = 0;
			int nBitsTotal = prop_.dataBits;
			if (prop_.parity != Parity::None) nBitsTotal++;
			uint8_t bitValue = 0;
			double timeProbe = timeEvent + timePerBit * 1.5;
			while (eventIterAdv.Next(eventAdv)) {
				double timeEvent = eventAdv.GetTimeStamp() * prop_.timeStampFactor;
				for ( ; timeProbe < timeEvent && nBitsAccum < nBitsTotal; timeProbe += timePerBit) {
					bitAccum |= (bitValue << nBitsAccum);
					nBitsAccum++;
				}
				bitValue = eventAdv.IsPinHigh(pin_)? 1 : 0;
			}
			uint8_t data = static_cast<uint8_t>(bitAccum & ((1 << prop_.dataBits) - 1));
			uint8_t parity = (bitAccum >> prop_.dataBits) & 1;
			OnStartBit(data, parity);
		}
		signalPrev_ = event.IsPinHigh(pin_);
		break;
	}
	default: break;
	}
}

//------------------------------------------------------------------------------
// Decoder_UART::Core_Annotator
//------------------------------------------------------------------------------
Decoder_UART::Core_Annotator::Core_Annotator(const Property& prop) :
		Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}
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
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Data:0x%02x", data);
}

}
