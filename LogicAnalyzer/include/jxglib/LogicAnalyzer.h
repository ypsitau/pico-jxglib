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
	struct RawEvent {
		uint32_t timeStamp;
		uint32_t pinBitmap;;
	};
	struct Event{
		uint64_t timeStamp;
		uint32_t pinBitmap;
	};
	enum class Target { Internal, External };
	struct WaveStyle {
		const char* name;
		const char* strBlank;
		const char* strHigh;
		const char* strHighIdle;
		const char* strLow;
		const char* strLowIdle;
		const char* strLowToHigh;
		const char* strHighToLow;
		const char* formatHeader;
	};
	enum class PrintPart { Head, Tail, All };
	struct Processor {
		PIO::StateMachine sm;
		DMA::ChannelConfig config;
		DMA::Channel* pChannel;
		std::unique_ptr<RawEvent> rawEventBuff;
	public:
		Processor() : pChannel{nullptr} {}
		~Processor() { if (pChannel) { pChannel->unclaim(); } }
		int GetRawEventCount() const {
			return (reinterpret_cast<uint32_t>(pChannel->get_write_addr()) -
					reinterpret_cast<uint32_t>(rawEventBuff.get())) / sizeof(RawEvent);
		}
		const RawEvent& GetRawEvent(int iRawEvent) const { return rawEventBuff.get()[iRawEvent]; }
	};
	struct SamplingInfo {
		bool enabledFlag;
		uint32_t pinBitmap;
		uint pinMin;
	public:
		SamplingInfo() : enabledFlag{false}, pinBitmap{0}, pinMin{0} {}
	};
	struct PrintInfo {
		int nPins;
		std::unique_ptr<uint[]> pinTbl;
		int nEventsToPrint;
		PrintPart part;
		const WaveStyle* pWaveStyle;
	public:
		PrintInfo() : nPins{0}, nEventsToPrint{80}, part{PrintPart::Head}, pWaveStyle{&waveStyle_fancy2} {}
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
	Processor processorTbl_[4];
	SamplingInfo samplingInfo_;
	PrintInfo printInfo_;
	Target target_;
	int nRawEventMax_;
	int nClocksPerLoop_;
	float usecReso_;
public:
	LogicAnalyzer(int nRawEventMax = 8192);
	~LogicAnalyzer();
public:
	LogicAnalyzer& UpdateSamplingInfo();
	bool Enable();
	LogicAnalyzer& Disable();
	LogicAnalyzer& SetPins(const int pinTbl[], int nPins);
	LogicAnalyzer& SetTarget(Target target) { target_ = target; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { printInfo_.nEventsToPrint = nEventsToPrint; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	float GetResolution() const { return usecReso_; }
	bool HasSamplingPins() const { return samplingInfo_.pinBitmap != 0; }
	int GetEventCount() const;
	const RawEvent& GetRawEvent(int iProcessor, int iRawEvent) const;
	bool IsPinAsserted(uint32_t pinBitmap, uint pin) const { return ((pinBitmap << samplingInfo_.pinMin) & (1 << pin)) != 0; }
	bool IsPinEnabled(uint pin) const { return IsPinAsserted(samplingInfo_.pinBitmap, pin); }
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
};

}

#endif
