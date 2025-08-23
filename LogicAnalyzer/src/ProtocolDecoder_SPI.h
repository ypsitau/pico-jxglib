//==============================================================================
// ProtocolDecoder_SPI.h
//==============================================================================
#pragma once
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ProtocolDecoder_SPI
//------------------------------------------------------------------------------
class ProtocolDecoder_SPI : public LogicAnalyzer::Decoder {
public:
	class Factory : public LogicAnalyzer::Decoder::Factory {
	public:
		Factory() : LogicAnalyzer::Decoder::Factory("spi") {}
	public:
		virtual LogicAnalyzer::Decoder* Create(const LogicAnalyzer& logicAnalyzer) override {
			return new ProtocolDecoder_SPI(logicAnalyzer, name_);
		}
	};
public:
	   // SPI protocol state machine states
		enum class Stat {
			Done,				// Analysis complete
			WaitForIdle,		// Waiting for SCK to go high (mode = 2 or 3) or low (mode = 0 or 1)
			BitAccum_SCK,		// Accumulating bits on SCK edge
		};
		// SPI pin configuration
		struct Property {
			int mode;
			uint pinMOSI, pinMISO, pinSCK;
		};
		// Core SPI analyzer logic
		class Core {
		protected:
		   Stat stat_;				// Current state of the SPI state machine
		   int nBitsAccum_;			// Number of bits accumulated in current byte
		   uint16_t bitAccumMOSI_;	// Accumulated bits (MOSI)
		   uint16_t bitAccumMISO_;	// Accumulated bits (MISO)
		   bool signalSCKPrev_;		// Previous SCK pin state
		   const Property& prop_;	// SPI pin configuration
	public:
		Core(const Core& core);
		Core(const Property& prop);
	public:
		Stat GetStat() const { return stat_; }
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event);
	public:
		virtual void OnBitAccumBegin(const EventIterator& eventIter) {}
		virtual void OnBit(int iBit, const Event& event) {}
		virtual void OnBitAccumComplete() {}
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
		virtual void OnBitAccumBegin(const EventIterator& eventIter) override;
		virtual void OnBit(int iBit, const Event& event) override;
		virtual void OnBitAccumComplete() override;
	};
	class Core_BitAccumAdv : public Core {
	private:
		bool completeFlag_;
		struct {
			uint16_t bitAccumMOSI;
			uint16_t bitAccumMISO;
		} saved_;
	public:
		Core_BitAccumAdv(const Core& core) : Core(core), completeFlag_{false}, saved_{0, 0} {}
	public:
		bool IsComplete() const { return completeFlag_; }
		uint16_t GetBitAccumMOSI() const { return saved_.bitAccumMOSI; }
		uint16_t GetBitAccumMISO() const { return saved_.bitAccumMISO; }
	public:
		virtual void OnBitAccumComplete() override {
			completeFlag_ = true; saved_.bitAccumMOSI = bitAccumMOSI_; saved_.bitAccumMISO = bitAccumMISO_;
		}
	};
private:
	Core_Annotator annotator_;
private:
	Property prop_;
private:
	static Factory factory_;
public:
	ProtocolDecoder_SPI(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd);
	virtual bool CheckValidity(Printable& terr);
	virtual void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol) override;
	virtual void AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol) override;
};

}
