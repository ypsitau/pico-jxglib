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
	   // SPI protocol state machine states
	   enum class Stat {
		   Done,              // Analysis complete
		   WaitForStable,     // Waiting for CS to go low (start of transaction)
		   Start_CS_Fall,     // Detected CS falling edge, waiting for SCK
		   BitAccum_SCK_Edge, // Accumulating bits on SCK edge
		   BitAccum_SCK_Idle  // Idle between SCK edges
	   };
	   // SPI data field (expandable for future use)
	   enum class Field { Data };
	   // SPI pin configuration
	   struct Property {
		   uint pinMOSI, pinMISO, pinSCK, pinCS;
	   };
	   // Core SPI analyzer logic
	   class Core {
	   private:
		   Stat stat_;            // Current state of the SPI state machine
		   Field field_;          // Current field (data, etc.)
		   int nBitsAccum_;       // Number of bits accumulated in current byte
		   uint16_t bitAccum_;    // Accumulated bits (MOSI)
		   bool csPrev_;          // Previous CS pin state
		   bool sckPrev_;         // Previous SCK pin state
		   const Property& prop_; // SPI pin configuration
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
