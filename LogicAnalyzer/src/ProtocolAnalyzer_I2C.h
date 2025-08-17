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
		Factory() : ProtocolAnalyzer::Factory("i2c") {}
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
		Core(const Core& core);
		Core(const Property& prop);
	public:
		Stat GetStat() const { return stat_; }
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event);
	public:
		virtual void OnStart() {}
		virtual void OnBitAccumBegin(const EventIterator& eventIter) {}
		virtual void OnStop() {}
		virtual void OnRepeatedStart() {}
		virtual void OnBit(Field field, int iBit, bool bitValue) {}
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccum) {}
	};
	class Core_Annotator : public Core {
	private:
		char* buffLine_;
		int lenBuffLine_;
		int* piCol_;
		struct {
			bool validFlag;
			uint16_t bitAccum;
		} adv_;
	public:
		Core_Annotator(const Property& prop);
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol);
	public:
		virtual void OnStart() override;
		virtual void OnBitAccumBegin(const EventIterator& eventIter) override;
		virtual void OnStop() override;
		virtual void OnRepeatedStart() override;
		virtual void OnBit(Field field, int iBit, bool bitValue) override;
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccum) override;
	};
	class Core_BitAccumAdv : public Core {
	private:
		bool completeFlag_;
		uint16_t bitAccumAdv_;
	public:
		Core_BitAccumAdv(const Core& core) : Core(core), completeFlag_{false}, bitAccumAdv_{0} {}
	public:
		bool IsComplete() const { return completeFlag_; }
		uint16_t GetBitAccumAdv() const { return bitAccumAdv_; }
	public:
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccum) override { completeFlag_ = true; bitAccumAdv_ = bitAccum;}
	};
private:
	Core_Annotator annotator_;
private:
	Property prop_;
private:
	static Factory factory_;
public:
	ProtocolAnalyzer_I2C(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd);
	virtual bool CheckValidity(Printable& terr);
	virtual void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol) override;
	virtual void AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol) override;
};

}
