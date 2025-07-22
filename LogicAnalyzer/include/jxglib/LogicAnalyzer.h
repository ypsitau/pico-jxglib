//==============================================================================
// jxglib/LogicAnalyzer.h
//==============================================================================
#ifndef PICO_JXGLIB_LOGICANALYZER_H
#define PICO_JXGLIB_LOGICANALYZER_H
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
class LogicAnalyzer {
public:
	struct Event {
		uint32_t timeStamp;
		uint32_t bits;
	};
	struct WavePattern {
		const char* strHigh;
		const char* strHighIdle;
		const char* strLow;
		const char* strLowIdle;
		const char* strLowToHigh;
		const char* strHighToLow;
		const char* formatHeader;
	};
public:
	static const uint nEventsMax = 2048;
	static const WavePattern wavePattern_Fancy;
	static const WavePattern wavePattern_Simple;
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
	DMA::ChannelConfig config_;
	DMA::Channel* pChannel_;
	std::unique_ptr<Event> eventBuff_;
	uint32_t pinBitmap_;
	uint pinMin_;
	float usecReso_;
	int nEventsToPrint_;
	int nClocksPerLoop_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& Start();
	LogicAnalyzer& Restart();
	LogicAnalyzer& Stop();
	LogicAnalyzer& Clear();
	LogicAnalyzer& SetPins(uint32_t pinBitmap, uint32_t pinMin) { pinBitmap_ = pinBitmap; pinMin_ = pinMin; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { nEventsToPrint_ = nEventsToPrint; return *this; }
	float GetResolution() const { return usecReso_; }
	int GetEventCount() const;
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
	const Event& GetEvent(int iEvent) const { return eventBuff_.get()[iEvent]; }
};

}

#endif
