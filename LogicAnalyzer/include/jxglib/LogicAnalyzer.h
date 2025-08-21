//==============================================================================
// jxglib/LogicAnalyzer.h
//==============================================================================
#ifndef PICO_JXGLIB_LOGICANALYZER_H
#define PICO_JXGLIB_LOGICANALYZER_H
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"
#include "jxglib/TelePlot.h"
#include "jxglib/Shell.h"

namespace jxglib {

class ProtocolDecoder;

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
class LogicAnalyzer {
public:
	class SamplingInfo;
	enum class RawEventFormat { Auto, Long, Short };
	class RawEvent {
	protected:
		const void* pEntity_;
	public:
		RawEvent() : pEntity_{nullptr} {}
	public:
		void SetEntity(const void* pEntity) { pEntity_ = pEntity; }
	public:
		virtual uint32_t GetTimeStamp(uint nBitsPinBitmap) const = 0;
		virtual uint32_t GetPinBitmap(uint nBitsPinBitmap) const = 0;
	};
	class RawEvent_Short : public RawEvent {
	public:
		struct Entity {
			uint32_t value;
		};
	public:
		RawEvent_Short() {}
	public:
		const Entity& GetEntity() const { return *reinterpret_cast<const Entity*>(pEntity_); }
		uint32_t GetTimeStamp(uint nBitsPinBitmap) const override { return (~(GetEntity().value >> nBitsPinBitmap)) & ((1 << (32 - nBitsPinBitmap)) - 1); }
		uint32_t GetPinBitmap(uint nBitsPinBitmap) const override { return GetEntity().value & ((1 << nBitsPinBitmap) - 1); }
	};
	class RawEvent_Long : public RawEvent {
	public:
		struct Entity {
			uint32_t timeStamp;
			uint32_t pinBitmap;
		};
	public:
		RawEvent_Long() {}
	public:
		const Entity& GetEntity() const { return *reinterpret_cast<const Entity*>(pEntity_); }
		uint32_t GetTimeStamp(uint nBitsPinBitmap) const { return ~GetEntity().timeStamp; }
		uint32_t GetPinBitmap(uint nBitsPinBitmap) const { return GetEntity().pinBitmap; }
	};
	class Event {
	private:
		uint64_t timeStamp_;
		uint32_t pinBitmap_;
	public:
		static const Event None;
	public:
		Event(const Event& event) : timeStamp_{event.timeStamp_}, pinBitmap_{event.pinBitmap_} {}
		Event(uint64_t timeStamp = static_cast<uint64_t>(-1), uint32_t pinBitmap = 0) : timeStamp_{timeStamp}, pinBitmap_{pinBitmap} {}
	public:
		Event& operator=(const Event& event);
	public:
		void Invalidate() { timeStamp_ = static_cast<uint64_t>(-1); pinBitmap_ = 0; }
		bool IsValid() const { return timeStamp_ != static_cast<uint64_t>(-1); }
		uint64_t GetTimeStamp() const { return timeStamp_; }
		uint32_t GetPinBitmap() const { return pinBitmap_; }
		uint32_t GetPackedBitmap(const SamplingInfo& samplingInfo) const;
		template<typename... Pins> static uint32_t MakeMask(Pins... pins) {
			return ((1u << pins) | ...);
		}
		template<typename... Pins> bool IsPinHigh(Pins... pins) const {
			uint32_t mask = MakeMask(pins...);
			return (pinBitmap_ & mask) == mask;
		}
		template<typename... Pins> bool IsPinLow(Pins... pins) const {
			return (pinBitmap_ & MakeMask(pins...)) == 0;
		}
	};
	struct SignalReport {
		uint32_t nSamples;
		uint32_t sigBitmap;
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
		uint8_t* samplingBuff_;
		int bytesSamplingBuff_;
	public:
		Sampler();
		~Sampler();
	public:
		void AssignBuff(void* samplingBuff, int bytesSamplingBuff) {
			samplingBuff_ = reinterpret_cast<uint8_t*>(samplingBuff); bytesSamplingBuff_ = bytesSamplingBuff;
		}
		PIO::StateMachine& GetSM() { return sm_; }
		const PIO::StateMachine& GetSM() const { return sm_; }
		void SetProgram(const PIO::Program& program, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap);
		void ShareProgram(Sampler& sampler, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap);
		Sampler& EnableDMA();
		Sampler& ReleaseResource();
	public:
		const uint8_t* GetSamplingBuff() const { return samplingBuff_; }
		int GetBytesSampled() const;
		int GetBytesSamplingBuff() const { return bytesSamplingBuff_; }
		bool IsFull() const { return GetBytesSampled() >= bytesSamplingBuff_; }
		void DumpSamplingBuff(Printable& tout) const;
	};
	class EventIterator {
	private:
		const LogicAnalyzer& logicAnalyzer_;
		uint pinMin_;
		int nBitsPinBitmap_;
		int iRawEventTbl_[4];
		uint64_t timeStampOffsetTbl_[4];
		uint32_t pinBitmapPrev_;
		bool firstFlag_;
		uint64_t timeStampOffsetIncr_;
	private:
		RawEvent_Short rawEvent_Short_;
		RawEvent_Long rawEvent_Long_;
	public:
		EventIterator(const EventIterator& eventIter);
		EventIterator(const LogicAnalyzer& logicAnalyzer);
	public:
		void Rewind();
		bool HasMore() const;
		bool Next(Event& event);
		void Skip(int nEvents);
		int Count();
		int CountRelevant();
	private:
		const RawEvent* NextRawEvent(int* piSampler = nullptr);
		const RawEvent& GetRawEvent(int iSampler, int iRawEvent);
	};
	struct PrintInfo {
		int nPins;
		std::unique_ptr<uint[]> pinTbl;
		int nEventsToPrint;
		PrintPart part;
		const WaveStyle* pWaveStyle;
	public:
		PrintInfo() : nPins{0}, nEventsToPrint{80}, part{PrintPart::Head}, pWaveStyle{&waveStyle_unicode2} {}
	};
	class SamplingInfo {
	private:
		bool enabledFlag_;
		uint32_t pinBitmapEnabled_;
		uint pinMin_;
		int nPins_;
	public:
		SamplingInfo() : enabledFlag_{false}, pinBitmapEnabled_{0}, pinMin_{0}, nPins_{0} {}
	public:
		void SetEnabled(bool enabledFlag) { enabledFlag_ = enabledFlag; }
		bool IsEnabled() const { return enabledFlag_; }
		uint32_t GetPinBitmapEnabled() const { return pinBitmapEnabled_; }
		uint GetPinMin() const { return pinMin_; }	
		int CountPins() const { return nPins_; }
		bool HasEnabledPins() const { return pinBitmapEnabled_ != 0; }
		bool IsPinEnabled(uint pin) const { return ((pinBitmapEnabled_ << pinMin_) & (1u << pin)) != 0; }
		void Update(const PrintInfo& printInfo);
		int CountBits() const;
	};
public:
	class SigrokAdapter : public Tickable {
	public:
		enum class Stat {
			Initial, Recover,
			Identify,
			AnalogScaleAndOffset,
			SampleRate,
			SampleLimit,
			AnalogChannelEnable, AnalogChannelEnable_Channel,
			DigitalChannelEnable, DigitalChannelEnable_Channel,
			FixedSampleMode,
			ContinuousSampleMode,
		};
	private:
		LogicAnalyzer& logicAnalyzer_;
		EventIterator eventIter_;
		Event event_;
		Printable* pTerr_;
		Stream& stream_;
		Stat stat_;
		int nDigitalChToReport_;
		int nAnalogChToReport_;
		static const int versionNumber_ = 2;
		int uvoltScale_, uvoltOffset_;
		bool enableChannelFlag_;
		int iChannel_;
		int sampleRate_;
		int nSamples_;
		double timeStampFactor_;
		double sampleDelta_;
		int iEvent_;
	private:
		static const int nDigitalChToReportDefault_ = 8;
		static const int nSamplesHead_ = 10;	// number of samples to report at the beginning of a fixed sample mode
		static const int nSamplesTail_ = 10;	// number of samples to report at the end of a fixed sample mode
	public:
		SigrokAdapter(LogicAnalyzer& logicAnalyzer, Printable& terr, Stream& streamApplication);
	public:
		void SetPrintableErr(Printable& terr) { pTerr_ = &terr; }
	public:
		// virtual functions of Tickable
		virtual const char* GetTickableName() const override { return "LogicAnalyzer::SigrokAdapter"; }
		virtual void OnTick() override;
	private:
		void StartSampling();
		int CountSamplesBetweenEvents(const Event& event1, const Event& event2) const;
		void SendReport(const Event& event, int nSamples);
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
		enum class RLEMode {
			PairInclusive	= 0,
			PairExclusive	= 1,
			Periodic		= 2,
			Unlimited		= 3,
		};
		struct Config {
			struct {
				uint32_t mask		= 0;
				uint32_t value		= 0;
				uint32_t config		= 0; 
			} trigger[4];
			uint32_t divider		= 1;
			uint32_t delayCount		= 0;
			uint32_t readCount		= 0;
			uint32_t flags			= 0;
		public:
			uint16_t GetTrigger_Delay(int iStage) const { return static_cast<uint16_t>(trigger[iStage].config & 0xffff); }
			uint8_t GetTrigger_Level(int iStage) const { return static_cast<uint8_t>((trigger[iStage].config >> 16) & 0x03); }
			uint8_t GetTrigger_Channel(int iStage) const { return static_cast<uint8_t>((trigger[iStage].config >> 20) & 0x1f); }
			bool GetTrigger_Serial(int iStage) const { return (trigger[iStage].config & (1 << 26)) != 0; }
			bool GetTrigger_Start(int iStage) const { return (trigger[iStage].config & (1 << 27)) != 0; }
		public:	
			bool GetFlags_DemuxMode() const					{ return (flags & (1 << 0)) != 0; }
			bool GetFlags_NoiseFilter() const				{ return (flags & (1 << 1)) != 0; }
			bool GetFlags_DisableChannelGroup1() const		{ return (flags & (1 << 2)) != 0; }
			bool GetFlags_DisableChannelGroup2() const		{ return (flags & (1 << 3)) != 0; }
			bool GetFlags_DisableChannelGroup3() const		{ return (flags & (1 << 4)) != 0; }
			bool GetFlags_DisableChannelGroup4() const		{ return (flags & (1 << 5)) != 0; }
			bool GetFlags_ExternalClock() const				{ return (flags & (1 << 6)) != 0; }
			bool GetFlags_InvExternalClock() const			{ return (flags & (1 << 7)) != 0; }
			bool GetFlags_RunLengthEncoding() const			{ return (flags & (1 << 8)) != 0; }
			bool GetFlags_SwapChannels() const				{ return (flags & (1 << 9)) != 0; }
			bool GetFlags_ExternalTestMode() const			{ return (flags & (1 << 10)) != 0; }
			bool GetFlags_InternalTestMode() const			{ return (flags & (1 << 11)) != 0; }
			RLEMode GetFlags_RunLengthEncodingMode() const	{ return static_cast<RLEMode>((flags >> 14) & 0x03); }
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
	static const int bytesHeadMargin = 128;
	static const WaveStyle waveStyle_unicode1;
	static const WaveStyle waveStyle_unicode2;
	static const WaveStyle waveStyle_unicode3;
	static const WaveStyle waveStyle_unicode4;
	static const WaveStyle waveStyle_ascii1;
	static const WaveStyle waveStyle_ascii2;
	static const WaveStyle waveStyle_ascii3;
	static const WaveStyle waveStyle_ascii4;
private:
	RawEventFormat rawEventFormat_;
	RawEventFormat rawEventFormatRequested_;
	TelePlot* pTelePlot_;
	PIO::Program program_SamplerInit_;
	PIO::Program program_SamplerMain_;
	uint8_t* samplingBuffWhole_;
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
	std::unique_ptr<ProtocolDecoder> pProtocolDecoder_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& AttachTelePlot(TelePlot& telePlot) { pTelePlot_ = &telePlot; return *this; }
public:
	bool Enable();
	LogicAnalyzer& Disable();
	bool IsRawEventFormatShort() const { return rawEventFormat_ == RawEventFormat::Short; }
	bool IsRawEventFormatLong() const { return rawEventFormat_ == RawEventFormat::Long; }
	const SignalReport* CreateSignalReport(int nSignals, double samplePeriod, int* pnSignalReport);
	LogicAnalyzer& SetPIO(uint iPIO) { iPIO_ = iPIO; return *this; }
	uint8_t* GetSamplingBuffWhole() { return samplingBuffWhole_; }
	LogicAnalyzer& ReleaseResource();
	LogicAnalyzer& SetPins(const int pinTbl[], int nPins);
	LogicAnalyzer& SetSamplerCount(int nSampler) { nSampler_ = nSampler; return *this; }
	const SamplingInfo& GetSamplingInfo() const { return samplingInfo_; }
	const PrintInfo& GetPrintInfo() const { return printInfo_; }
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
	LogicAnalyzer& SetEventFormat(RawEventFormat rawEventFormat) { rawEventFormatRequested_ = rawEventFormat; return *this; }
	double GetSampleRate() const { return static_cast<double>(::clock_get_hz(clk_sys) / clocksPerLoop_); }
	float GetResolution() const { return usecReso_; }
	LogicAnalyzer& UpdateSamplingInfo() { samplingInfo_.Update(printInfo_); return *this; }
	bool HasEnabledPins() const { return samplingInfo_.HasEnabledPins(); }
	bool IsRawEventFull() const { return samplerTbl_[0].IsFull(); }
	int GetRawEventCount(int iSampler) const;
	int GetRawEventCount() const;
	int GetRawEventCountMax() const;
	const LogicAnalyzer& PrintWave(Printable& tout, Printable& terr) const;
	ProtocolDecoder* SetProtocolDecoder(const char* protocolName);
	const LogicAnalyzer& PlotWave() const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
public:
	static size_t GetFreeHeapBytes();
};

//------------------------------------------------------------------------------
// ProtocolDecoder
//------------------------------------------------------------------------------
class ProtocolDecoder {
public:
	using EventIterator = LogicAnalyzer::EventIterator;
	using Event = LogicAnalyzer::Event;
public:
	class Factory {
	protected:
		const char* name_;
		Factory* pFactoryNext_;
	private:
		static Factory* pFactoryHead_;
	public:
		Factory(const char* name);
	public:
		const char* GetName() const { return name_; }
		void SetNext(Factory* pFactory) { pFactoryNext_ = pFactory; }
		Factory* GetNext() const { return pFactoryNext_; }
	public:
		static Factory* GetHead() { return pFactoryHead_; }
		static Factory* Find(const char* name);
	public:
		virtual ProtocolDecoder* Create(const LogicAnalyzer& logicAnalyzer) = 0;
	};
protected:
	const LogicAnalyzer& logicAnalyzer_;
	const char* name_;
public:
	ProtocolDecoder(const LogicAnalyzer& logicAnalyzer, const char* name) : logicAnalyzer_{logicAnalyzer}, name_{name} {}
	virtual ~ProtocolDecoder() = default;
public:
	const char* GetName() const { return name_; }
public:
	virtual bool EvalSubcmd(Printable& terr, const char* subcmd) { return false; }
	virtual bool CheckValidity(Printable& terr) { return false; }
	virtual void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol) = 0;
	virtual void AnnotateWaveStreak(char* buffLine, int lenBuffLine, int *piCol) = 0;
};

}

#endif
