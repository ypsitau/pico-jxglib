//==============================================================================
// Decoder_UART.h
//==============================================================================
#pragma once
#include "jxglib/LogicAnalyzer.h"

namespace jxglib::LogicAnalyzerExt {

//------------------------------------------------------------------------------
// Decoder_UART
//------------------------------------------------------------------------------
class Decoder_UART : public LogicAnalyzer::Decoder {
public:
	class Factory : public LogicAnalyzer::Decoder::Factory {
	public:
		Factory() : LogicAnalyzer::Decoder::Factory("uart") {}
	public:
		virtual LogicAnalyzer::Decoder* Create(const LogicAnalyzer& logicAnalyzer) override {
			return new Decoder_UART(logicAnalyzer, name_);
		}
	};
public:
	enum class Stat {
		Done, WaitForIdle, StartBit, BitAccum, StopBit
	};
	enum class Field { Data };
	struct Property {
		uint pinRX;
		int baudrate;
		int dataBits;
		bool parityEnable;
		bool parityOdd;
		int stopBits;
	};
	class Core {
	private:
		Stat stat_;                // Current state of the UART state machine
		Field field_;              // Current field (data, parity, etc.)
		int nBitsAccum_;           // Number of bits accumulated
		uint16_t bitAccum_;        // Accumulated bits (data)
		int bitIdx_;               // Current bit index
		const Property& prop_;     // UART configuration properties
		bool lastRX_;              // Last RX pin state
		int sampleCounter_;        // Sample counter (not always used)
		// UART sampling timing variables
		uint64_t lastStartTime_ = 0;      // Timestamp of the last detected start bit
		uint64_t bitSampleTime_ = 0;      // Timestamp for the next bit sample
		int bitsToSample_ = 0;            // Number of bits to sample in the current frame
		uint64_t bitPeriod_ = 0;          // Bit period in microseconds
		uint64_t stopBitSampleTime_ = 0;  // Timestamp to sample the stop bit
	public:
		Core(const Core& core);
		Core(const Property& prop);
	public:
		Stat GetStat() const { return stat_; }
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event);
	public:
		virtual void OnStartBit() {}
		virtual void OnBit(Field field, int iBit, bool bitValue) {}
		virtual void OnStopBit(bool valid) {}
		virtual void OnByte(uint8_t data, bool parityErr) {}
	};
	class Core_Annotator : public Core {
	private:
		char* buffLine_;
		int lenBuffLine_;
		int* piCol_;
	public:
		Core_Annotator(const Property& prop);
	public:
		void ProcessEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol);
	public:
		virtual void OnStartBit() override;
		virtual void OnBit(Field field, int iBit, bool bitValue) override;
		virtual void OnStopBit(bool valid) override;
		virtual void OnByte(uint8_t data, bool parityErr) override;
	};
private:
	Core_Annotator annotator_;
private:
	Property prop_;
private:
	static Factory factory_;
public:
	Decoder_UART(const LogicAnalyzer& logicAnalyzer, const char* name);
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd) override;
	virtual bool CheckValidity(Printable& terr) override;
	virtual void Reset() override;
	virtual void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int* piCol) override;
	virtual void AnnotateWaveStreak(char* buffLine, int lenBuffLine, int* piCol) override;
};

}
