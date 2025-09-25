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
#include "jxglib/Shell.h"
#include "jxglib/JSON.h"

namespace jxglib {

class Decoder;

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
class LogicAnalyzer {
public:
	class PrintInfo;
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
		uint32_t GetPackedBitmap(const PrintInfo& printInfo) const;
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
	enum class PinTarget : uint8_t { Inherited, Internal, External };
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
		int CountValidPins() const;
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
	class Decoder {
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
			virtual Decoder* Create(const LogicAnalyzer& logicAnalyzer) = 0;
		};
	protected:
		const LogicAnalyzer& logicAnalyzer_;
		const char* name_;
	public:
		Decoder(const LogicAnalyzer& logicAnalyzer, const char* name);
		virtual ~Decoder() = default;
	public:
		const char* GetName() const { return name_; }
		void PrintEvent(Printable& tout);
		void AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol);
		void AnnotateWaveStreak(double timeStamp, char* buffLine, int lenBuffLine, int *piCol);
	public:
		virtual bool EvalSubcmd(Printable& tout, Printable& terr, const char* subcmd) { return false; }
		virtual bool CheckValidity(Printable& terr) { return false; }
		virtual void Reset() = 0;
		virtual int GetColsAnnotation() const = 0;
		virtual void DoAnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol) = 0;
		virtual void DoAnnotateWaveStreak(double timeStamp, char* buffLine, int lenBuffLine, int *piCol) = 0;
	public:
		static bool IsValidPin(uint pin) { return LogicAnalyzer::IsValidPin(pin); }
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
		Stream* pStream_;
		Stat stat_;
		int nDigitalChAvailable_;
		int nAnalogChAvailable_;
		int nDigitalChEnabled_;
		int nAnalogChEnabled_;
		uint32_t digitalChBitmapEnabled_;
		uint32_t analogChBitmapEnabled_;
		static const int versionNumber_ = 2;
		int uvoltScale_, uvoltOffset_;
		bool enableChannelFlag_;
		int iChannel_;
		int sampleRate_;
		int nSamples_;
		uint8_t embeddedRLEData_;
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
		Stream& GetStream() { return *pStream_; }
	public:
		// virtual functions of Tickable
		virtual const char* GetTickableName() const override { return "LogicAnalyzer::SigrokAdapter"; }
		virtual void OnTick() override;
	private:
		void Reset();
		void StartSampling();
		int CountSamplesBetweenEvents(const Event& event1, const Event& event2) const;
		void SendBitmap(const Event& event);
		void SendRLE(int nSamples);
		void SendReport(const Event& event, int nSamples);
	};
public:
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
	PIO::Program program_SamplerInit_;
	PIO::Program program_SamplerMain_;
	uint8_t* samplingBuffWhole_;
	uint iPIO_;
	int nSampler_;
	Sampler samplerTbl_[4];
	SamplingInfo samplingInfo_;
	PrintInfo printInfo_;
	PinTarget pinTargetGlobal_;
	PinTarget pinTargetTbl_[GPIO::NumPins];
	float heapRatio_;
	float heapRatioRequested_;
	int clocksPerLoop_;
	float usecReso_;
	std::unique_ptr<Decoder> pDecoder_;
	bool suppressPrintSettingsFlag_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	bool Enable();
	LogicAnalyzer& Disable();
	bool IsRawEventFormatShort() const { return rawEventFormat_ == RawEventFormat::Short; }
	bool IsRawEventFormatLong() const { return rawEventFormat_ == RawEventFormat::Long; }
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
	LogicAnalyzer& SetPinTargetGlobal(PinTarget pinTarget) { pinTargetGlobal_ = pinTarget; return *this; }
	LogicAnalyzer& SetPinTarget(uint pin, PinTarget pinTarget) { pinTargetTbl_[pin] = pinTarget; return *this; }
	PinTarget GetPinTarget(uint pin) const;
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { printInfo_.nEventsToPrint = nEventsToPrint; return *this; }
	LogicAnalyzer& SetPrintPart(PrintPart part) { printInfo_.part = part; return *this; }
	PrintPart GetPrintPart() const { return printInfo_.part; }
	LogicAnalyzer& SetWaveStyle(const WaveStyle& waveStyle) { printInfo_.pWaveStyle = &waveStyle; return *this; }
	const WaveStyle& GetWaveStyle() const { return *printInfo_.pWaveStyle; }
	LogicAnalyzer& SetEventFormat(RawEventFormat rawEventFormat) { rawEventFormatRequested_ = rawEventFormat; return *this; }
	double GetSampleRate() const { return static_cast<double>(::clock_get_hz(clk_sys) / clocksPerLoop_) * nSampler_; }
	float GetResolution() const { return usecReso_; }
	LogicAnalyzer& UpdateSamplingInfo() { samplingInfo_.Update(printInfo_); return *this; }
	bool HasEnabledPins() const { return samplingInfo_.HasEnabledPins(); }
	bool IsRawEventFull() const { return samplerTbl_[0].IsFull(); }
	int GetRawEventCount(int iSampler) const;
	int GetRawEventCount() const;
	int GetRawEventCountMax() const;
	const LogicAnalyzer& PrintWave(Printable& tout, Printable& terr, bool liveFlag = false) const;
	const LogicAnalyzer& WriteFileJSON(Printable& tout) const;
	Decoder* SetDecoder(const char* decoderName);
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
	void SuppressPrintSettings(bool suppressPrintSettingsFlag = true) { suppressPrintSettingsFlag_ = suppressPrintSettingsFlag; }
public:
	static size_t GetFreeHeapBytes();
	static bool IsBlankPin(uint pin) { return pin == static_cast<uint>(-1); }
	static bool IsValidPin(uint pin) { return pin < GPIO::NumPins; }
	static bool IsAnnotationPin(uint pin) { return !IsBlankPin(pin) && pin >= GPIO::NumPins; }
};

}

#endif
