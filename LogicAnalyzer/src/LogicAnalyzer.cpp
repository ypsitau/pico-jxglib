//==============================================================================
// LogicAnalyzer.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"
#include "jxglib/BinaryInfo.h"

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
				//::gpio_disable_pulls(pin);
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

const LogicAnalyzer& LogicAnalyzer::PrintWave(Printable& tout, Printable& terr) const
{
	if (pProtocolDecoder_ && !pProtocolDecoder_->CheckValidity(terr)) return *this;
	int iCol = 0;
	char buffLine[256];
	EventIterator eventIter(*this);
	int nEventsRelevant = eventIter.CountRelevant();
	if (nEventsRelevant == 0) {
		tout.Printf("no events to print\n");
		return *this;
	}
	//for (int iSampler = 0; iSampler < nSampler_; ++iSampler) samplerTbl_[iSampler].DumpSamplingBuff(tout);
	const char* strBlank = "    ";
	const WaveStyle& waveStyle = *printInfo_.pWaveStyle;
	auto flushLine = [&]() {
		tout.Println(CutTrailingSpace(buffLine));
		iCol = 0;
	};
	auto flushLineWithEvent = [&](const Event& event) {
		if (pProtocolDecoder_) pProtocolDecoder_->AnnotateWaveEvent(eventIter, event, buffLine, sizeof(buffLine), &iCol);
		flushLine();
	};
	auto flushLineWithStreak = [&]() {
		if (pProtocolDecoder_) pProtocolDecoder_->AnnotateWaveStreak(buffLine, sizeof(buffLine), &iCol);
		flushLine();
	};
	auto printHeader = [&]() {
		iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12s ", "Time [usec]");
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (IsBlankPin(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", strBlank);
			} else if (IsValidPin(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, waveStyle.formatHeader, pin);
			}
		}
		flushLine();
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
		iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12s", "");
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (IsBlankPin(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", strBlank);
			} else if (IsAnnotationPin(pin)) {
				// annotation jobs
			} else if (samplingInfo_.IsPinEnabled(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", eventInitial.IsPinHigh(pin)? waveStyle.strHigh : waveStyle.strLow);
			} else {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", waveStyle.strBlank);
			}
		}
		flushLineWithEvent(eventInitial);
	}
	Event eventPrev = eventInitial;
	Event event = eventBase;
	double timeStampFactor = 1000'000. / GetSampleRate() / nSampler_;
	bool contFlag = true;
	for (int iEvent = 0; iEvent < nEventsToPrint && contFlag; ++iEvent, contFlag = eventIter.Next(event)) {
		double delta = static_cast<double>(event.GetTimeStamp() - eventPrev.GetTimeStamp()) * timeStampFactor;
		int nDelta = static_cast<int>(delta / usecReso_);
		if (iEvent == 0 || nDelta >= 40) {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12s", "");
			for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
				uint pin = printInfo_.pinTbl[iPin];
				if (IsBlankPin(pin)) {
					iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", strBlank);
				} else if (IsAnnotationPin(pin)) {
					// annotation jobs
				} else if (samplingInfo_.IsPinEnabled(pin)) {
					iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", eventPrev.IsPinHigh(pin)? waveStyle.strHighIdle : waveStyle.strLowIdle);
				} else {
					iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", waveStyle.strBlank);
				}
			}
			flushLineWithStreak();
		} else {
			for (int i = 0; i < nDelta; ++i) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12s", "");
				for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
					uint pin = printInfo_.pinTbl[iPin];
					if (IsBlankPin(pin)) {
						iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", strBlank);
					} else if (IsAnnotationPin(pin)) {
						// annotation jobs
					} else if (samplingInfo_.IsPinEnabled(pin)) {
						iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", eventPrev.IsPinHigh(pin)? waveStyle.strHigh : waveStyle.strLow);
					} else {
						iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", waveStyle.strBlank);
					}
				}
				flushLineWithStreak();
			}
		}
		uint32_t bitsTransition = event.GetPinBitmap() ^ eventPrev.GetPinBitmap();
		iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12.2f", static_cast<double>(event.GetTimeStamp() - eventBase.GetTimeStamp()) * timeStampFactor);
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (IsBlankPin(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", strBlank);
			} else if (IsAnnotationPin(pin)) {
				// annotation jobs
			} else if (samplingInfo_.IsPinEnabled(pin)) {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", (bitsTransition & (1u << pin))?
					(event.IsPinHigh(pin)? waveStyle.strLowToHigh : waveStyle.strHighToLow) :
					(event.IsPinHigh(pin)? waveStyle.strHigh : waveStyle.strLow));
			} else {
				iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%s", waveStyle.strBlank);
			}
		}
		flushLineWithEvent(event);
		if (Tickable::TickSub()) break;
		eventPrev = event;
	}
	if (nEventsToPrint > 0) printHeader();
	return *this;
}

