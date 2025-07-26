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
	public:
		Event(uint64_t timeStamp = 0, uint32_t pinBitmap = 0) : timeStamp{timeStamp}, pinBitmap{pinBitmap} {}
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
	class Sampler {
	private:
		PIO::StateMachine sm_;
		DMA::Channel* pChannel_;
		DMA::ChannelConfig channelConfig_;
		int nRawEventPerSampler_;
		RawEvent* rawEventBuff_;
	public:
		Sampler();
		~Sampler();
	public:
		bool AllocBuff(int nRawEventPerSampler);
		void FreeBuff();
		PIO::StateMachine& GetSM() { return sm_; }
		const PIO::StateMachine& GetSM() const { return sm_; }
		void SetProgram(const PIO::Program& program, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nPinsConsecutive);
		void ShareProgram(Sampler& sampler, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nPinsConsecutive);
		Sampler& EnableDMA();
		Sampler& Disable();
	public:
		int GetRawEventCount() const;
		bool IsFull() const { return GetRawEventCount() >= nRawEventPerSampler_; }
		const RawEvent& GetRawEvent(int iRawEvent) const { return rawEventBuff_[iRawEvent]; }
		const RawEvent* GetRawEventBuff() const { return rawEventBuff_; }
	};
	class EventIterator {
	private:
		const LogicAnalyzer& logicAnalyzer_;
		int iRawEventTbl_[4];
		const RawEvent* pRawEventPrev_;
		bool doneFlag_;
	public:
		EventIterator(const LogicAnalyzer& logicAnalyzer);
	public:
		bool IsDone() const { return doneFlag_; }
		bool Next(Event& event);
		void Rewind();
		void Skip(int nEvents);
		int Count();
	private:
		const RawEvent* NextRawEvent(int* piSampler = nullptr);
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
	PIO::Program programSamplerInit_;
	PIO::Program programSamplerMain_;
	int nSampler_;
	Sampler samplerTbl_[4];
	SamplingInfo samplingInfo_;
	PrintInfo printInfo_;
	Target target_;
	float heapRatio_;
	int clocksPerLoop_;
	float usecReso_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& UpdateSamplingInfo();
	bool Enable();
	LogicAnalyzer& Disable();
	LogicAnalyzer& SetPins(const int pinTbl[], int nPins);
	LogicAnalyzer& SetSamplerCount(int nSampler);
	int GetSamplerCount() const { return nSampler_; }
	Sampler& GetSampler(int iSampler) { return samplerTbl_[iSampler]; }
	const Sampler& GetSampler(int iSampler) const { return samplerTbl_[iSampler]; }
	LogicAnalyzer& SetHeapRatio(float heapRatio) { heapRatio_ = heapRatio; return *this; };
	LogicAnalyzer& SetTarget(Target target) { target_ = target; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { printInfo_.nEventsToPrint = nEventsToPrint; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	int CalcRawEventMax() const { return static_cast<int>(heapRatio_ * GetFreeHeapBytes() / sizeof(RawEvent)); }
	double CalcClockPIOProgram() const { return static_cast<double>(::clock_get_hz(clk_sys) / clocksPerLoop_); }
	float GetResolution() const { return usecReso_; }
	bool HasSamplingPins() const { return samplingInfo_.pinBitmap != 0; }
	int GetRawEventCount() const;
	const RawEvent& GetRawEvent(int iSampler, int iRawEvent) const;
	bool IsPinAsserted(uint32_t pinBitmap, uint pin) const { return ((pinBitmap << samplingInfo_.pinMin) & (1 << pin)) != 0; }
	bool IsPinEnabled(uint pin) const { return IsPinAsserted(samplingInfo_.pinBitmap, pin); }
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
public:
	static size_t GetFreeHeapBytes();
};

}

#endif
