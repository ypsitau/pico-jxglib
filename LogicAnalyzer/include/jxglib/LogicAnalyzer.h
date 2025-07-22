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
	struct WaveStyle {
		const char* name;
		const char* strHigh;
		const char* strHighIdle;
		const char* strLow;
		const char* strLowIdle;
		const char* strLowToHigh;
		const char* strHighToLow;
		const char* formatHeader;
	};
	enum class PrintPart { Head, Tail, All };
	struct PrintInfo {
		int nEvents;
		PrintPart part;
		const WaveStyle* pWaveStyle;
	};
public:
	static const uint nEventsMax = 2048;
	static const WaveStyle waveStyle_fancy1;
	static const WaveStyle waveStyle_fancy2;
	static const WaveStyle waveStyle_fancy3;
	static const WaveStyle waveStyle_fancy4;
	static const WaveStyle waveStyle_simple1;
	static const WaveStyle waveStyle_simple2;
	static const WaveStyle waveStyle_simple3;
	static const WaveStyle waveStyle_simple4;
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
	DMA::ChannelConfig config_;
	DMA::Channel* pChannel_;
	std::unique_ptr<Event> eventBuff_;
	bool enabledFlag_;
	uint32_t pinBitmap_;
	uint pinMin_;
	float usecReso_;
	int nClocksPerLoop_;
	PrintInfo printInfo_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& Enable();
	LogicAnalyzer& Disable();
	LogicAnalyzer& SetPins(uint32_t pinBitmap, uint32_t pinMin) { pinBitmap_ = pinBitmap; pinMin_ = pinMin; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEvents) { printInfo_.nEvents = nEvents; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	float GetResolution() const { return usecReso_; }
	int GetEventCount() const;
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
	const Event& GetEvent(int iEvent) const { return eventBuff_.get()[iEvent]; }
};

}

#endif
