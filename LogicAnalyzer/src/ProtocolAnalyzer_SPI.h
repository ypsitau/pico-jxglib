//==============================================================================
// ProtocolAnalyzer_SPI.h
//==============================================================================
#pragma once
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolAnalyzer_SPI
//------------------------------------------------------------------------------
class ProtocolAnalyzer_SPI : public ProtocolAnalyzer {
public:
	class Factory : public ProtocolAnalyzer::Factory {
	public:
		Factory() : ProtocolAnalyzer::Factory("spi") {}
	public:
		virtual ProtocolAnalyzer* Create(const LogicAnalyzer& logicAnalyzer) override {
			return new ProtocolAnalyzer_SPI(logicAnalyzer, name_);
		}
	};
public:
	enum class Stat {
		Done, WaitForStable, Start_CS_Fall, BitAccum_SCK_Edge, BitAccum_SCK_Idle
	};
	enum class Field { Data };
	struct Property {
		uint pinMOSI, pinMISO, pinSCK, pinCS;
	};
	class Core {
	private:
		Stat stat_;
		Field field_;
		int nBitsAccum_;
		uint16_t bitAccum_;
		bool csPrev_;
		bool sckPrev_;
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
		virtual void OnBit(Field field, int iBit, bool bitMOSI, bool bitMISO) {}
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccumMOSI, uint16_t bitAccumMISO) {}
	};
	class Core_Annotator : public Core {
	private:
		char* buffLine_;
		int lenBuffLine_;
		int* piCol_;
		struct {
			bool validFlag;
			uint16_t bitAccumMOSI;
			uint16_t bitAccumMISO;
		} adv_;
	public:
		Core_Annotator(const Property& prop);
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol);
	public:
		virtual void OnStart() override;
		virtual void OnBitAccumBegin(const EventIterator& eventIter) override;
		virtual void OnStop() override;
		virtual void OnBit(Field field, int iBit, bool bitMOSI, bool bitMISO) override;
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccumMOSI, uint16_t bitAccumMISO) override;
	};
	class Core_BitAccumAdv : public Core {
	private:
		bool completeFlag_;
		uint16_t bitAccumMOSI_;
		uint16_t bitAccumMISO_;
	public:
		Core_BitAccumAdv(const Core& core) : Core(core), completeFlag_{false}, bitAccumMOSI_{0}, bitAccumMISO_{0} {}
	public:
		bool IsComplete() const { return completeFlag_; }
		uint16_t GetBitAccumMOSI() const { return bitAccumMOSI_; }
		uint16_t GetBitAccumMISO() const { return bitAccumMISO_; }
	public:
		virtual void OnBitAccumComplete(Field field, uint16_t bitAccumMOSI, uint16_t bitAccumMISO) override { completeFlag_ = true; bitAccumMOSI_ = bitAccumMOSI; bitAccumMISO_ = bitAccumMISO; }
	};
private:
	Core_Annotator annotator_;
private:
	Property prop_;
private:
	static Factory factory_;
public:
	ProtocolAnalyzer_SPI(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd);
	virtual bool CheckValidity(Printable& terr);
	virtual void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol) override;
	virtual void AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol) override;
};

}
