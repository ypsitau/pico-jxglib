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
		samplingBuffWhole_{nullptr}, iPIORequested_{static_cast<uint>(-1)},
		nSampler_{1}, pinTargetGlobal_{PinTarget::Internal}, heapRatio_{.7}, heapRatioRequested_{.7}, usecReso_{1'000},
		suppressPrintSettingsFlag_{false}
{
	for (uint pin = 0; pin < count_of(pinTargetTbl_); ++pin) pinTargetTbl_[pin] = PinTarget::Inherited;
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
		samplingBuffWhole_ = reinterpret_cast<uint8_t*>(::malloc(nSampler_ * bytesSamplingBuffPerSampler));
		if (!samplingBuffWhole_) return false;
	}
	heapRatio_ = heapRatioRequested_;
	for (int iSampler = 0; iSampler < nSampler_; ++iSampler) {
		samplerTbl_[iSampler].AssignBuff(samplingBuffWhole_ + iSampler * bytesSamplingBuffPerSampler, bytesSamplingBuffPerSampler);
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
	uint iPIO = iPIORequested_;
	if (iPIO == static_cast<uint>(-1)) {
		iPIO = PIO::Num - 1;
		int i = 0;
		for (i = 0; i < PIO::Num; i++, iPIO--) if (PIO::Block(iPIO).is_vacant()) break;
		if (i == PIO::Num) return false; // all PIO blocks are in use
	}
	pio_hw_t* pio = ::pio_get_instance(iPIO);
	samplerTbl_[0].SetProgram(program_SamplerMain_, pio, 0, relAddrEntryTbl[0], samplingInfo_.GetPinMin(), nBitsPinToSample);
	for (int iSampler = 1; iSampler < nSampler_; ++iSampler) {
		samplerTbl_[iSampler].ShareProgram(samplerTbl_[0], pio, iSampler, relAddrEntryTbl[iSampler], samplingInfo_.GetPinMin(), nBitsPinToSample);
	}
	uint pin = samplingInfo_.GetPinMin();
	for (uint32_t pinBitmap = samplingInfo_.GetPinBitmapEnabled(); pinBitmap != 0; pinBitmap >>= 1, ++pin) {
		if ((pinBitmap & 1) && GetPinTarget(pin) == PinTarget::External) {
			::pio_gpio_init(samplerTbl_[0].GetSM().pio, pin);
			//::gpio_disable_pulls(pin);
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
	uint pin = samplingInfo_.GetPinMin();
	for (uint32_t pinBitmap = samplingInfo_.GetPinBitmapEnabled(); pinBitmap != 0; pinBitmap >>= 1, ++pin) {
		if ((pinBitmap & 1) && GetPinTarget(pin) == PinTarget::External) {
			::gpio_set_dir(pin, GPIO_IN);
			::gpio_put(pin, 0);
			::gpio_pull_down(pin);
			::gpio_set_function(pin, GPIO_FUNC_NULL);
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

LogicAnalyzer::PinTarget LogicAnalyzer::GetPinTarget(uint pin) const
{
	PinTarget pinTarget = pinTargetTbl_[pin];
	return (pinTarget == PinTarget::Inherited)? pinTargetGlobal_ : pinTarget;
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

const LogicAnalyzer& LogicAnalyzer::PrintWave(Printable& tout, Printable& terr, bool liveFlag) const
{
	if (pDecoder_) {
		if (!pDecoder_->CheckValidity(terr)) return *this;
		pDecoder_->Reset();
	}
	int iCol = 0;
	char buffLine[256];
	EventIterator eventIter(*this);
	//for (int iSampler = 0; iSampler < nSampler_; ++iSampler) samplerTbl_[iSampler].DumpSamplingBuff(tout);
	const char* strBlank = "    ";
	const WaveStyle& waveStyle = *printInfo_.pWaveStyle;
	auto flushLine = [&]() {
		tout.Println(CutTrailingSpace(buffLine));
		iCol = 0;
	};
	auto consumeEvent = [&](const Event& event) {
		if (pDecoder_) {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, " ");
			pDecoder_->AnnotateWaveEvent(eventIter, event, buffLine, sizeof(buffLine), &iCol);
		}
		iCol = 0;
	};
	auto flushLineWithEvent = [&](const Event& event) {
		if (pDecoder_) {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, " ");
			pDecoder_->AnnotateWaveEvent(eventIter, event, buffLine, sizeof(buffLine), &iCol);
		}
		//::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "|");
		flushLine();
	};
	auto flushLineWithStreak = [&](double timeStamp) {
		if (pDecoder_) {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, " ");
			pDecoder_->AnnotateWaveStreak(timeStamp, buffLine, sizeof(buffLine), &iCol);
		}
		//::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "|");
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
	int nEventsRelevant = eventIter.CountRelevant();
	if (!liveFlag && nEventsRelevant == 0) {
		tout.Printf("no events to print\n");
		return *this;
	}
	double timeStampFactor = 1000'000. / GetSampleRate();
	Event event, eventBase, eventPrev;
	printHeader();
	for (int iEvent = 0; ; ++iEvent) {
		if (liveFlag) {
			while (!eventIter.Next(event)) if (Tickable::TickSub()) goto done;
		} else if (printInfo_.part == PrintPart::Head) {
			if (iEvent >= ChooseMin(nEventsRelevant, printInfo_.nEventsToPrint)) break;
			if (!eventIter.Next(event)) break;
		} else if (printInfo_.part == PrintPart::Tail) {
			if (!eventIter.Next(event)) break;
			if (iEvent + printInfo_.nEventsToPrint < nEventsRelevant) {
				if (iEvent == 1) eventBase = event;
				consumeEvent(event);
				eventPrev = event;
				continue;
			}
		} else { // printInfo_.part == PrintPart::All
			if (!eventIter.Next(event)) break;
		}
		if (iEvent == 1) eventBase = event;
		uint32_t bitsTransition = 0;
		if (eventPrev.IsValid()) {
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
				flushLine();
			} else {
				double timeStampPrev = static_cast<double>(eventPrev.GetTimeStamp()) * timeStampFactor;
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
					flushLineWithStreak(timeStampPrev + delta * (i + 1) / (nDelta + 1));
				}
			}
			bitsTransition = event.GetPinBitmap() ^ eventPrev.GetPinBitmap();
		}
		if (eventBase.IsValid()) {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12.2f", static_cast<double>(event.GetTimeStamp() - eventBase.GetTimeStamp()) * timeStampFactor);
		} else {
			iCol += ::snprintf(buffLine + iCol, sizeof(buffLine) - iCol, "%12s", "");
		}
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
done:
	printHeader();
	return *this;
}

const LogicAnalyzer& LogicAnalyzer::WriteFileJSON(Printable& tout) const
{
	EventIterator eventIter(*this);
	Event event;
	tout.Printf("{\"sample-rate\":%f, \"events\":[", GetSampleRate());
	for (int iEvent = 0; eventIter.Next(event); ++iEvent) {
		tout.Printf("%s [%llu,%u]", (iEvent == 0)? "\n" : ",\n", event.GetTimeStamp(), event.GetPinBitmap());
		if (Tickable::TickSub()) break;
	}
	tout.Printf("\n]}\n");
	return *this;
}

LogicAnalyzer::Decoder* LogicAnalyzer::SetDecoder(const char* decoderName)
{
	if (pDecoder_ && ::strcasecmp(pDecoder_->GetName(), decoderName) == 0) {
		// nothing to do
	} else {
		pDecoder_.reset();
		Decoder::Factory* pFactory = Decoder::Factory::Find(decoderName);
		if (pFactory) {
			pDecoder_.reset(pFactory->Create(*this));
		}
	}
	return pDecoder_.get();
}

const LogicAnalyzer& LogicAnalyzer::PrintSettings(Printable& tout) const
{
	if (suppressPrintSettingsFlag_) return *this;
	static const char* markExternal = "`";
	auto printPinSequence = [&](bool firstFlag, uint pinStart, uint pinEnd) {
		if (pinEnd - pinStart < 2) {
			for (uint pin = pinStart; pin <= pinEnd; ++pin) {
				tout.Printf(firstFlag? "%s%d" : ",%s%d",
					(GetPinTarget(pin) == PinTarget::External)? markExternal : "", pin);
				firstFlag = false;
			}
		} else {
			tout.Printf(firstFlag? "%s%d-%s%d" : ",%s%d-%s%d",
				(GetPinTarget(pinStart) == PinTarget::External)? markExternal : "", pinStart,
				(GetPinTarget(pinEnd) == PinTarget::External)? markExternal : "", pinEnd);
			firstFlag = false;
		}
	};
	int nRawEvents = GetRawEventCount();
	if (samplingInfo_.IsEnabled()) {
		const PIO::StateMachine& sm = samplerTbl_[0].GetSM();
		tout.Printf("enabled%s pio:%d", samplerTbl_[0].IsFull()? "(full)" : "", sm.pio.get_index());
	} else {
		tout.Printf("disabled ----");
	}
	tout.Printf(" %.1fMHz (samplers:%d)", GetSampleRate() / 1000'000., nSampler_);
	do {
		bool firstFlag = true;
		tout.Printf(" pins:");
		uint pinStart = GPIO::InvalidPin;
		uint pinEnd = GPIO::InvalidPin;
		PinTarget pinTargetPrev = PinTarget::Inherited;
		for (uint pin = 0; pin < GPIO::NumPins; ++pin) {
			PinTarget pinTarget = GetPinTarget(pin);
			if (samplingInfo_.IsPinEnabled(pin)) {
				if (pinStart == GPIO::InvalidPin) {
					pinStart = pinEnd = pin;
				} else if (pin == pinEnd + 1 && pinTarget == pinTargetPrev) {
					pinEnd = pin;
				} else {
					printPinSequence(firstFlag, pinStart, pinEnd);
					pinStart = pinEnd = pin;
				}
			} else if (pinStart != GPIO::InvalidPin) {
				printPinSequence(firstFlag, pinStart, pinEnd);
				pinStart = pinEnd = GPIO::InvalidPin;
				firstFlag = false;
			}
			pinTargetPrev = pinTarget;
		}
		if (pinStart != GPIO::InvalidPin) {
			printPinSequence(firstFlag, pinStart, pinEnd);
		} else if (firstFlag) {
			tout.Print("none");
		}
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
		++nEvent;
		if (pRawEvent->GetTimeStamp(nBitsPinBitmap_) != 0) nEventRtn = nEvent;
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
	logicAnalyzer_{logicAnalyzer}, eventIter_{logicAnalyzer_}, pTerr_{&terr}, pStream_{&streamApplication}, stat_{Stat::Initial},
	nDigitalChAvailable_{0}, nAnalogChAvailable_{0}, nDigitalChEnabled_{0}, nAnalogChEnabled_{0},
	digitalChBitmapEnabled_{0}, analogChBitmapEnabled_{0}, uvoltScale_{0}, uvoltOffset_{0},
	enableChannelFlag_{false}, iChannel_{0}, sampleRate_{1}, nSamples_{0}, timeStampFactor_{1.0}, sampleDelta_{1.0}, iEvent_{0}
{
}

void LogicAnalyzer::SigrokAdapter::StartSampling()
{
	eventIter_.Rewind();
	event_.Invalidate();
	timeStampFactor_ = 1. / logicAnalyzer_.GetSampleRate();
	sampleDelta_ = (sampleRate_ == 0)? 1. : 1. / static_cast<double>(sampleRate_);
	iEvent_ = 0;
	embeddedRLEData_ = 0x00;
}

void LogicAnalyzer::SigrokAdapter::OnTick()
{
	Printable& terr = *pTerr_;
	char ch;
	bool pollingFlag = (GetStream().Read(&ch, 1) == 0);
	//if (!pollingFlag) ::printf("Command: %c\n", ch);
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
			StartSampling();
			if (logicAnalyzer_.Enable()) {
				//logicAnalyzer_.PrintSettings(terr);
				stat_ = Stat::FixedSampleMode;
			} else {
				terr.Printf("failed to enable logic analyzer\n");
			}
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
			//terr.Printf("PulseView/sigrok-cli connected%s\n", (nDigitalChAvailable_ == 0) ? " (no digital channels enabled)" : "");
			if (nDigitalChAvailable_ == 0) {
				nDigitalChAvailable_ = nDigitalChToReportDefault_;
			}
			GetStream().Printf("SRPICO,A%02d1D%02d,%02d", nAnalogChAvailable_, nDigitalChAvailable_, versionNumber_).Flush();
			//logicAnalyzer_.PrintSettings(terr);
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
				GetStream().Printf("%04dx%05d", uvoltScale_, uvoltOffset_);
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
			GetStream().Printf("*").Flush();
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
			GetStream().Printf("*").Flush();
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
			GetStream().Printf("*").Flush();
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
			GetStream().Printf("*").Flush();
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
						//SendReport(event_, nSamples);
						SendRLE(nSamples);
						event_ = eventNext;
					}
				} else {
					event_ = eventNext;
				}
				SendBitmap(event_);
				iEvent_++;
			}
		} else if (ch == '+') {		// abort
			GetStream().Flush();
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

void LogicAnalyzer::SigrokAdapter::SendBitmap(const Event& event)
{
	int iBuff = 0;
	uint8_t buff[32];
	uint32_t packedBitmap = event.GetPackedBitmap(logicAnalyzer_.GetPrintInfo());
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
	} else {
		// Normal mode
		int iBit = 0;
		uint8_t data = 0;
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
	}
	GetStream().Write(buff, iBuff);
}

void LogicAnalyzer::SigrokAdapter::SendRLE(int nSamples)
{
	int iBuff = 0;
	uint8_t buff[32];
	if (nSamples <= 0) return;
	if (nSamples > 640 * (count_of(buff) - 4)) nSamples = 640 * (count_of(buff) - 4);	// prevent overflow of buff
	if (nDigitalChEnabled_ <= 4) {
		// D4 mode
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
	GetStream().Write(buff, iBuff);
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
	GetStream().Write(buff, iBuff);
	//GetStream().Flush();
}

//------------------------------------------------------------------------------
// LogicAnalyzer::Decoder
//------------------------------------------------------------------------------
LogicAnalyzer::Decoder::Decoder(const LogicAnalyzer& logicAnalyzer, const char* name) :
			logicAnalyzer_{logicAnalyzer}, name_{name}
{}

void LogicAnalyzer::Decoder::PrintEvent(Printable& tout)
{
	char buffLine[256];
	double timeStampFactor = 1000'000. / logicAnalyzer_.GetSampleRate();
	EventIterator eventIter(logicAnalyzer_);
	Event event, eventBase;
	for (int iEvent = 0; eventIter.Next(event); ++iEvent) {
		if (iEvent == 1) eventBase = event;
		int iCol = 0;
		AnnotateWaveEvent(eventIter, event, buffLine, sizeof(buffLine), &iCol);
		CutTrailingSpace(buffLine);
		if (buffLine[0] == '\0') {
			// nothing to do
		} else if (eventBase.IsValid()) {
			tout.Printf("%12.2f %s\n", static_cast<double>(event.GetTimeStamp() - eventBase.GetTimeStamp()) * timeStampFactor, buffLine);
		} else {
			tout.Printf("%12s %s\n", "", buffLine);
		}
		if (Tickable::TickSub()) break;
	}

}

void LogicAnalyzer::Decoder::AnnotateWaveEvent(const EventIterator& eventIter, const Event& event, char* buffLine, int lenBuffLine, int *piCol)
{
	int nColsAnnotation = GetColsAnnotation();
	int& iCol = *piCol;
	int iColOrg = iCol;
	DoAnnotateWaveEvent(eventIter, event, buffLine, lenBuffLine, piCol);
	int nColsAdded = iCol - iColOrg;
	if (nColsAdded < nColsAnnotation) {
		iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, "%-*s", nColsAnnotation - nColsAdded, "");
	}
}

void LogicAnalyzer::Decoder::AnnotateWaveStreak(double timeStamp, char* buffLine, int lenBuffLine, int *piCol)
{
	int nColsAnnotation = GetColsAnnotation();
	int& iCol = *piCol;
	int iColOrg = iCol;
	DoAnnotateWaveStreak(timeStamp, buffLine, lenBuffLine, piCol);
	int nColsAdded = iCol - iColOrg;
	if (nColsAdded < nColsAnnotation) {
		iCol += ::snprintf(buffLine + iCol, lenBuffLine - iCol, "%-*s", nColsAnnotation - nColsAdded, "");
	}
}

}
