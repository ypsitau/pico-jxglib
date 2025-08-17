//==============================================================================
// ProtocolAnalyzer_I2C.cpp
//==============================================================================
#include "ProtocolAnalyzer_I2C.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolAnalyzer_I2C
//------------------------------------------------------------------------------
ProtocolAnalyzer_I2C::Factory ProtocolAnalyzer_I2C::factory_;

ProtocolAnalyzer_I2C::ProtocolAnalyzer_I2C(const LogicAnalyzer& logicAnalyzer, const char* name) :
	ProtocolAnalyzer(logicAnalyzer, name), stat_{Stat::WaitForStable}, field_{Field::Address},
	nBitsAccum_{0}, bitAccum_{0}, signalSDAPrev_{false}, prop_{GPIO::InvalidPin, GPIO::InvalidPin}, annotator_(prop_)
{}

bool ProtocolAnalyzer_I2C::EvalSubcmd(Printable& terr, const char* subcmd)
{
	char* endptr = nullptr;;
	const char* value = nullptr;
	if (Shell::Arg::GetAssigned(subcmd, "sda", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid SDA pin number\n");
			return false;
		}
		prop_.pinSDA = static_cast<uint>(num);
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "scl", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid SCL pin number\n");
			return false;
		}
		prop_.pinSCL = static_cast<uint>(num);
		return true;
	}
	terr.Printf("unknown parameter: %s\n", subcmd);
	return false;
}

bool ProtocolAnalyzer_I2C::FinishSubcmd(Printable& terr)
{
	if (prop_.pinSDA == GPIO::InvalidPin) {
		terr.Printf("specify SDA pin number\n");
		return false;
	}
	if (prop_.pinSCL == GPIO::InvalidPin) {
		terr.Printf("specify SCL pin number\n");
		return false;
	}
	return true;
}

void ProtocolAnalyzer_I2C::AnnotateWave(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(event, buffLine, lenBuffLine, piCol);
#if 0

	int& iCol = *piCol;

	switch (stat_) {
	case Stat::WaitForStable: {
		if (event.IsPinHigh(prop_.pinSCL, prop_.pinSDA)) {
			stat_ = Stat::Start_SDA_Fall;
		}
		break;
	}
	case Stat::Start_SDA_Fall: {
		if (event.IsPinLow(prop_.pinSDA)) {		// SDA falls while SCL high: Start Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Start");
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	case Stat::BitAccum_SCL_Fall: {
		if (!signalSDAPrev_ && event.IsPinHigh(prop_.pinSDA)) {		// SDA rises while SCL high: Stop Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Stop");
			stat_ = Stat::Start_SDA_Fall;
		} else if (signalSDAPrev_ && event.IsPinLow(prop_.pinSDA)) {	// SDA falls while SCL high: repeated start Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Repeated Start");
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
		} else if (event.IsPinLow(prop_.pinSCL)) {
			stat_ = Stat::BitAccum_SCL_Rise;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	case Stat::BitAccum_SCL_Rise: {
		if (event.IsPinHigh(prop_.pinSCL)) {
			int iBit = nBitsAccum_;
			uint bitValue = event.IsPinHigh(prop_.pinSDA)? 1 : 0;
			if (iBit == 7 && field_ == Field::Address) {
				iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " %s", bitValue? "Read" : "Write");
			} else if (iBit == 8) {
				iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " %s", bitValue? "Nack" : "Ack");
			} else {
				iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " %d", bitValue);
			}
			bitAccum_ = (bitAccum_ << 1) | bitValue;
			nBitsAccum_++;
			if (nBitsAccum_ == 9) {
				nBitsAccum_ = 0;
				bitAccum_ = 0x000;
				field_ = Field::Data;
			}
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	default:break;
	}
#endif
}

//------------------------------------------------------------------------------
// ProtocolAnalyzer_I2C::Core
//------------------------------------------------------------------------------
void ProtocolAnalyzer_I2C::Core::ProcessEvent(const Event& event)
{
	switch (stat_) {
	case Stat::WaitForStable: {
		if (event.IsPinHigh(prop_.pinSCL, prop_.pinSDA)) {
			stat_ = Stat::Start_SDA_Fall;
		}
		break;
	}
	case Stat::Start_SDA_Fall: {
		if (event.IsPinLow(prop_.pinSDA)) {		// SDA falls while SCL high: Start Condition
			OnStart();
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	case Stat::BitAccum_SCL_Fall: {
		if (!signalSDAPrev_ && event.IsPinHigh(prop_.pinSDA)) {		// SDA rises while SCL high: Stop Condition
			OnStop();
			stat_ = Stat::Start_SDA_Fall;
		} else if (signalSDAPrev_ && event.IsPinLow(prop_.pinSDA)) {	// SDA falls while SCL high: repeated start Condition
			OnRepeatedStart();
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
		} else if (event.IsPinLow(prop_.pinSCL)) {
			stat_ = Stat::BitAccum_SCL_Rise;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	case Stat::BitAccum_SCL_Rise: {
		if (event.IsPinHigh(prop_.pinSCL)) {
			int iBit = nBitsAccum_;
			uint bitValue = event.IsPinHigh(prop_.pinSDA)? 1 : 0;
			OnBit(field_, iBit, bitValue);
			bitAccum_ = (bitAccum_ << 1) | bitValue;
			nBitsAccum_++;
			if (nBitsAccum_ == 9) {
				OnByte(field_, static_cast<uint8_t>(bitAccum_ >> 1), bitAccum_ & 0x01);
				nBitsAccum_ = 0;
				bitAccum_ = 0x000;
				field_ = Field::Data;
			}
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		break;
	}
	default:break;
	}
}

//------------------------------------------------------------------------------
// ProtocolAnalyzer_I2C::Core
//------------------------------------------------------------------------------
void ProtocolAnalyzer_I2C::Core_Annotator::OnStart()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Start");
}

void ProtocolAnalyzer_I2C::Core_Annotator::OnStop()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Stop");
}

void ProtocolAnalyzer_I2C::Core_Annotator::OnRepeatedStart()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Repeated Start");
}

void ProtocolAnalyzer_I2C::Core_Annotator::OnBit(Field field, int iBit, bool bitValue)
{
	int& iCol = *piCol_;
	if (iBit == 7 && field == Field::Address) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %s", bitValue? "Read" : "Write");
	} else if (iBit == 8) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %s", bitValue? "Nack" : "Ack");
	} else {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %d", bitValue);
	}
}

void ProtocolAnalyzer_I2C::Core_Annotator::OnByte(Field field, uint8_t byte, bool bitAck)
{
}

}
