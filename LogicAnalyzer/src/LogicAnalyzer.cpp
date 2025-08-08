
//==============================================================================
// LogicAnalyzer.cpp
//==============================================================================
#include <stdlib.h>
#include <memory.h>
#include "jxglib/BinaryInfo.h"
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_unicode1 = {
	name:			"unicode1",
	strBlank:		"   ",
	strHigh:		"  │",
	strHighIdle:	"  :",
	strLow:			" │ ",
	strLowIdle:		" : ",
	strLowToHigh:	" └┐",
	strHighToLow:	" ┌┘",
	formatHeader:	"P%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_unicode2 = {
	name:			"unicode2",
	strBlank:		"    ",
	strHigh:		"   │",
	strHighIdle:	"   :",
	strLow:			" │  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" └─┐",
	strHighToLow:	" ┌─┘",
	formatHeader:	"P%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_unicode3 = {
	name:			"unicode3",
	strBlank:		"     ",
	strHigh:		"    │",
	strHighIdle:	"    :",
	strLow:			" │   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" └──┐",
	strHighToLow:	" ┌──┘",
	formatHeader:	"GP%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_unicode4 = {
	name:			"unicode4",
	strBlank:		"      ",
	strHigh:		"     │",
	strHighIdle:	"     :",
	strLow:			" │    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" └───┐",
	strHighToLow:	" ┌───┘",
	formatHeader:	"GP%-2d  ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_ascii1 = {
	name:			"ascii1",
	strBlank:		"   ",
	strHigh:		"  |",
	strHighIdle:	"  :",
	strLow:			" | ",
	strLowIdle:		" : ",
	strLowToHigh:	" ',",
	strHighToLow:	" ,'",
	formatHeader:	"P%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_ascii2 = {
	name:			"ascii2",
	strBlank:		"    ",
	strHigh:		"   |",
	strHighIdle:	"   :",
	strLow:			" |  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" `-,",
	strHighToLow:	" ,-'",
	formatHeader:	"P%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_ascii3 = {
	name:			"ascii3",
	strBlank:		"     ",
	strHigh:		"    |",
	strHighIdle:	"    :",
	strLow:			" |   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" `--,",
	strHighToLow:	" ,--'",
	formatHeader:	"GP%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_ascii4 = {
	name:			"ascii4",
	strBlank:		"      ",
	strHigh:		"     |",
	strHighIdle:	"     :",
	strLow:			" |    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" `---,",
	strHighToLow:	" ,---'",
	formatHeader:	"GP%-2d  ",
};

LogicAnalyzer::LogicAnalyzer() : rawEventFormat_{RawEventFormat::Short}, rawEventFormatRequested_{RawEventFormat::Auto},
		pTelePlot_{nullptr}, samplingBuffWhole_{nullptr}, iPIO_{PIO::Num - 1},
		nSampler_{1}, target_{Target::Internal}, heapRatio_{.7}, heapRatioRequested_{.7}, usecReso_{1'000}
{
	clocksPerLoop_ = 12; // program_SampleMain_ takes 12 clocks in the loop
	SetSamplerCount(1);
}

LogicAnalyzer::~LogicAnalyzer()
{
	if (samplingBuffWhole_) ::free(samplingBuffWhole_);
}

bool LogicAnalyzer::Enable()
{
	ReleaseResource();
	if (!samplingInfo_.HasEnabledPins()) return true;
	if (heapRatio_ != heapRatioRequested_ && samplingBuffWhole_) {
		::free(samplingBuffWhole_);
		samplingBuffWhole_ = nullptr;
	}
	int bytesSamplingBuffPerSampler = static_cast<int>(heapRatioRequested_ * GetFreeHeapBytes()) /
						nSampler_ / sizeof(RawEvent_Long::Entity) * sizeof(RawEvent_Long::Entity);
	if (!samplingBuffWhole_) {
		samplingBuffWhole_ = reinterpret_cast<uint8_t*>(::malloc(bytesHeadMargin + nSampler_ * bytesSamplingBuffPerSampler));
		if (!samplingBuffWhole_) return false;
	}
	heapRatio_ = heapRatioRequested_;
	for (int iSampler = 0; iSampler < nSampler_; ++iSampler) {
		samplerTbl_[iSampler].AssignBuff(samplingBuffWhole_ + bytesHeadMargin + iSampler * bytesSamplingBuffPerSampler, bytesSamplingBuffPerSampler);
	}
	uint nBitsTimeStamp = 32;
	uint nBitsPinToReport = 32;
	uint nBitsPinToSample = samplingInfo_.CountBits(); // must be less than 32 to avoid auto-push
	rawEventFormat_ = (rawEventFormatRequested_ != RawEventFormat::Auto)? rawEventFormatRequested_ :
					(nBitsPinToSample <= 8)? RawEventFormat::Short : RawEventFormat::Long;
	if (rawEventFormat_ == RawEventFormat::Short) {
		nBitsTimeStamp = 32 - nBitsPinToSample;
		nBitsPinToReport = nBitsPinToSample;
	}
	uint relAddrEntryTbl[4];
	program_SamplerInit_
	.program("sampler_init")
		.mov("osr", "~null")						// osr = 0xffffffff
	.end();
	program_SamplerMain_
	.program("sampler_main")
	.pub(&relAddrEntryTbl[3])
		.jmp("entry") [(nSampler_ == 4)? (9 - 1) : 0]
	.pub(&relAddrEntryTbl[2])
		.jmp("entry") [(nSampler_ == 4)? (6 - 1) : (nSampler_ == 3)? (8 - 1) :  0]
	.pub(&relAddrEntryTbl[1])
		.jmp("entry") [(nSampler_ == 4)? (3 - 1) : (nSampler_ == 3)? (4 - 1) : (nSampler_ == 2)? (6 - 1) :  0]
	.L("loop").wrap_target()
		.out("x", nBitsTimeStamp)					// x[nBitsTimeStamp-1:0] = osr[nBitsTimeStamp-1:0]
		.jmp("x--", "no_wrap_around")				// if (x == 0) {x--} else {x--; goto no_wrap_around}
		.mov("osr", "x")							// osr = x
	.L("entry").pub(&relAddrEntryTbl[0])
		.mov("isr", "null")							// isr = 0x00000000
		.in("pins", nBitsPinToSample)				// isr = pins[nBitsPinToSample-1:0] (no auto-push)
		.mov("x", "isr")							// x = isr
		.jmp("report_event") [1]					// goto report_event
	.L("no_wrap_around")
		.mov("osr", "x")							// osr = x
		.mov("isr", "null")							// isr = 0x00000000
		.in("pins", nBitsPinToSample)				// isr = pins[nBitsPinToSample-1:0] (no auto-push)
		.mov("x", "isr")							// x = isr
		.jmp("x!=y", "report_event")				// if (x != y) goto report_event
		.jmp("loop") [4]							// goto loop
	.L("report_event")
		.mov("isr", "null")							// isr = 0x00000000
		.in("osr", nBitsTimeStamp)					// isr[nBitsTimeStamp-1:0] = osr[nBitsTimeStamp-1:0]
		.in("x", nBitsPinToReport)					// isr[31:nBitsPinToReport] = isr[nBitsTimeStamp-1:0]
													// isr[nBitsPinToReport-1:0] = x[nBitsPinToReport-1:0] (auto-push)
		.mov("y", "x") [1]							// y = x
	.wrap()
	.end();
	pio_hw_t* pio = ::pio_get_instance(iPIO_);
	samplerTbl_[0].SetProgram(program_SamplerMain_, pio, 0, relAddrEntryTbl[0], samplingInfo_.GetPinMin(), nBitsPinToSample);
	for (int iSampler = 1; iSampler < nSampler_; ++iSampler) {
		samplerTbl_[iSampler].ShareProgram(samplerTbl_[0], pio, iSampler, relAddrEntryTbl[iSampler], samplingInfo_.GetPinMin(), nBitsPinToSample);
	}
	if (target_ == Target::External) {
		uint pin = samplingInfo_.GetPinMin();
		for (uint32_t pinBitmap = samplingInfo_.GetPinBitmapEnabled(); pinBitmap != 0; pinBitmap >>= 1, ++pin) {
			if (pinBitmap & 1) {
				::pio_gpio_init(samplerTbl_[0].GetSM().pio, pin);
				::gpio_disable_pulls(pin);
			}
		}
	}
	uint32_t mask = 0;
	for (int iSampler = 0; iSampler < nSampler_; ++iSampler) {
		samplerTbl_[iSampler].EnableDMA();
		samplerTbl_[iSampler].GetSM().exec(program_SamplerInit_);
		mask |= (1 << samplerTbl_[iSampler].GetSM().sm);
	}
	::pio_enable_sm_mask_in_sync(pio, mask);	// enable all state machines simultaneously
	samplingInfo_.SetEnabled(true);
	return true;
}

LogicAnalyzer& LogicAnalyzer::Disable()
{
	::free(samplingBuffWhole_);
	samplingBuffWhole_ = nullptr;
	return ReleaseResource();
}

LogicAnalyzer& LogicAnalyzer::ReleaseResource()
{
	if (!samplingInfo_.IsEnabled()) return *this;
	samplingInfo_.SetEnabled(false);
	for (int iSampler = 0; iSampler < count_of(samplerTbl_); ++iSampler) {
		samplerTbl_[iSampler].ReleaseResource();
	}
	if (target_ == Target::External) {
		uint pin = samplingInfo_.GetPinMin();
		for (uint32_t pinBitmap = samplingInfo_.GetPinBitmapEnabled(); pinBitmap != 0; pinBitmap >>= 1, ++pin) {
			if (pinBitmap & 1) {
				::gpio_set_dir(pin, GPIO_IN);
				::gpio_put(pin, 0);
				::gpio_pull_down(pin);
				::gpio_set_function(pin, GPIO_FUNC_NULL);
			}
		}
	}
	return *this;
}

LogicAnalyzer& LogicAnalyzer::SetPins(const int pinTbl[], int nPins)
{
	printInfo_.nPins = nPins;
	if (nPins == 0) {
		printInfo_.pinTbl.reset();
	} else {
		printInfo_.pinTbl.reset(new uint[nPins]);
		for (int i = 0; i < nPins; ++i) {
			int pin = pinTbl[i];
			printInfo_.pinTbl[i] = pin;
		}
	}
	return *this;
}

int LogicAnalyzer::GetRawEventCount(int iSampler) const
{
	if (!samplingInfo_.IsEnabled()) return 0;
	return samplerTbl_[iSampler].GetBytesSampled() /
		((rawEventFormat_ == RawEventFormat::Short)? sizeof(RawEvent_Short::Entity) : sizeof(RawEvent_Long::Entity));
}

int LogicAnalyzer::GetRawEventCount() const
{
	if (!samplingInfo_.IsEnabled()) return 0;
	int nEvent = 0;
	for (int iSampler = 0; iSampler < nSampler_; ++iSampler) nEvent += GetRawEventCount(iSampler); //samplerTbl_[iSampler].GetRawEventCount();
	return nEvent;
}

int LogicAnalyzer::GetRawEventCountMax() const
{
	if (!samplingInfo_.IsEnabled()) return 0;
	int bytesSamplingBuff = 0;
	for (int iSampler = 0; iSampler < nSampler_; ++iSampler) bytesSamplingBuff += samplerTbl_[iSampler].GetBytesSamplingBuff();
	return bytesSamplingBuff /
		((rawEventFormat_ == RawEventFormat::Short)? sizeof(RawEvent_Short::Entity) : sizeof(RawEvent_Long::Entity));
}

const LogicAnalyzer& LogicAnalyzer::PrintWave(Printable& tout)
{
	uint nBitsPinBitmap = samplingInfo_.CountBits();
	EventIterator eventIter(*this, nBitsPinBitmap);
	int nEventsRelevant = eventIter.CountRelevant();
	if (nEventsRelevant == 0) {
		tout.Printf("no events to print\n");
		return *this;
	}
	//for (int iSampler = 0; iSampler < nSampler_; ++iSampler) samplerTbl_[iSampler].DumpSamplingBuff(tout);
	const char* strBlank = "    ";
	const WaveStyle& waveStyle = *printInfo_.pWaveStyle;
	auto printHeader = [&]() {
		tout.Printf("%12s ", "Time [usec]");
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
			} else {
				tout.Printf(waveStyle.formatHeader, pin);
			}
		}
		tout.Println();
	};
	printHeader();
	int nEventsToPrint =
		(printInfo_.part == PrintPart::Head)? ChooseMin(nEventsRelevant, printInfo_.nEventsToPrint) :
		(printInfo_.part == PrintPart::Tail)?  ChooseMin(nEventsRelevant, printInfo_.nEventsToPrint) :
		(printInfo_.part == PrintPart::All)? nEventsRelevant : 0;
	int nEventsToSkip =
		(printInfo_.part == PrintPart::Head)? 0 :
		(printInfo_.part == PrintPart::Tail)? nEventsRelevant - nEventsToPrint :
		(printInfo_.part == PrintPart::All)? 0 : 0;
	int iEventBase = (nEventsToPrint == 0)? 0 : 1;
	Event eventInitial, eventBase;
	if (nEventsToSkip == 0) {
		eventIter.Next(eventInitial);
		if (!eventIter.Next(eventBase)) eventBase = eventInitial;
	} else {
		eventIter.Skip(nEventsToSkip);
		eventIter.Next(eventInitial);
		eventBase = eventInitial;
	}
	if (nEventsToPrint > 0) {
		tout.Printf("%12s", "");
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
				continue;
			}
			if (samplingInfo_.IsPinEnabled(pin)) {
				tout.Print(samplingInfo_.IsPinAsserted(eventInitial.GetPinBitmap(), pin)? waveStyle.strHigh : waveStyle.strLow);
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
	}
	Event eventPrev = eventInitial;
	Event event = eventBase;
	double timeStampFactor = 1000'000. / GetSampleRate() / nSampler_;
	for (int iEvent = 0; iEvent < nEventsToPrint && !eventIter.IsDone(); ++iEvent, eventIter.Next(event)) {
		double delta = static_cast<double>(event.GetTimeStamp() - eventPrev.GetTimeStamp()) * timeStampFactor;
		int nDelta = static_cast<int>(delta / usecReso_);
		if (nDelta < 40) {
			for (int i = 0; i < nDelta; ++i) {
				tout.Printf("%12s", "");
				for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
					uint pin = printInfo_.pinTbl[iPin];
					if (pin == -1) {
						tout.Print(strBlank);
						continue;
					}
					if (samplingInfo_.IsPinEnabled(pin)) {
						tout.Print(samplingInfo_.IsPinAsserted(eventPrev.GetPinBitmap(), pin)? waveStyle.strHigh : waveStyle.strLow);
					} else {
						tout.Print(waveStyle.strBlank);
					}
				}
				tout.Println();
			}
		} else {
			tout.Printf("%12s", "");
			for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
				uint pin = printInfo_.pinTbl[iPin];
				if (pin == -1) {
					tout.Print(strBlank);
					continue;
				}
				if (samplingInfo_.IsPinEnabled(pin)) {
					tout.Print(samplingInfo_.IsPinAsserted(eventPrev.GetPinBitmap(), pin)? waveStyle.strHighIdle : waveStyle.strLowIdle);
				} else {
					tout.Print(waveStyle.strBlank);
				}
			}
			tout.Println();
		}
		uint32_t bitsTransition = event.GetPinBitmap() ^ eventPrev.GetPinBitmap();
		tout.Printf("%12.2f", static_cast<double>(event.GetTimeStamp() - eventBase.GetTimeStamp()) * timeStampFactor);
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
				continue;
			}
			if (samplingInfo_.IsPinEnabled(pin)) {
				tout.Print(samplingInfo_.IsPinAsserted(bitsTransition, pin)?
					(samplingInfo_.IsPinAsserted(event.GetPinBitmap(), pin)? waveStyle.strLowToHigh : waveStyle.strHighToLow) :
					(samplingInfo_.IsPinAsserted(event.GetPinBitmap(), pin)? waveStyle.strHigh : waveStyle.strLow));
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
		if (Tickable::TickSub()) break;
		eventPrev = event;
	}
	if (nEventsToPrint > 0) printHeader();
	return *this;
}

// This method destroys the sampling buffers and creates a set of SignalReport. 
const LogicAnalyzer::SignalReport* LogicAnalyzer::CreateSignalReport(int nSamples, double samplePeriod, int* pnSignalReports)
{
	SignalReport* signalReportTbl = reinterpret_cast<SignalReport*>(GetSamplingBuffWhole());
	*pnSignalReports = 0;
	uint nBitsPinBitmap = samplingInfo_.CountBits();
	EventIterator eventIter(*this, nBitsPinBitmap);
	int nEventsRelevant = eventIter.CountRelevant();
	Event event, eventPrev;
	double timeStampFactor = 1000'000. / GetSampleRate() / GetSamplerCount();
	int iEvent = 0;
	int nSamplesCaptured = 0;
	if (eventIter.Next(eventPrev)) {
		int iSignalReport = 0;
		iEvent++;
		signalReportTbl[iSignalReport++] = SignalReport {1, eventPrev.GetPinBitmap()};
		nSamplesCaptured++;
		uint32_t pinBitmap = 0;
		for ( ; eventIter.Next(event) && iEvent < nEventsRelevant && nSamplesCaptured < nSamples; ++iEvent) {
			pinBitmap = event.GetPinBitmap();
			if (pinBitmap == eventPrev.GetPinBitmap()) continue;
			double timeDelta = (event.GetTimeStamp() - eventPrev.GetTimeStamp()) * timeStampFactor;
			if (timeDelta < samplePeriod) continue;
			uint32_t nSamples = static_cast<uint32_t>(timeDelta / samplePeriod);
			signalReportTbl[iSignalReport++] = SignalReport { nSamples, pinBitmap };
			nSamplesCaptured += nSamples;
			eventPrev = event;
		}
		if (nSamplesCaptured > nSamples) {
			signalReportTbl[iSignalReport - 1].nSamples -= nSamplesCaptured - nSamples; // trim the last sample
		} else if (nSamplesCaptured < nSamples) {
			signalReportTbl[iSignalReport - 1].nSamples += nSamples - nSamplesCaptured; // fill the rest with the last sample
		}
		*pnSignalReports = iSignalReport;
	}
	return signalReportTbl;
}

const LogicAnalyzer& LogicAnalyzer::PlotWave()
{
	if (!pTelePlot_) return *this;
	TelePlot& tp = *pTelePlot_;
	uint nBitsPinBitmap = samplingInfo_.CountBits();
	EventIterator eventIter(*this, nBitsPinBitmap);
	int nEventsRelevant = eventIter.CountRelevant();
	if (nEventsRelevant == 0) return *this;
	double timeStampFactor = 1000'000. / GetSampleRate() / nSampler_;
	Event eventInitial, eventBase;
	eventIter.Next(eventInitial);
	if (!eventIter.Next(eventBase)) eventBase = eventInitial;
	Event eventPrev = eventBase;
	Event event = eventBase;
	TelePlot::Telemetry t = tp.Add("GPIO1");
	for (int iEvent = 0; iEvent < 50 && !eventIter.IsDone(); ++iEvent, eventIter.Next(event)) {
		double delta = static_cast<double>(event.GetTimeStamp() - eventPrev.GetTimeStamp()) * timeStampFactor;
		int nDelta = static_cast<int>(delta * 10 / usecReso_);
		if (nDelta > 100) nDelta = 100;
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) continue;
			if (samplingInfo_.IsPinEnabled(pin)) {
				float value = samplingInfo_.IsPinAsserted(eventPrev.GetPinBitmap(), pin)? .2 : 0;
				for (int i = 0; i < nDelta; ++i) t.Plot(value);
			}
		}
		if (Tickable::TickSub()) break;
		eventPrev = event;
	}
	return *this;
}

const LogicAnalyzer& LogicAnalyzer::PrintSettings(Printable& tout) const
{
	int nRawEvents = GetRawEventCount();
	if (samplingInfo_.IsEnabled()) {
		const PIO::StateMachine& sm = samplerTbl_[0].GetSM();
		tout.Printf("enabled%s pio:%d", samplerTbl_[0].IsFull()? "(full)" : "", sm.pio.get_index());
	} else {
		tout.Printf("disabled ----");
	}
	tout.Printf(" %.1fMHz (samplers:%d) target:%s", GetSampleRate() * nSampler_ / 1000'000.,
		nSampler_, (target_ == Target::Internal)? "internal" : "external");
	do {
		bool firstFlag = true;
		tout.Printf(" pins:");
		uint pinStart = -1;
		uint pinEnd = -1;
		bool inSequence = false;	
		for (uint pin = 0; pin <= GPIO::NumPins; ++pin) {
			if (pin < GPIO::NumPins && samplingInfo_.IsPinEnabled(pin)) {
				if (inSequence) {
					pinEnd = pin;
				} else {
					pinStart = pinEnd = pin;
					inSequence = true;
				}
			} else if (inSequence) {
				if (pinEnd - pinStart < 2) {
					for (uint pin = pinStart; pin <= pinEnd; ++pin) {
						tout.Printf(firstFlag? "%d" : ",%d", pin);
						firstFlag = false;
					}
				} else {
					tout.Printf(firstFlag? "%d-%d" : ",%d-%d", pinStart, pinEnd);
					firstFlag = false;
				}
				inSequence = false;
			}
		}
		if (firstFlag) tout.Print("none");
	} while (0);
	tout.Printf(" events:%d/%d%s (heap-ratio:%.1f)", nRawEvents, GetRawEventCountMax(),
						(rawEventFormatRequested_ == RawEventFormat::Long)? "L" : "", heapRatio_);
	tout.Println();
	return *this;
}

size_t LogicAnalyzer::GetFreeHeapBytes()
{
    return &__stack - &__bss_end__;
}

//------------------------------------------------------------------------------
// LogicAnalyzer::Sampler
//------------------------------------------------------------------------------
LogicAnalyzer::Sampler::Sampler() : pChannel_{nullptr}, bytesSamplingBuff_{0}, samplingBuff_{nullptr}
{
}

LogicAnalyzer::Sampler::~Sampler()
{
	if (pChannel_) pChannel_->unclaim();
}

void LogicAnalyzer::Sampler::SetProgram(const PIO::Program& program, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap)
{
	sm_.config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	sm_.set_program(program, pio, sm).set_listen_pins(pinMin, nBitsPinBitmap).init_with_entry(relAddrEntry);
}

void LogicAnalyzer::Sampler::ShareProgram(Sampler& sampler, pio_hw_t* pio, uint sm, uint relAddrEntry, uint pinMin, int nBitsPinBitmap)
{
	sm_.config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	sm_.share_program(sampler.GetSM(), pio, sm).set_listen_pins(pinMin, nBitsPinBitmap).init_with_entry(relAddrEntry);
}

LogicAnalyzer::Sampler& LogicAnalyzer::Sampler::EnableDMA()
{
	//sm_.set_enabled();
	pChannel_ = DMA::claim_unused_channel();
	channelConfig_.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(sm_.get_dreq_rx()) // set DREQ of StateMachine's rx
		.set_chain_to(*pChannel_)    // disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(samplingBuff_)
		.set_trans_count_trig(bytesSamplingBuff_ / sizeof(uint32_t));
	return *this;
}

LogicAnalyzer::Sampler& LogicAnalyzer::Sampler::ReleaseResource()
{
	if (!pChannel_) return *this; // already disabled
	sm_.set_enabled(false);
	sm_.remove_program();
	pChannel_->abort();
	pChannel_->unclaim();
	pChannel_ = nullptr;
	samplingBuff_ = nullptr;
	bytesSamplingBuff_ = 0;
	return *this;
}

int LogicAnalyzer::Sampler::GetBytesSampled() const
{
	return pChannel_? (reinterpret_cast<uint32_t>(pChannel_->get_write_addr()) - reinterpret_cast<uint32_t>(samplingBuff_)) : 0;
}

void LogicAnalyzer::Sampler::DumpSamplingBuff(Printable& tout) const
{
	Printable::DumpT(tout).Data32Bit()(samplingBuff_, GetBytesSampled() / sizeof(uint32_t));
}

//------------------------------------------------------------------------------
// LogicAnalyzer::SamplingInfo
//------------------------------------------------------------------------------
void LogicAnalyzer::SamplingInfo::Update(const PrintInfo& printInfo)
{
	pinBitmapEnabled_ = 0;
	pinMin_ = GPIO::NumPins;
	for (int i = 0; i < printInfo.nPins; ++i) {
		uint pin = printInfo.pinTbl[i];
		if (pin != -1) {
			if (pinMin_ > pin) pinMin_ = pin;
			pinBitmapEnabled_ |= (1 << pin);
		}
	}
	pinBitmapEnabled_ >>= pinMin_;
}

int LogicAnalyzer::SamplingInfo::CountBits() const
{
	uint nBits = 0;
	for (uint32_t pinBitmap = pinBitmapEnabled_; pinBitmap != 0; pinBitmap >>= 1, ++nBits) ;
	return nBits;
}

//------------------------------------------------------------------------------
// LogicAnalyzer::EventIterator
//------------------------------------------------------------------------------
LogicAnalyzer::EventIterator::EventIterator(LogicAnalyzer& logicAnalyzer, int nBitsPinBitmap) :
		logicAnalyzer_{logicAnalyzer}, pinBitmapPrev_{0}, firstFlag_{true}, doneFlag_{false},
		timeStampOffsetIncr_{0}, nBitsPinBitmap_{nBitsPinBitmap}
{
	int nBitsTimeStamp = 32 - nBitsPinBitmap;
	timeStampOffsetIncr_ = 1LL << nBitsTimeStamp;
	Rewind();
}

bool LogicAnalyzer::EventIterator::Next(Event& event)
{
	if (doneFlag_) return false;
	int iSampler;
	const RawEvent* pRawEvent = NextRawEvent(&iSampler);
	if (!pRawEvent) {
		doneFlag_ = true;
		return false;
	}
	uint64_t timeStamp = (timeStampOffsetTbl_[iSampler] + pRawEvent->GetTimeStamp(nBitsPinBitmap_)) * logicAnalyzer_.GetSamplerCount() + iSampler;
	uint32_t pinBitmap = pRawEvent->GetPinBitmap(nBitsPinBitmap_);
	event = Event(timeStamp, pinBitmap);
	pinBitmapPrev_ = pinBitmap;
	firstFlag_= false;
	return true;
}

void LogicAnalyzer::EventIterator::Rewind()
{
	for (int iSampler = 0; iSampler < logicAnalyzer_.GetSamplerCount(); ++iSampler) {
		iRawEventTbl_[iSampler] = 0;
		timeStampOffsetTbl_[iSampler] = 0;
	}
	pinBitmapPrev_ = 0;
	firstFlag_= true;
	doneFlag_ = false;
}

void LogicAnalyzer::EventIterator::Skip(int nEvents)
{
	for (int iEvent = 0; iEvent < nEvents && NextRawEvent(); ++iEvent) ;
}

int LogicAnalyzer::EventIterator::Count()
{
	int nEvent = 0;
	while (NextRawEvent()) ++nEvent;
	Rewind();
	return nEvent;
}

int LogicAnalyzer::EventIterator::CountRelevant()
{
	int nEvent = 0, nEventRtn = 0;;
	while (const RawEvent* pRawEvent = NextRawEvent()) {
		if (pRawEvent->GetTimeStamp(nBitsPinBitmap_) != 0) nEventRtn = nEvent;
		++nEvent;
	}
	Rewind();
	return nEventRtn;
}

const LogicAnalyzer::RawEvent* LogicAnalyzer::EventIterator::NextRawEvent(int* piSampler)
{
	for (;;) {
		int iSamplerRtn = -1;
		uint64_t timeStampAdjRtn = 0;
		for (int iSampler = logicAnalyzer_.GetSamplerCount() - 1; iSampler >= 0; --iSampler) {
			int iRawEvent = iRawEventTbl_[iSampler];
			if (iRawEvent < logicAnalyzer_.GetRawEventCount(iSampler)) {
				const RawEvent& rawEvent = GetRawEvent(iSampler, iRawEvent);
				uint64_t timeStampAdj = timeStampOffsetTbl_[iSampler] + rawEvent.GetTimeStamp(nBitsPinBitmap_);
				if (iRawEvent > 0 && rawEvent.GetTimeStamp(nBitsPinBitmap_) == 0) timeStampAdj += timeStampOffsetIncr_; // wrap-around
				if (iSamplerRtn < 0 || timeStampAdjRtn >= timeStampAdj) {
					iSamplerRtn = iSampler;
					timeStampAdjRtn = timeStampAdj;
				}
			}
		}
		if (iSamplerRtn < 0) break; // no more events
		int iRawEvent = iRawEventTbl_[iSamplerRtn];
		iRawEventTbl_[iSamplerRtn]++;
		const RawEvent& rawEvent = GetRawEvent(iSamplerRtn, iRawEvent);
		if (iRawEvent > 0 && rawEvent.GetTimeStamp(nBitsPinBitmap_) == 0) {
			timeStampOffsetTbl_[iSamplerRtn] += timeStampOffsetIncr_; // wrap-around
		}
		if (firstFlag_ || rawEvent.GetPinBitmap(nBitsPinBitmap_) != pinBitmapPrev_) {
			if (piSampler) *piSampler = iSamplerRtn;
			return &rawEvent;
		}
	}
	return nullptr;
}

const LogicAnalyzer::RawEvent& LogicAnalyzer::EventIterator::GetRawEvent(int iSampler, int iRawEvent)
{
	if (logicAnalyzer_.IsRawEventFormatShort()) {
		rawEvent_Short_.SetEntity(logicAnalyzer_.GetSampler(iSampler).GetSamplingBuff() + iRawEvent * sizeof(RawEvent_Short::Entity));
		return rawEvent_Short_;
	} else {
		rawEvent_Long_.SetEntity(logicAnalyzer_.GetSampler(iSampler).GetSamplingBuff() + iRawEvent * sizeof(RawEvent_Long::Entity));
		return rawEvent_Long_;
	}
}

//------------------------------------------------------------------------------
// LogicAnalyzer::SUMPAdapter
// https://firmware.buspirate.com/binmode-reference/protocol-sump
// http://dangerousprototypes.com/docs/The_Logic_Sniffer%27s_extended_SUMP_protocol
//------------------------------------------------------------------------------
LogicAnalyzer::SUMPAdapter::SUMPAdapter(LogicAnalyzer& logicAnalyzer, Stream& stream) : logicAnalyzer_(logicAnalyzer), stream_(stream)
{}

void LogicAnalyzer::SUMPAdapter::ProcessCommand(uint8_t cmd, uint32_t arg)
{
	if (cmd == Command::Reset) {
		//Run->Disable();
	} else if (cmd == Command::Run) {
		RunCapture();
	} else if (cmd == Command::ID) {
		stream_.Print("1ALS").Flush();
	} else if (cmd == Command::GetMetadata) {
		SendMeta_String(TokenKey::DeviceName,			PICO_PROGRAM_NAME);
		SendMeta_String(TokenKey::FirmwareVersion,		PICO_PROGRAM_VERSION_STRING);
		SendMeta_32bit(TokenKey::SampleMemory,			2048);
		SendMeta_32bit(TokenKey::SampleRate,			static_cast<uint32_t>(logicAnalyzer_.GetSampleRate()));
		SendMeta_32bit(TokenKey::ProtocolVersion,		2);
		SendMeta_32bit(TokenKey::NumberOfProbes,		8);
		SendMeta(TokenKey::EndOfMetadata);
		Flush();
	} else if ((cmd & 0xf3) == Command::SetTriggerMask) {
		int iStage = (cmd & 0x0c) >> 2;
		cfg_.trigger[iStage].mask = arg;
	} else if ((cmd & 0xf3) == Command::SetTriggerValues) {
		int iStage = (cmd & 0x0c) >> 2;
		cfg_.trigger[iStage].value = arg;
	} else if ((cmd & 0xf3) == Command::SetTriggerConfig) {
		int iStage = (cmd & 0x0c) >> 2;
		cfg_.trigger[iStage].config = arg;
	} else if (cmd == Command::SetDivider) {
		cfg_.divider = arg & 0x00ffffff;
	} else if (cmd == Command::SetReadDelayCount) {
		cfg_.delayCount = 4 * (((arg >> 16) & 0xffff) + 1);
		cfg_.readCount = 4 * ((arg & 0xffff) + 1);
	} else if (cmd == Command::SetFlags) {
		cfg_.flags = arg;
	} else {
		//::printf("Unknown command: 0x%02x, arg:0x%08x\n", cmd, arg);
	}
}

void LogicAnalyzer::SUMPAdapter::SendValue(uint32_t value)
{
	// send in big-endian order
	stream_.PutByte((value >> 24) & 0xff);
	stream_.PutByte((value >> 16) & 0xff);
	stream_.PutByte((value >> 8) & 0xff);
	stream_.PutByte(value & 0xff);
}

void LogicAnalyzer::SUMPAdapter::SendMeta(uint8_t tokenKey)
{
	stream_.PutByte(tokenKey);
}

void LogicAnalyzer::SUMPAdapter::SendMeta_String(uint8_t tokenKey, const char* str)
{
	stream_.PutByte(tokenKey);
	stream_.PrintRaw(str);
	stream_.PutByte(0);
}

void LogicAnalyzer::SUMPAdapter::SendMeta_32bit(uint8_t tokenKey, uint32_t value)
{
	stream_.PutByte(tokenKey);
	SendValue(value);
}

void LogicAnalyzer::SUMPAdapter::RunCapture()
{
	double samplePeriod = 1. / (logicAnalyzer_.GetSampleRate() / (cfg_.divider + 1) * 10);
	logicAnalyzer_.Enable();
	absolute_time_t elapsedTime = ::make_timeout_time_ms(samplePeriod * cfg_.readCount * 1000);
	while (!::time_reached(elapsedTime) && !logicAnalyzer_.IsRawEventFull()) {
		Tickable::TickSub();
	}
#if 0
	for (int iSignalReport = nSignalReports - 1; iSignalReport >= 0; --iSignalReport) {
		SignalReport& signalReport = signalReportTbl[iSignalReport];
		signalReport.sigBitmap
	}
	uint8_t buff[2048];
	for (int i = 0; i < cfg_.readCount; ++i) {
		buff[i] = 1 << (i % 8); // fill with some data		
	}
	for (int j = cfg_.readCount - 1; j >= 0; --j) {
		stream_.PutByte(buff[j]);
	}
#endif
	Flush();
}

void LogicAnalyzer::SUMPAdapter::OnTick()
{
	uint8_t data;
	if (stream_.Read(&data, 1) == 0) {
		// nothing to do
	} else if (comm_.stat == Stat::Cmd) {
		comm_.cmd = data;
		if (comm_.cmd & 0x80) {
			comm_.byteArg = 0;
			comm_.arg = 0;
			comm_.stat = Stat::Arg;
		} else {
			ProcessCommand(comm_.cmd, 0);
		}
	} else if (comm_.stat == Stat::Arg) {
		// receive in little-endian order
		comm_.arg = comm_.arg | (static_cast<uint32_t>(data) << (comm_.byteArg * 8));
		//comm_.arg = (comm_.arg << 8) | data;
		comm_.byteArg++;
		if (comm_.byteArg == 4) {
			ProcessCommand(comm_.cmd, comm_.arg);
			comm_.stat = Stat::Cmd;
		}
	}
}

//------------------------------------------------------------------------------
// LogicAnalyzer::SUMPAdapter::Config
//------------------------------------------------------------------------------
void LogicAnalyzer::SUMPAdapter::Config::Print(Printable& tout) const
{
	for (int iStage = 0; iStage < count_of(trigger); ++iStage) {
		tout.Printf("Trigger Stage#%d\n", iStage);
		tout.Printf("  Mask:                    0x%08x\n", trigger[iStage].mask);
		tout.Printf("  Value:                   0x%08x\n", trigger[iStage].value);
		tout.Printf("  Delay:                   %d\n", GetTrigger_Delay(iStage));
		tout.Printf("  Level:                   %d\n", GetTrigger_Level(iStage));
		tout.Printf("  Channel:                 %d\n", GetTrigger_Channel(iStage));
		tout.Printf("  Serial:                  %d\n", GetTrigger_Serial(iStage));
		tout.Printf("  Start:                   %d\n", GetTrigger_Start(iStage));
	}
	tout.Printf("Divider:                   %d\n", divider);
	tout.Printf("Delay Count:               %d\n", delayCount);
	tout.Printf("Read Count:                %d\n", readCount);
	tout.Printf("Demux Mode:                %d\n", GetFlags_DemuxMode());
	tout.Printf("Noise Filter:              %d\n", GetFlags_NoiseFilter());
	tout.Printf("Disable Channel Groups1:   %d\n", GetFlags_DisableChannelGroup1());
	tout.Printf("Disable Channel Groups2:   %d\n", GetFlags_DisableChannelGroup2());
	tout.Printf("Disable Channel Groups3:   %d\n", GetFlags_DisableChannelGroup3());
	tout.Printf("Disable Channel Groups4:   %d\n", GetFlags_DisableChannelGroup4());
	tout.Printf("External Clock:            %d\n", GetFlags_ExternalClock());
	tout.Printf("Inv External Clock:        %d\n", GetFlags_InvExternalClock());
	tout.Printf("Run Length Encoding:       %d\n", GetFlags_RunLengthEncoding());
	tout.Printf("Swap Channels:             %d\n", GetFlags_SwapChannels());
	tout.Printf("External Test Mode:        %d\n", GetFlags_ExternalTestMode());
	tout.Printf("Internal Test Mode:        %d\n", GetFlags_InternalTestMode());
	tout.Printf("Run Length Encoding Mode0: %d\n", GetFlags_RunLengthEncodingMode0());
	tout.Printf("Run Length Encoding Mode1: %d\n", GetFlags_RunLengthEncodingMode1());
}

}
