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
		int nPins;
		std::unique_ptr<uint[]> pinTbl;
		int nEvents;
		PrintPart part;
		const WaveStyle* pWaveStyle;
	};
public:
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
	PIO::StateMachine smTbl_[4];
	DMA::ChannelConfig configTbl_[4];
	DMA::Channel* pChannelTbl_[4];
	std::unique_ptr<Event> eventBuffTbl_[4];
	int nEventsMax_;
	bool enabledFlag_;
	uint32_t pinBitmap_;
	uint pinMin_;
	int nClocksPerLoop_;
	float usecReso_;
	PrintInfo printInfo_;
public:
	LogicAnalyzer(int nEventsMax = 8192);
	~LogicAnalyzer();
public:
	LogicAnalyzer& Enable();
	LogicAnalyzer& Disable();
	LogicAnalyzer& SetPins(const int pinTbl[], int nPins);
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEvents) { printInfo_.nEvents = nEvents; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	float GetResolution() const { return usecReso_; }
	int GetEventCount() const;
	const Event& GetEvent(int iEvent) const;
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
};

}

#endif
