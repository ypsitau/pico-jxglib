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
	LogicAnalyzer::Decoder(logicAnalyzer, name), annotator_(prop_), prop_{GPIO::InvalidPin, 9600, 8, false, false, 1}
{}

bool Decoder_UART::EvalSubcmd(Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;
	const char* value = nullptr;
	if (Shell::Arg::GetAssigned(subcmd, "rx", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid RX pin number\n");
			return false;
		}
		prop_.pinRX = static_cast<uint>(num);
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
			prop_.parityEnable = false;
		} else if (parity == 'e' || parity == 'E') {
			prop_.parityEnable = true; prop_.parityOdd = false;
		} else if (parity == 'o' || parity == 'O') {
			prop_.parityEnable = true; prop_.parityOdd = true;
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
	if (prop_.pinRX == GPIO::InvalidPin) {
		terr.Printf("specify RX pin number\n");
		rtn = false;
	}
	if (prop_.baudrate <= 0) {
		terr.Printf("specify baudrate\n");
		rtn = false;
	}
	return rtn;
}

void Decoder_UART::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void Decoder_UART::AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol)
{
	//annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

//------------------------------------------------------------------------------
// Decoder_UART::Core
//------------------------------------------------------------------------------
Decoder_UART::Core::Core(const Core& core) : prop_{core.prop_}, stat_{core.stat_}, field_{core.field_},
	nBitsAccum_{core.nBitsAccum_}, bitAccum_{core.bitAccum_}, bitIdx_{core.bitIdx_}, lastRX_{core.lastRX_}, sampleCounter_{core.sampleCounter_},
	lastStartTime_{core.lastStartTime_}, bitSampleTime_{core.bitSampleTime_}, bitsToSample_{core.bitsToSample_}, bitPeriod_{core.bitPeriod_}, stopBitSampleTime_{core.stopBitSampleTime_}
{}

Decoder_UART::Core::Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForIdle}, field_{Field::Data},
	nBitsAccum_{0}, bitAccum_{0}, bitIdx_{0}, lastRX_{true}, sampleCounter_{0},
	lastStartTime_{0}, bitSampleTime_{0}, bitsToSample_{0}, bitPeriod_{0}, stopBitSampleTime_{0}
{}

void Decoder_UART::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
{
	// Get the current RX pin state
	bool rx = event.IsPinHigh(prop_.pinRX);
	switch (stat_) {
	case Stat::WaitForIdle: {
		// Wait for the line to go low (start bit)
		if (!rx) {
			stat_ = Stat::StartBit;
			OnStartBit();
			bitIdx_ = 0;
			bitAccum_ = 0;
			// Initialize sampling timing variables
			lastStartTime_ = event.GetTimeStamp();
			bitPeriod_ = 1000000ULL / prop_.baudrate; // Bit period in microseconds
			// First data bit is sampled at 1.5 bit periods after start
			bitSampleTime_ = lastStartTime_ + bitPeriod_ + bitPeriod_/2;
			bitsToSample_ = prop_.dataBits;
			stopBitSampleTime_ = bitSampleTime_ + bitPeriod_ * bitsToSample_;
		}
		break;
	}
	case Stat::StartBit: {
		// Immediately transition to BitAccum state
		stat_ = Stat::BitAccum;
		break;
	}
	case Stat::BitAccum: {
		// Sample each data bit at the calculated sampling time
		if (event.GetTimeStamp() >= bitSampleTime_ && bitIdx_ < prop_.dataBits) {
			bool rxSample = event.IsPinHigh(prop_.pinRX);
			OnBit(field_, bitIdx_, rxSample);
			bitAccum_ |= (rxSample ? 1 : 0) << bitIdx_;
			bitIdx_++;
			bitSampleTime_ += bitPeriod_;
			if (bitIdx_ >= prop_.dataBits) {
				stat_ = Stat::StopBit;
			}
		}
		break;
	}
	case Stat::StopBit: {
		// Wait until the stop bit sampling time, then sample and finish the frame
		if (event.GetTimeStamp() >= stopBitSampleTime_) {
			bool rxSample = event.IsPinHigh(prop_.pinRX);
			OnStopBit(rxSample);
			// Parity is not implemented yet
			OnByte(static_cast<uint8_t>(bitAccum_ & 0xFF), false);
			stat_ = Stat::WaitForIdle;
		}
		break;
	}
	default: break;
	}
	// Store the last RX state
	lastRX_ = rx;
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

void Decoder_UART::Core_Annotator::OnStartBit()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Start");
}

void Decoder_UART::Core_Annotator::OnBit(Field field, int iBit, bool bitValue)
{
	int& iCol = *piCol_;
	if (iBit == 0) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Data:");
	}
	if (iBit < 8) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, "%d", bitValue ? 1 : 0);
	}
}

void Decoder_UART::Core_Annotator::OnStopBit(bool valid)
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Stop");
}

void Decoder_UART::Core_Annotator::OnByte(uint8_t data, bool parityErr)
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " 0x%02X", data);
	if (parityErr) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " ParityErr");
	}
}

}
