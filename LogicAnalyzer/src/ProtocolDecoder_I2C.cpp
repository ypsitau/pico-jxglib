//==============================================================================
// ProtocolDecoder_I2C.cpp
//==============================================================================
#include "ProtocolDecoder_I2C.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolDecoder_I2C
//------------------------------------------------------------------------------
ProtocolDecoder_I2C::Factory ProtocolDecoder_I2C::factory_;

ProtocolDecoder_I2C::ProtocolDecoder_I2C(const LogicAnalyzer& logicAnalyzer, const char* name) :
	Decoder(logicAnalyzer, name), annotator_(prop_), prop_{GPIO::InvalidPin, GPIO::InvalidPin}
{}

bool ProtocolDecoder_I2C::EvalSubcmd(Printable& terr, const char* subcmd)
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
	terr.Printf("unknown sub-command: %s\n", subcmd);
	return false;
}

bool ProtocolDecoder_I2C::CheckValidity(Printable& terr)
{
	bool rtn = true;
	if (prop_.pinSDA == GPIO::InvalidPin) {
		terr.Printf("specify SDA pin number\n");
		rtn = false;
	}
	if (prop_.pinSCL == GPIO::InvalidPin) {
		terr.Printf("specify SCL pin number\n");
		rtn = false;
	}
	return rtn;
}

void ProtocolDecoder_I2C::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

void ProtocolDecoder_I2C::AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol)
{
	//annotator_.ProcessEvent(eventIter, event, buffLine, lenBuffLine, piCol);
}

//------------------------------------------------------------------------------
// ProtocolDecoder_I2C::Core
//------------------------------------------------------------------------------
ProtocolDecoder_I2C::Core::Core(const Core& core) : prop_{core.prop_}, stat_{core.stat_}, field_{core.field_},
	nBitsAccum_{core.nBitsAccum_}, bitAccum_{core.bitAccum_}, signalSDAPrev_{core.signalSDAPrev_}
{}

ProtocolDecoder_I2C::Core::Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForStable}, field_{Field::Address},
	nBitsAccum_{0}, bitAccum_{0}, signalSDAPrev_{false}
{}

void ProtocolDecoder_I2C::Core::ProcessEvent(const EventIterator& eventIter, const Event& event)
{
	switch (stat_) {
	case Stat::WaitForStable: {
		if (event.IsPinHigh(prop_.pinSCL, prop_.pinSDA)) {
			stat_ = Stat::Start_SDA_Fall;
		}
		break;
	}
	case Stat::Start_SDA_Fall: {
		bool bitAccumBeginFlag = false;
		if (event.IsPinLow(prop_.pinSDA)) {		// SDA falls while SCL high: Start Condition
			OnStart();
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
			stat_ = Stat::BitAccum_SCL_Fall;
			bitAccumBeginFlag = true;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		if (bitAccumBeginFlag) OnBitAccumBegin(eventIter);
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
		bool bitAccumBeginFlag = false;
		if (event.IsPinHigh(prop_.pinSCL)) {
			int iBit = nBitsAccum_;
			uint bitValue = event.IsPinHigh(prop_.pinSDA)? 1 : 0;
			OnBit(field_, iBit, bitValue);
			bitAccum_ = (bitAccum_ << 1) | bitValue;
			nBitsAccum_++;
			if (nBitsAccum_ == 9) {
				OnBitAccumComplete(field_, bitAccum_);
				nBitsAccum_ = 0;
				bitAccum_ = 0x000;
				field_ = Field::Data;
				bitAccumBeginFlag = true;
			}
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(prop_.pinSDA);
		if (bitAccumBeginFlag) OnBitAccumBegin(eventIter);
		break;
	}
	default:break;
	}
}

//------------------------------------------------------------------------------
// ProtocolDecoder_I2C::Core_Annotator
//------------------------------------------------------------------------------
ProtocolDecoder_I2C::Core_Annotator::Core_Annotator(const Property& prop) :
		Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr}, adv_{false, 0}
{}

void ProtocolDecoder_I2C::Core_Annotator::ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol)
{
	buffLine_ = buffLine;
	lenBuffLine_ = lenBuffLine;
	piCol_ = piCol;
	Core::ProcessEvent(eventIter, event);
}

void ProtocolDecoder_I2C::Core_Annotator::OnStart()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Start");
}

void ProtocolDecoder_I2C::Core_Annotator::OnBitAccumBegin(const EventIterator& eventIter)
{
	Core_BitAccumAdv coreAdv(*this);
	EventIterator eventIterAdv(eventIter);
	Event eventAdv;
	while (eventIterAdv.Next(eventAdv)) {
		coreAdv.ProcessEvent(eventIterAdv, eventAdv);
		if (coreAdv.IsComplete()) break;
	}
	adv_.validFlag = coreAdv.IsComplete();
	adv_.bitAccum = coreAdv.GetBitAccumAdv();
}

void ProtocolDecoder_I2C::Core_Annotator::OnStop()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Stop");
}

void ProtocolDecoder_I2C::Core_Annotator::OnRepeatedStart()
{
	int& iCol = *piCol_;
	iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Repeated Start");
}

void ProtocolDecoder_I2C::Core_Annotator::OnBit(Field field, int iBit, bool bitValue)
{
	int& iCol = *piCol_;
	if (iBit == 7 && field == Field::Address) {
		//iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %s", bitValue? "Read" : "Write");
	} else if (iBit == 8) {
		iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %s", bitValue? "Nack" : "Ack");
	} else if (iBit == 3 && adv_.validFlag) {
		if (field == Field::Address) {
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Addr:0x%02X:%s", adv_.bitAccum >> 2,
					(adv_.bitAccum & (1 << 1))? "Read" : "Write");
		} else { // field == Field::Data
			iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " Data:0x%02X", adv_.bitAccum >> 1);
		}
	} else {
		//iCol += ::snprintf(buffLine_ + iCol, lenBuffLine_ - iCol, " %d", bitValue);
	}
}

void ProtocolDecoder_I2C::Core_Annotator::OnBitAccumComplete(Field field, uint16_t bitAccum)
{
	adv_.validFlag = false;
}

}
