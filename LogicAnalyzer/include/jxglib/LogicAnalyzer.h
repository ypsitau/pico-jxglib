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
		uint32_t value1_;
		uint32_t value2_;
	public:
		uint32_t GetTimeStamp() const { return value1_; }
		uint32_t GetPinBitmap() const { return value2_; }
	};
	class Event{
	private:
		uint64_t timeStamp_;
		uint32_t pinBitmap_;
	public:
		Event(uint64_t timeStamp = 0, uint32_t pinBitmap = 0) : timeStamp_{timeStamp}, pinBitmap_{pinBitmap} {}
	public:
		uint64_t GetTimeStamp() const { return timeStamp_; }
		uint32_t GetPinBitmap() const { return pinBitmap_; }
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
		RawEvent* rawEventBuff_;
		int nRawEventMax_;
	public:
		Sampler();
		~Sampler();
	public:
		void AssignBuff(RawEvent* rawEventBuff, int nRawEventMax) { rawEventBuff_ = rawEventBuff; nRawEventMax_ = nRawEventMax; }
		PIO::StateMachine& GetSM() { return sm_; }
		const PIO::StateMachine& GetSM() const { return sm_; }
		void SetProgram(const PIO::Program& program, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nPinsConsecutive);
		void ShareProgram(Sampler& sampler, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nPinsConsecutive);
		Sampler& EnableDMA();
		Sampler& ReleaseResource();
	public:
		int GetRawEventCount() const;
		int GetRawEventCountMax() const { return nRawEventMax_; }
		bool IsFull() const { return GetRawEventCount() >= nRawEventMax_; }
		const RawEvent& GetRawEvent(int iRawEvent) const { return rawEventBuff_[iRawEvent]; }
		const RawEvent* GetRawEventBuff() const { return rawEventBuff_; }
		void DumpRawEventBuff(Printable& tout) const;
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
	struct PrintInfo {
		int nPins;
		std::unique_ptr<uint[]> pinTbl;
		int nEventsToPrint;
		PrintPart part;
		const WaveStyle* pWaveStyle;
	public:
		PrintInfo() : nPins{0}, nEventsToPrint{80}, part{PrintPart::Head}, pWaveStyle{&waveStyle_fancy2} {}
	};
	class SamplingInfo {
	private:
		bool enabledFlag_;
		uint32_t pinBitmapEnabled_;
		uint pinMin_;
	public:
		SamplingInfo() : enabledFlag_{false}, pinBitmapEnabled_{0}, pinMin_{0} {}
		void SetEnabled(bool enabledFlag) { enabledFlag_ = enabledFlag; }
		bool IsEnabled() const { return enabledFlag_; }
		uint32_t GetPinBitmapEnabled() const { return pinBitmapEnabled_; }
		uint GetPinMin() const { return pinMin_; }	
		bool HasEnabledPins() const { return pinBitmapEnabled_ != 0; }
		bool IsPinAsserted(uint32_t pinBitmap, uint pin) const { return ((pinBitmap << pinMin_) & (1 << pin)) != 0; }
		bool IsPinEnabled(uint pin) const { return IsPinAsserted(pinBitmapEnabled_, pin); }
		void Update(const PrintInfo& printInfo);
		int CountConsecutivePins() const;
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
	PIO::Program program_SamplerInit_;
	PIO::Program program_SamplerMain_;
	RawEvent* rawEventBuffWhole_;
	uint iPIO_;
	int nSampler_;
	Sampler samplerTbl_[4];
	SamplingInfo samplingInfo_;
	PrintInfo printInfo_;
	Target target_;
	float heapRatio_;
	float heapRatioRequested_;
	int clocksPerLoop_;
	float usecReso_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	bool Enable();
	LogicAnalyzer& Disable();
	LogicAnalyzer& SetPIO(uint iPIO) { iPIO_ = iPIO; return *this; }
	LogicAnalyzer& ReleaseResource();
	LogicAnalyzer& SetPins(const int pinTbl[], int nPins);
	LogicAnalyzer& SetSamplerCount(int nSampler);
	int GetSamplerCount() const { return nSampler_; }
	Sampler& GetSampler(int iSampler) { return samplerTbl_[iSampler]; }
	const Sampler& GetSampler(int iSampler) const { return samplerTbl_[iSampler]; }
	LogicAnalyzer& SetHeapRatio(float heapRatio) { heapRatioRequested_ = heapRatio; return *this; }
	LogicAnalyzer& SetTarget(Target target) { target_ = target; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { printInfo_.nEventsToPrint = nEventsToPrint; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	double CalcClockPIOProgram() const { return static_cast<double>(::clock_get_hz(clk_sys) / clocksPerLoop_); }
	float GetResolution() const { return usecReso_; }
	LogicAnalyzer& UpdateSamplingInfo() { samplingInfo_.Update(printInfo_); return *this; }
	bool HasEnabledPins() const { return samplingInfo_.HasEnabledPins(); }
	int GetRawEventCount() const;
	int GetRawEventCountMax() const;
	const RawEvent& GetRawEvent(int iSampler, int iRawEvent) const;
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
public:
	static size_t GetFreeHeapBytes();
};

}

#endif