ProtocolDecoder* LogicAnalyzer::SetProtocolDecoder(const char* protocolName)
{
	if (pProtocolDecoder_ && ::strcasecmp(pProtocolDecoder_->GetName(), protocolName) == 0) {
		// nothing to do
	} else {
		pProtocolDecoder_.reset();
		ProtocolDecoder::Factory* pFactory = ProtocolDecoder::Factory::Find(protocolName);
		if (pFactory) {
			pProtocolDecoder_.reset(pFactory->Create(*this));
		}
	}
	return pProtocolDecoder_.get();
}

// This method destroys the sampling buffers and creates a set of SignalReport. 
const LogicAnalyzer::SignalReport* LogicAnalyzer::CreateSignalReport(int nSamples, double samplePeriod, int* pnSignalReports)
{
	SignalReport* signalReportTbl = reinterpret_cast<SignalReport*>(GetSamplingBuffWhole());
	*pnSignalReports = 0;
	EventIterator eventIter(*this);
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

const LogicAnalyzer& LogicAnalyzer::PlotWave() const
{
	if (!pTelePlot_) return *this;
	TelePlot& tp = *pTelePlot_;
	EventIterator eventIter(*this);
	int nEventsRelevant = eventIter.CountRelevant();
	if (nEventsRelevant == 0) return *this;
	double timeStampFactor = 1000'000. / GetSampleRate() / nSampler_;
	Event eventInitial, eventBase;
	eventIter.Next(eventInitial);
	if (!eventIter.Next(eventBase)) eventBase = eventInitial;
	Event eventPrev = eventBase;
	Event event = eventBase;
	TelePlot::Telemetry t = tp.Add("GPIO1");
	bool contFlag = true;
	for (int iEvent = 0; iEvent < 50 && contFlag; ++iEvent, contFlag = eventIter.Next(event)) {
		double delta = static_cast<double>(event.GetTimeStamp() - eventPrev.GetTimeStamp()) * timeStampFactor;
		int nDelta = static_cast<int>(delta * 10 / usecReso_);
		if (nDelta > 100) nDelta = 100;
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (IsBlankPin(pin)) {
				// nothing to do
			} else if (IsAnnotationPin(pin)) {
				// annotation jobs
			} else if (samplingInfo_.IsPinEnabled(pin)) {
				float value = eventPrev.IsPinHigh(pin)? .2 : 0;
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
// LogicAnalyzer::PrintInfo
//------------------------------------------------------------------------------
int LogicAnalyzer::PrintInfo::CountValidPins() const
{
	int nValidPins = 0;
	for (int i = 0; i < nPins; ++i) if (IsValidPin(pinTbl[i])) nValidPins++;
	return nValidPins;
}

//------------------------------------------------------------------------------
// LogicAnalyzer::SamplingInfo
//------------------------------------------------------------------------------
void LogicAnalyzer::SamplingInfo::Update(const PrintInfo& printInfo)
{
	pinBitmapEnabled_ = 0;
	pinMin_ = GPIO::NumPins;
	nPins_ = printInfo.nPins;
	for (int i = 0; i < printInfo.nPins; ++i) {
		uint pin = printInfo.pinTbl[i];
		if (IsValidPin(pin)) {
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
// LogicAnalyzer::Event
//------------------------------------------------------------------------------
const LogicAnalyzer::Event LogicAnalyzer::Event::None;

LogicAnalyzer::Event& LogicAnalyzer::Event::operator=(const Event& event)
{
	if (this != &event) {
		timeStamp_ = event.timeStamp_;
		pinBitmap_ = event.pinBitmap_;
	}
	return *this;
}

uint32_t LogicAnalyzer::Event::GetPackedBitmap(const PrintInfo& printInfo) const
{
	int iBit = 0;
	uint32_t bitmap = 0;
	for (int iPin = 0; iPin < printInfo.nPins; ++iPin) {
		uint pin = printInfo.pinTbl[iPin];
		if (IsValidPin(pin)) {
			if (IsPinHigh(pin)) bitmap |= (1u << iBit);
			iBit++;
		}
	}
#if 0
	int iBit = 0;
	uint pin = samplingInfo.GetPinMin();
	for (uint32_t pinBitmap = samplingInfo.GetPinBitmapEnabled(); pinBitmap != 0; pinBitmap >>= 1, ++pin) {
		if (pinBitmap & 1) {
			if (IsPinHigh(pin)) bitmap |= 1u << iBit;
			iBit++;
		}
	}
#endif
	return bitmap;
}

//------------------------------------------------------------------------------
// LogicAnalyzer::EventIterator
//------------------------------------------------------------------------------
LogicAnalyzer::EventIterator::EventIterator(const EventIterator& eventIter) :
	logicAnalyzer_{eventIter.logicAnalyzer_},
	pinMin_{eventIter.pinMin_},
	nBitsPinBitmap_{eventIter.nBitsPinBitmap_},
	iRawEventTbl_{},
	timeStampOffsetTbl_{},
	pinBitmapPrev_{eventIter.pinBitmapPrev_},
	firstFlag_{eventIter.firstFlag_},
	timeStampOffsetIncr_{eventIter.timeStampOffsetIncr_}
{
	::memcpy(iRawEventTbl_, eventIter.iRawEventTbl_, sizeof(iRawEventTbl_));
	::memcpy(timeStampOffsetTbl_, eventIter.timeStampOffsetTbl_, sizeof(timeStampOffsetTbl_));
}

LogicAnalyzer::EventIterator::EventIterator(const LogicAnalyzer& logicAnalyzer) :
	logicAnalyzer_{logicAnalyzer}, pinMin_{0}, nBitsPinBitmap_{0},
	pinBitmapPrev_{0}, firstFlag_{true}, timeStampOffsetIncr_{0}
{
	Rewind();
}

void LogicAnalyzer::EventIterator::Rewind()
{
	pinMin_ = logicAnalyzer_.GetSamplingInfo().GetPinMin();
	nBitsPinBitmap_ = logicAnalyzer_.GetSamplingInfo().CountBits();
	int nBitsTimeStamp = 32 - nBitsPinBitmap_;
	timeStampOffsetIncr_ = 1LL << nBitsTimeStamp;
	for (int iSampler = 0; iSampler < logicAnalyzer_.GetSamplerCount(); ++iSampler) {
		iRawEventTbl_[iSampler] = 0;
		timeStampOffsetTbl_[iSampler] = 0;
	}
	pinBitmapPrev_ = 0;
	firstFlag_= true;
}

bool LogicAnalyzer::EventIterator::HasMore() const
{
	for (int iSampler = 0; iSampler < logicAnalyzer_.GetSamplerCount(); ++iSampler) {
		if (iRawEventTbl_[iSampler] < logicAnalyzer_.GetRawEventCount(iSampler)) return true; // at least one sampler has more events
	}
	return false;
}

bool LogicAnalyzer::EventIterator::Next(Event& event)
{
	int iSampler;
	const RawEvent* pRawEvent = NextRawEvent(&iSampler);
	if (!pRawEvent) return false;
	uint64_t timeStamp = (timeStampOffsetTbl_[iSampler] + pRawEvent->GetTimeStamp(nBitsPinBitmap_)) * logicAnalyzer_.GetSamplerCount() + iSampler;
	uint32_t pinBitmap = pRawEvent->GetPinBitmap(nBitsPinBitmap_) << pinMin_;
	event = Event(timeStamp, pinBitmap);
	pinBitmapPrev_ = pinBitmap;
	firstFlag_= false;
	return true;
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
		if (firstFlag_ || (rawEvent.GetPinBitmap(nBitsPinBitmap_) << pinMin_) != pinBitmapPrev_) {
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
// LogicAnalyzer::SigrokAdapter
// see git://sigrok.org/libsigrok.git
//   /src/hardware/raspberrypi-pico/protocol.c
//------------------------------------------------------------------------------
LogicAnalyzer::SigrokAdapter::SigrokAdapter(LogicAnalyzer& logicAnalyzer, Printable& terr, Stream& streamApplication) :
	logicAnalyzer_{logicAnalyzer}, eventIter_{logicAnalyzer_}, pTerr_{&terr}, stream_{streamApplication}, stat_{Stat::Initial},
	nDigitalChAvailable_{0}, nAnalogChAvailable_{0}, nDigitalChEnabled_{0}, nAnalogChEnabled_{0},
	digitalChBitmapEnabled_{0}, analogChBitmapEnabled_{0}, uvoltScale_{0}, uvoltOffset_{0},
	enableChannelFlag_{false}, iChannel_{0}, sampleRate_{1}, nSamples_{0}, timeStampFactor_{1.0}, sampleDelta_{1.0}, iEvent_{0}
{
}

void LogicAnalyzer::SigrokAdapter::StartSampling()
{
	eventIter_.Rewind();
	event_.Invalidate();
	timeStampFactor_ = 1. / (static_cast<double>(logicAnalyzer_.GetSampleRate()) * logicAnalyzer_.GetSamplerCount());
	sampleDelta_ = (sampleRate_ == 0)? 1. : 1. / static_cast<double>(sampleRate_);
	iEvent_ = 0;
	embeddedRLEData_ = 0x00;
}

void LogicAnalyzer::SigrokAdapter::OnTick()
{
	Printable& terr = *pTerr_;
	char ch;
	bool pollingFlag = (stream_.Read(&ch, 1) == 0);
	//::printf("%c\n", ch);
	switch (stat_) {
	case Stat::Initial: {
		if (pollingFlag) {
			// nothing to do
		} else if (ch == '*') {		// Reset
			Reset();
		} else if (ch == '+') {		// Abort
			// nothing to do
		} else if (ch == 'i') {		// Identify
			stat_ = Stat::Identify;
		} else if (ch == 'a') {		// Analog Scale and Offset
			stat_ = Stat::AnalogScaleAndOffset;
		} else if (ch == 'R') {		// Sample rate
			sampleRate_ = 0;
			stat_ = Stat::SampleRate;
		} else if (ch == 'L') {		// Sample limit
			nSamples_ = 0;
			stat_ = Stat::SampleLimit;
		} else if (ch == 'A') {		// Analog Channel Enable
			stat_ = Stat::AnalogChannelEnable;
		} else if (ch == 'D') {		// Digital Channel Enable
			stat_ = Stat::DigitalChannelEnable;
		} else if (ch == 'F') {		// Fixed Sample Mode
			//uint8_t buff[8];
			//buff[0] = 0x80 | (7 << 4) | 0x0f;
			//buff[1] = 47 + 2;
			//buff[2] = 0x80 | (3 << 4) | 0x05;
			//buff[3] = 47 + 3;
			//stream_.Write(buff, 4);
			//stream_.Flush();
			StartSampling();
			logicAnalyzer_.Enable();
			stat_ = Stat::FixedSampleMode;
		} else if (ch == 'C') {		// Continuous Sample Mode
			stat_ = Stat::ContinuousSampleMode;
		}
		break;
	}
	case Stat::Recover: {
		if (pollingFlag) {
			// nothing to do
		} else if (ch == '*') {
			Reset();
			stat_ = Stat::Initial;
		}
		break;
	}
	case Stat::Identify: {
		if (pollingFlag) {
			// nothing to do
		} else if (ch == '\n') {
			nDigitalChAvailable_ = logicAnalyzer_.GetPrintInfo().CountValidPins();
			//nDigitalChAvailable_ = logicAnalyzer_.GetSamplingInfo().CountPins();
			//nDigitalChAvailable_ = 4;
			if (nDigitalChAvailable_ == 0) {
				terr.Printf(
					"PulseView is connected, but no digital channels are enalbed.\n"
					"%d channels are assumed by default, but for proper operation,\n"
					"please run the 'la' command (e.g., la -p 2,3) to specify which channels to enable,\n"
					"and then reopen PulseView.\n", nDigitalChAvailable_);
				nDigitalChAvailable_ = nDigitalChToReportDefault_;
			}
			stream_.Printf("SRPICO,A%02d1D%02d,%02d", nAnalogChAvailable_, nDigitalChAvailable_, versionNumber_).Flush();
		}
		stat_ = Stat::Initial;
		break;
	}
	case Stat::AnalogScaleAndOffset: {
		if (pollingFlag) {
			// nothing to do
		} else if ('0' <= ch && ch <= '9') {
			int channel = ch - '0';
			if (channel >= 0 && channel < nAnalogChAvailable_) {
				stream_.Printf("%04dx%05d", uvoltScale_, uvoltOffset_);
			}
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::SampleRate: {
		if (pollingFlag) {
			// nothing to do
		} else if ('0' <= ch && ch <= '9') {
			sampleRate_ = sampleRate_ * 10 + (ch - '0');
		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::SampleLimit: {
		if (pollingFlag) {
			// nothing to do
		} else if ('0' <= ch && ch <= '9') {
			nSamples_ = nSamples_ * 10 + (ch - '0');
		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::AnalogChannelEnable: {
		if (pollingFlag) {
			// nothing to do
		} else if (ch == '0' || ch == '1') {
			enableChannelFlag_ = (ch == '1');
			iChannel_ = 0;
			stat_ = Stat::AnalogChannelEnable_Channel;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::AnalogChannelEnable_Channel: {
		if (pollingFlag) {
			// nothing to do
		} else if ('0' <= ch && ch <= '9') {
			iChannel_ = iChannel_ * 10 + (ch - '0');
		} else if (ch == '\n') {
			if (enableChannelFlag_) {
				nAnalogChEnabled_++;
				analogChBitmapEnabled_ |= (1u << iChannel_);
			} else {
				analogChBitmapEnabled_ &= ~(1u << iChannel_);
			}
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::DigitalChannelEnable: {
		if (pollingFlag) {
			// nothing to do
		} else if (ch == '0' || ch == '1') {
			enableChannelFlag_ = (ch == '1');
			iChannel_ = 0;
			stat_ = Stat::DigitalChannelEnable_Channel;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::DigitalChannelEnable_Channel: {
		if (pollingFlag) {
			// nothing to do
		} else if ('0' <= ch && ch <= '9') {
			iChannel_ = iChannel_ * 10 + (ch - '0');
		} else if (ch == '\n') {
			if (enableChannelFlag_) {
				nDigitalChEnabled_++;
				digitalChBitmapEnabled_ |= (1u << iChannel_);
			} else {
				digitalChBitmapEnabled_ &= ~(1u << iChannel_);
			}
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::FixedSampleMode: {
		if (pollingFlag) {
			Event eventNext;
			while (eventIter_.Next(eventNext)) {
				if (event_.IsValid()) {
					int nSamples = (iEvent_ == 1)? nSamplesHead_ : CountSamplesBetweenEvents(event_, eventNext);
					if (nSamples > 0) {
						SendReport(event_, nSamples);
						event_ = eventNext;
					}
				} else {
					event_ = eventNext;
				}
				iEvent_++;
			}
		} else if (ch == '+') {		// abort
			if (event_.IsValid()) {
				SendReport(event_, nSamplesTail_);
			}
			stream_.Flush();
			stat_ = Stat::Initial;
		}
		break;
	}
	case Stat::ContinuousSampleMode: {
		break;
	}
	default:
		break;
	}
}

void LogicAnalyzer::SigrokAdapter::Reset()
{
	nDigitalChEnabled_ = 0;
	nAnalogChEnabled_ = 0;
	digitalChBitmapEnabled_ = 0;
	analogChBitmapEnabled_ = 0;
}

int LogicAnalyzer::SigrokAdapter::CountSamplesBetweenEvents(const Event& event1, const Event& event2) const
{
	//double timeStamp1 = timeStampFactor_ * static_cast<double>(event1.GetTimeStamp());
	//double timeStamp2 = timeStampFactor_ * static_cast<double>(event2.GetTimeStamp());
	//int nSamples = static_cast<int>((timeStamp2 - timeStamp1) / sampleDelta_);
	//::printf("%.2f-%.2f %.2f %dsamples\n", timeStamp1 * 1000'000, timeStamp2 * 1000'000, sampleDelta_ * 1000'000, nSamples);
	int64_t iSample1 = static_cast<int64_t>(timeStampFactor_ * static_cast<double>(event1.GetTimeStamp()) / sampleDelta_);
	int64_t iSample2 = static_cast<int64_t>(timeStampFactor_ * static_cast<double>(event2.GetTimeStamp()) / sampleDelta_);
	int nSamples = static_cast<int>(iSample2 - iSample1);
	return nSamples;
}

void LogicAnalyzer::SigrokAdapter::SendReport(const Event& event, int nSamples)
{
	int iBuff = 0;
	uint8_t buff[32];
	if (nSamples <= 0) return;
	if (nSamples > 640 * (count_of(buff) - 4)) nSamples = 640 * (count_of(buff) - 4);	// prevent overflow of buff
	uint32_t packedBitmap = event.GetPackedBitmap(logicAnalyzer_.GetPrintInfo());
	//::printf("%08x * %dsamples\n", packedBitmap, nSamples);
	if (nDigitalChEnabled_ <= 4) {
		// D4 mode
		int iBit = 0;
		uint8_t data = 0;
		for (uint32_t bitmapEnabled = digitalChBitmapEnabled_; bitmapEnabled != 0; bitmapEnabled >>= 1, packedBitmap >>= 1) {
			if (bitmapEnabled & 1) {
				data |= (packedBitmap & 1) << iBit;
				iBit++;
			}
		}
		buff[iBuff++] = 0x80 | data | embeddedRLEData_;
		embeddedRLEData_ = 0x00;
		nSamples--;
		if (nSamples > 0) {
			while (nSamples >= 8 * 80) {				// 8 * 80 <= nSamples
				buff[iBuff++] = (8 * 80) / 8 + 47;		// = 127 (0x7f)
				nSamples -= 8 * 80;
			}
			if (nSamples >= 8) {
				int nChunksToSend = nSamples / 8;
				buff[iBuff++] = (8 * nChunksToSend) / 8 + 47;
				nSamples -= nChunksToSend * 8;
			}
			embeddedRLEData_ = static_cast<uint8_t>(nSamples << 4);
		}
	} else {
		// Normal mode
		int iBit = 0;
		uint8_t data = 0;
		//::printf("%032b\n", packedBitmap);
		for (uint32_t bitmapEnabled = digitalChBitmapEnabled_; bitmapEnabled != 0; bitmapEnabled >>= 1, packedBitmap >>= 1) {
			if (bitmapEnabled & 1) {
				data |= ((packedBitmap & 1) << iBit);
				iBit++;
				if (iBit == 7) {
					buff[iBuff++] = 0x80 | data;
					data = 0;
					iBit = 0;
				}
			}
		}
		if (iBit > 0) buff[iBuff++] = 0x80 | data;
		nSamples--;
		if (nSamples > 0) {
			while (nSamples >= 32 * 49) {				// 32 * 49 <= nSamples
				buff[iBuff++] = (32 * 49) / 32 + 78;	// = 127 (0x7f)
				nSamples -= 32 * 49;
			}
			if (nSamples >= 64) {						// 64 <= nSamples < 32 * 49
				int nChunksToSend = nSamples / 32;
				buff[iBuff++] = nChunksToSend + 78;
				nSamples -= 32 * nChunksToSend;
			}
			if (nSamples >= 32) {						// 32 <= nSamples < 64
				buff[iBuff++] = 32 + 47;				// = 79 (0x4f)
				nSamples -= 32;
			}
			if (nSamples > 32) nSamples = 32;
			if (nSamples > 0) {							// 0 < nSamples < 32
				buff[iBuff++] = nSamples + 47;
			}
		}
	}
	//Dump(buff, iBuff);
	stream_.Write(buff, iBuff);
	//stream_.Flush();
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
		SendMeta_32bit(TokenKey::SampleMemory,			1024);
		SendMeta_32bit(TokenKey::SampleRate,			static_cast<uint32_t>(logicAnalyzer_.GetSampleRate()));
		SendMeta_32bit(TokenKey::ProtocolVersion,		2);
		SendMeta_32bit(TokenKey::NumberOfProbes,		16); //logicAnalyzer_.GetPrintInfo().nPins);
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
	Tickable::Sleep(1000);
	::printf("%d samples\n", cfg_.readCount);
	for (int i = 0; i < cfg_.readCount; ++i) {
		stream_.PutByte(1 << (i % 8));		// Probe 0-7
		stream_.PutByte(~(1 << (i % 8)));	// Probe 8-15
	}
#if 0
	for (int i = 0; i < cfg_.readCount; ++i) {
		stream_.PutByte(1 << (i % 8));	// Probe 0-7
		stream_.PutByte(0x00);			// Probe 8-15
		stream_.PutByte(0xff);			// Probe 16-23
		stream_.PutByte(0xaa);			// Probe 24-31
	}
#endif
	Flush();
#if 0
	::printf("start capture\n");
	double samplePeriod = 1. / (logicAnalyzer_.GetSampleRate() / (cfg_.divider + 1) * 10);
	logicAnalyzer_.Enable();
	//absolute_time_t elapsedTime = ::make_timeout_time_ms(samplePeriod * cfg_.readCount * 1000);
	absolute_time_t elapsedTime = ::make_timeout_time_ms(1'000);
	while (!::time_reached(elapsedTime) && !logicAnalyzer_.IsRawEventFull()) {
	//while (!logicAnalyzer_.IsRawEventFull()) {
		Tickable::TickSub();
	}
	int nSignalReports = 0;
	const SignalReport* signalReportTbl = logicAnalyzer_.CreateSignalReport(cfg_.readCount, samplePeriod, &nSignalReports);
	::printf("end capture: %d reports\n", nSignalReports);
#if 0
	for (int iSignalReport = nSignalReports - 1; iSignalReport >= 0; --iSignalReport) {
		const SignalReport& signalReport = signalReportTbl[iSignalReport];
		for (int i = 0; i < signalReport.nSamples; ++i) {
			::printf("%02x\n", signalReport.sigBitmap);
			stream_.PutByte(static_cast<uint8_t>(signalReport.sigBitmap));
		}
	}
#endif
	for (int i = 0; i < cfg_.readCount / 2; ++i) {
		stream_.PutByte(0x00);
		stream_.PutByte(0xff);
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
	tout.Printf("Run Length Encoding Mode:  %d\n", GetFlags_RunLengthEncodingMode());
}

}
