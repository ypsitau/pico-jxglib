//==============================================================================
// ProtocolAnalyzer_I2C.h
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
	struct Property {
		uint pinSDA, pinSCL;
	};
	class Core {
	private:
		Stat stat_;
		Field field_;
		int nBitsAccum_;
		uint16_t bitAccum_;
		bool signalSDAPrev_;
		const Property& prop_;
	public:
		Core(const Property& prop) : prop_{prop}, stat_{Stat::WaitForStable}, field_{Field::Address}, nBitsAccum_{0}, bitAccum_{0}, signalSDAPrev_{false} {}
	public:
		void ProcessEvent(const Event& event);
	public:
		virtual void OnStart() {}
		virtual void OnStop() {}
		virtual void OnRepeatedStart() {}
		virtual void OnBit(Field field, int iBit, bool bitValue) {}
		virtual void OnByte(Field field, uint8_t byte, bool bitAck) {}
	};
	class Core_Annotator : public Core {
	private:
		char* buffLine_;
		int lenBuffLine_;
		int* piCol_;
	public:
		Core_Annotator(const Property& prop) : Core(prop), buffLine_{nullptr}, lenBuffLine_{0}, piCol_{nullptr} {}
	public:
		void ProcessEvent(const Event& event, char* buffLine, int lenBuffLine, int* piCol) {
			buffLine_ = buffLine;
			lenBuffLine_ = lenBuffLine;
			piCol_ = piCol;
			Core::ProcessEvent(event);
		}
	public:
		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual void OnRepeatedStart() override;
		virtual void OnBit(Field field, int iBit, bool bitValue) override;
		virtual void OnByte(Field field, uint8_t byte, bool bitAck) override;
	};
private:
	Stat stat_;
	Field field_;
	int nBitsAccum_;
	uint16_t bitAccum_;
	bool signalSDAPrev_;
	Core_Annotator annotator_;
private:
	Property prop_;
private:
	static Factory factory_;
public:
	ProtocolAnalyzer_I2C(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd);
	virtual bool FinishSubcmd(Printable& terr);
	virtual void AnnotateWave(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol) override;
};

}
