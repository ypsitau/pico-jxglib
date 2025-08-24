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
		WaitForIdle, DataAccum,
	};
	enum class Parity { None, Even, Odd };
	struct Property {
		int baudrate;
		int dataBits;
		Parity parity;
		int stopBits;
		double timeStampFactor;
	};
	class Core {
	private:
		const Property& prop_;
		uint pin_;
		Stat stat_;
		bool signalPrev_;
		double timeStartNext_;
	public:
		Core(const Core& core);
		Core(const Property& prop);
	public:
		void SetPin(uint pin) { pin_ = pin; }
		uint GetPin() const { return pin_; }
		Stat GetStat() const { return stat_; }
	public:
		void Reset();
		void ProcessEvent(const EventIterator& eventIter, const Event& event);
	public:
		virtual void OnStartBit(uint8_t data, uint8_t parity) {}
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
		virtual void OnStartBit(uint8_t data, uint8_t parity) override;
	};
private:
	Core_Annotator annotatorTX_;
	Core_Annotator annotatorRX_;
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
