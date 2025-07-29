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
		uint32_t value_;
	public:
		uint32_t GetTimeStamp(uint nBitsPinBitmap) const { return (~(value_ >> nBitsPinBitmap)) & ((1 << (32 - nBitsPinBitmap)) - 1); }
		uint32_t GetPinBitmap(uint nBitsPinBitmap) const { return value_ & ((1 << nBitsPinBitmap) - 1); }
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
		void SetProgram(const PIO::Program& program, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap);
		void ShareProgram(Sampler& sampler, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap);
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
		int nBitsPinBitmap_;
		int iRawEventTbl_[4];
		uint64_t timeStampOffsetTbl_[4];
		const RawEvent* pRawEventPrev_;
		bool doneFlag_;
		uint64_t timeStampOffsetIncr_;
	public:
		EventIterator(const LogicAnalyzer& logicAnalyzer, int nBitsPinBitmap);
	public:
		bool IsDone() const { return doneFlag_; }
		bool Next(Event& event);
		void Rewind();
		void Skip(int nEvents);
		int Count();
		int CountRelevant();
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
		int CountBits() const;
	};
public:
	class SUMPAdapter : public Tickable {
	public:
		// SUMP Commands
		struct Command {
			static const uint8_t Reset				= 0x00;
			static const uint8_t Run				= 0x01;
			static const uint8_t ID					= 0x02;
			static const uint8_t GetMetadata		= 0x04;
			static const uint8_t SetDivider			= 0x80;
			static const uint8_t SetReadDelayCount	= 0x81;
			static const uint8_t SetFlags			= 0x82;
			static const uint8_t SetTriggerMask		= 0xc0;
			static const uint8_t SetTriggerValues	= 0xc1;
			static const uint8_t SetTriggerConfig	= 0xc2;
		};
		// SUMP Metadata tokens
		struct TokenKey {
			static const uint8_t DeviceName				= 0x01;
			static const uint8_t FirmwareVersion		= 0x02;
			static const uint8_t NumberOfProbes			= 0x20;
			static const uint8_t SampleMemory			= 0x21;
			static const uint8_t DynamicMemory			= 0x22;
			static const uint8_t SampleRate				= 0x23;
			static const uint8_t ProtocolVersion		= 0x24;
			static const uint8_t NumberOfProbes_Short	= 0x40;
			static const uint8_t ProtocolVersion_Short	= 0x41;
			static const uint8_t EndOfMetadata			= 0x00;
		};
		struct Config {
			uint32_t triggerMask	= 0;
			uint32_t triggerValues	= 0;
			uint32_t triggerConfig	= 0;
			uint32_t divider		= 1;
			uint16_t delayCount		= 0;
			uint16_t readCount		= 0;
			uint32_t flags			= 0;
		public:
			uint16_t GetTriggerConfig_Delay() const { return static_cast<uint16_t>(triggerConfig & 0xffff); }
			uint8_t GetTriggerConfig_Level() const { return static_cast<uint8_t>((triggerConfig >> 16) & 0x03); }
			uint8_t GetTriggerConfig_Channel() const { return static_cast<uint8_t>((triggerConfig >> 20) & 0x1f); }
			bool GetTriggerConfig_Serial() const { return (triggerConfig & (1 << 26)) != 0; }
			bool GetTriggerConfig_Start() const { return (triggerConfig & (1 << 27)) != 0; }
		public:
			bool GetFlags_Demux() const { return (flags & (1 << 0)) != 0; }
			bool GetFlags_Filter() const { return (flags & (1 << 1)) != 0; }
			uint8_t GetFlags_ChannelGroups() const { return static_cast<uint8_t>((flags >> 2) & 0x0f); }
			bool GetFlags_External() const { return (flags & (1 << 6)) != 0; }
			bool GetFlags_Inverted() const { return (flags & (1 << 7)) != 0; }
		public:
			void Print(Printable& tout = Stdio::Instance) const;
		};
		enum class Stat { Cmd, Arg };
	private:
		LogicAnalyzer& logicAnalyzer_;
		Stream& stream_;
		struct {
			Stat stat = Stat::Cmd;
			uint8_t cmd = 0;
			uint32_t arg = 0;
			int byteArg = 0;
		} comm_;
		Config cfg_;
	public:
		SUMPAdapter(LogicAnalyzer& logicAnalyzer, Stream& stream);
	public:
		const Config& GetConfig() const { return cfg_; }
	public:
		void ProcessCommand(uint8_t cmd, uint32_t arg);
		void RunCapture();
	private:
		void SendValue(uint32_t value);
		void SendMeta(uint8_t tokenKey);
		void SendMeta_String(uint8_t tokenKey, const char* str);
		void SendMeta_32bit(uint8_t tokenKey, uint32_t value);
		void Flush() { stream_.Flush(); }
	public:
		// virtual functions of Tickable
		virtual const char* GetTickableName() const override { return "SUMPAdapter"; }
		virtual void OnTick() override;
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
	LogicAnalyzer& SetSamplerCount(int nSampler) { nSampler_ = nSampler; return *this; }
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
