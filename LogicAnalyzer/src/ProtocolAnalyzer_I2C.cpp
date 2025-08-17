//==============================================================================
// ProtocolAnalyzer_I2C.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolAnalyzer_I2C
//------------------------------------------------------------------------------
class ProtocolAnalyzer_I2C : public ProtocolAnalyzer {
public:
	class Factory : public ProtocolAnalyzer::Factory {
	public:
		Factory() : ProtocolAnalyzer::Factory("I2C") {}
	public:
		virtual ProtocolAnalyzer* Create(const LogicAnalyzer& logicAnalyzer) override {
			return new ProtocolAnalyzer_I2C(logicAnalyzer, name_);
		}
	};
public:
	enum class Stat {
		Done, WaitForStable, Start_SDA_Fall, BitAccum_SCL_Fall, BitAccum_SCL_Rise, Stop_SCL_Fall,
	};
	enum class Field { Address, Data };
private:
	Stat stat_;
	Field field_;
	int nBitsAccum_;
	uint16_t bitAccum_;
	bool signalSDAPrev_;
private:
	uint pinSDA_, pinSCL_;
private:
	static Factory factory_;
public:
	ProtocolAnalyzer_I2C(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd);
	virtual bool FinishSubcmd(Printable& terr);
	virtual void OnPrintWave(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol) override;
};

ProtocolAnalyzer_I2C::Factory ProtocolAnalyzer_I2C::factory_;

ProtocolAnalyzer_I2C::ProtocolAnalyzer_I2C(const LogicAnalyzer& logicAnalyzer, const char* name) :
	ProtocolAnalyzer(logicAnalyzer, name), stat_{Stat::WaitForStable}, field_{Field::Address},
	nBitsAccum_{0}, bitAccum_{0}, signalSDAPrev_{false}, pinSDA_{GPIO::InvalidPin}, pinSCL_{GPIO::InvalidPin}
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
		pinSDA_ = static_cast<uint>(num);
		return true;
	} else if (Shell::Arg::GetAssigned(subcmd, "scl", &value)) {
		int num = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid SCL pin number\n");
			return false;
		}
		pinSCL_ = static_cast<uint>(num);
		return true;
	}
	terr.Printf("unknown parameter: %s\n", subcmd);
	return false;
}

bool ProtocolAnalyzer_I2C::FinishSubcmd(Printable& terr)
{
	if (pinSDA_ == GPIO::InvalidPin) {
		terr.Printf("specify SDA pin number\n");
		return false;
	}
	if (pinSCL_ == GPIO::InvalidPin) {
		terr.Printf("specify SCL pin number\n");
		return false;
	}
	return true;
}

void ProtocolAnalyzer_I2C::OnPrintWave(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	int& iCol = *piCol;
	switch (stat_) {
	case Stat::WaitForStable: {
		if (event.IsPinHigh(pinSCL_, pinSDA_)) {
			stat_ = Stat::Start_SDA_Fall;
		}
		break;
	}
	case Stat::Start_SDA_Fall: {
		if (event.IsPinLow(pinSDA_)) {		// SDA falls while SCL high: Start Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Start");
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
			stat_ = Stat::BitAccum_SCL_Fall;
		}
		signalSDAPrev_ = event.IsPinHigh(pinSDA_);
		break;
	}
	case Stat::BitAccum_SCL_Fall: {
		if (!signalSDAPrev_ && event.IsPinHigh(pinSDA_)) {		// SDA rises while SCL high: Stop Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Stop");
			stat_ = Stat::Start_SDA_Fall;
		} else if (signalSDAPrev_ && event.IsPinLow(pinSDA_)) {	// SDA falls while SCL high: repeated start Condition
			iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, " Repeated Start");
			nBitsAccum_ = 0;
			bitAccum_ = 0x000;
			field_ = Field::Address;
		} else if (event.IsPinLow(pinSCL_)) {
			stat_ = Stat::BitAccum_SCL_Rise;
		}
		signalSDAPrev_ = event.IsPinHigh(pinSDA_);
		do {
			EventIterator eventIterAdv(eventIter);
			Stat statAdv = stat_;
			int nBitsAccumAdv = nBitsAccum_;
			uint16_t bitAccumAdv = bitAccum_;
			bool signalSDAPrevAdv = signalSDAPrev_;
			Event eventAdv;
			while (statAdv != Stat::Done && eventIterAdv.Next(eventAdv)) {
				switch (statAdv) {
				case Stat::BitAccum_SCL_Fall: {
					if (!signalSDAPrevAdv && eventAdv.IsPinHigh(pinSDA_)) {			// SDA rises while SCL high: Stop Condition
						statAdv = Stat::Done;
					} else if (signalSDAPrevAdv && eventAdv.IsPinLow(pinSDA_)) {	// SDA falls while SCL high: repeated start Condition
						statAdv = Stat::Done;
					} else if (eventAdv.IsPinLow(pinSCL_)) {
						statAdv = Stat::BitAccum_SCL_Rise;
					}
					signalSDAPrevAdv = eventAdv.IsPinHigh(pinSDA_);
					break;
				}
				case Stat::BitAccum_SCL_Rise: {
					if (eventAdv.IsPinHigh(pinSCL_)) {
						uint bitValue = eventAdv.IsPinHigh(pinSDA_)? 1 : 0;
						bitAccumAdv = (bitAccumAdv << 1) | bitValue;
						nBitsAccumAdv++;
						statAdv = (nBitsAccumAdv == 9)? Stat::Done : Stat::BitAccum_SCL_Fall;
					}
					signalSDAPrevAdv = eventAdv.IsPinHigh(pinSDA_);
					break;
				}
				default: break;
				}

			}
		} while (0);
		break;
	}
	case Stat::BitAccum_SCL_Rise: {
		if (event.IsPinHigh(pinSCL_)) {
			int iBit = nBitsAccum_;
			uint bitValue = event.IsPinHigh(pinSDA_)? 1 : 0;
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
		signalSDAPrev_ = event.IsPinHigh(pinSDA_);
		break;
	}
	default:break;
	}
}

}
