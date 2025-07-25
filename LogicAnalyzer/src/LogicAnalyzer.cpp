
//==============================================================================
// LogicAnalyzer.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy1 = {
	name:			"fancy1",
	strBlank:		"   ",
	strHigh:		"  │",
	strHighIdle:	"  :",
	strLow:			" │ ",
	strLowIdle:		" : ",
	strLowToHigh:	" └┐",
	strHighToLow:	" ┌┘",
	formatHeader:	"P%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy2 = {
	name:			"fancy2",
	strBlank:		"    ",
	strHigh:		"   │",
	strHighIdle:	"   :",
	strLow:			" │  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" └─┐",
	strHighToLow:	" ┌─┘",
	formatHeader:	"P%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy3 = {
	name:			"fancy3",
	strBlank:		"     ",
	strHigh:		"    │",
	strHighIdle:	"    :",
	strLow:			" │   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" └──┐",
	strHighToLow:	" ┌──┘",
	formatHeader:	"GP%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy4 = {
	name:			"fancy4",
	strBlank:		"      ",
	strHigh:		"     │",
	strHighIdle:	"     :",
	strLow:			" │    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" └───┐",
	strHighToLow:	" ┌───┘",
	formatHeader:	"GP%-2d  ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple1 = {
	name:			"simple1",
	strBlank:		"   ",
	strHigh:		"  |",
	strHighIdle:	"  :",
	strLow:			" | ",
	strLowIdle:		" : ",
	strLowToHigh:	" ++",
	strHighToLow:	" ++",
	formatHeader:	"P%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple2 = {
	name:			"simple2",
	strBlank:		"    ",
	strHigh:		"   |",
	strHighIdle:	"   :",
	strLow:			" |  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" +-+",
	strHighToLow:	" +-+",
	formatHeader:	"P%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple3 = {
	name:			"simple3",
	strBlank:		"     ",
	strHigh:		"    |",
	strHighIdle:	"    :",
	strLow:			" |   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" +--+",
	strHighToLow:	" +--+",
	formatHeader:	"GP%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple4 = {
	name:			"simple4",
	strBlank:		"      ",
	strHigh:		"     |",
	strHighIdle:	"     :",
	strLow:			" |    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" +---+",
	strHighToLow:	" +---+",
	formatHeader:	"GP%-2d  ",
};

LogicAnalyzer::LogicAnalyzer(int nRawEventMax) : target_{Target::Internal}, nRawEventMax_{nRawEventMax}, nClocksPerLoop_{1}, usecReso_{1'000}
{}

LogicAnalyzer::~LogicAnalyzer()
{}

LogicAnalyzer& LogicAnalyzer::UpdateSamplingInfo()
{
	samplingInfo_.pinBitmap = 0;
	samplingInfo_.pinMin = GPIO::NumPins;
	for (int i = 0; i < printInfo_.nPins; ++i) {
		uint pin = printInfo_.pinTbl[i];
		if (pin != -1) {
			if (samplingInfo_.pinMin > pin) samplingInfo_.pinMin = pin;
			samplingInfo_.pinBitmap |= (1 << pin);
		}
	}
	samplingInfo_.pinBitmap >>= samplingInfo_.pinMin;
	return *this;
}

bool LogicAnalyzer::Enable()
{
	if (samplingInfo_.enabledFlag) Disable(); // disable if already enabled
	if (samplingInfo_.pinBitmap == 0) return true;
	if (!samplerTbl_[0].AllocBuff(nRawEventMax_)) return false;
	uint nPinsConsecutive = 0;	// nPinsConsecutive must be less than 32 to avoid auto-push during sampling
	for (uint32_t pinBitmap = samplingInfo_.pinBitmap; pinBitmap != 0; pinBitmap >>= 1, ++nPinsConsecutive) ;
	uint relAddrEntry;
	program_
	.program("logic_analyzer")
	.L("loop").wrap_target()
		.mov("x", "~osr")
		.jmp("x--", "no_wrap_around")
		.mov("osr", "~x")
	.pub(&relAddrEntry)
		.mov("isr", "null")
		.in("pins", nPinsConsecutive)	// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("do_report")	[1]
	.L("no_wrap_around")
		.mov("osr", "~x")				// increment osr (counter) by 1
		.mov("isr", "null")
		.in("pins", nPinsConsecutive)	// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("x!=y", "do_report")		// if pins state changed, report it
		.jmp("loop")		[2]
	.L("do_report")
		.in("osr", 32)					// auto-push osr (counter)
		.in("x", 32)					// auto-push x (current pins state)
		.mov("y", "x")					// save current pins state in y
	.wrap()
	.end();
	nClocksPerLoop_ = 10;
	samplerTbl_[0].SetProgram(program_, relAddrEntry, samplingInfo_.pinMin, nPinsConsecutive);
	if (target_ == Target::External) {
		uint pin = samplingInfo_.pinMin;
		for (uint32_t pinBitmap = samplingInfo_.pinBitmap; pinBitmap != 0; pinBitmap >>= 1, ++pin) {
			if (pinBitmap & 1) {
				::pio_gpio_init(samplerTbl_[0].GetSM().pio, pin);
				::gpio_disable_pulls(pin);
			}
		}
	}
	samplerTbl_[0].EnableSM();
	samplerTbl_[0].EnableDMA();
	samplingInfo_.enabledFlag = true;
	return true;
}

LogicAnalyzer& LogicAnalyzer::Disable()
{
	if (samplingInfo_.enabledFlag) {
		samplingInfo_.enabledFlag = false;
		samplerTbl_[0].DisableSM();
		samplerTbl_[0].DisableDMA();
		if (target_ == Target::External) {
			uint pin = samplingInfo_.pinMin;
			for (uint32_t pinBitmap = samplingInfo_.pinBitmap; pinBitmap != 0; pinBitmap >>= 1, ++pin) {
				if (pinBitmap & 1) {
					::gpio_set_dir(pin, GPIO_IN);
					::gpio_put(pin, 0);
					::gpio_pull_down(pin);
					::gpio_set_function(pin, GPIO_FUNC_NULL);
				}
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

int LogicAnalyzer::GetEventCount() const
{
	return samplingInfo_.enabledFlag? samplerTbl_[0].GetRawEventCount() : 0;
}

const LogicAnalyzer::RawEvent& LogicAnalyzer::GetRawEvent(int iSampler, int iRawEvent) const
{
	return samplerTbl_[iSampler].GetRawEvent(iRawEvent);
}

const LogicAnalyzer& LogicAnalyzer::PrintWave(Printable& tout) const
{
	int nEventsAll = GetEventCount();
	if (nEventsAll == 0) {
		tout.Printf("no events to print\n");
		return *this;
	}
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
	double clockPIOProgram = static_cast<double>(::clock_get_hz(clk_sys) / nClocksPerLoop_);
	int nEventsToPrint =
		(printInfo_.part == PrintPart::Head)? ChooseMin(nEventsAll, printInfo_.nEventsToPrint) :
		(printInfo_.part == PrintPart::Tail)?  ChooseMin(nEventsAll, printInfo_.nEventsToPrint) :
		(printInfo_.part == PrintPart::All)? nEventsAll : 0;
	int iEventStart =
		(printInfo_.part == PrintPart::Head)? 0 :
		(printInfo_.part == PrintPart::Tail)? nEventsAll - nEventsToPrint :
		(printInfo_.part == PrintPart::All)? 0 : 0;
	int iEventBase = (nEventsToPrint == 0)? 0 : 1;
	const RawEvent& eventStart = GetRawEvent(0, iEventStart);
	const RawEvent& eventBase = GetRawEvent(0, iEventBase);
	if (nEventsToPrint > 0) {
		if (iEventStart == iEventBase) {
			tout.Printf("%12.2f", 0.);
		} else {
			tout.Printf("%12s", "");
		}
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
				continue;
			}
			if (IsPinEnabled(pin)) {
				tout.Print(IsPinAsserted(eventStart.pinBitmap, pin)? waveStyle.strHigh : waveStyle.strLow);
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
	}
	const RawEvent* pEventPrev = &eventStart;
	for (int i = 1; i < nEventsToPrint; ++i) {
		const RawEvent& event = GetRawEvent(0, iEventStart + i);
		if (pEventPrev->pinBitmap == event.pinBitmap) continue; // skip if no change
		double delta = static_cast<double>(event.timeStamp - pEventPrev->timeStamp) * 1000'000 / clockPIOProgram;
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
					if (IsPinEnabled(pin)) {
						tout.Print(IsPinAsserted(pEventPrev->pinBitmap, pin)? waveStyle.strHigh : waveStyle.strLow);
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
				if (IsPinEnabled(pin)) {
					tout.Print(IsPinAsserted(pEventPrev->pinBitmap, pin)? waveStyle.strHighIdle : waveStyle.strLowIdle);
				} else {
					tout.Print(waveStyle.strBlank);
				}
			}
			tout.Println();
		}
		uint32_t bitsTransition = event.pinBitmap ^ pEventPrev->pinBitmap;
		tout.Printf("%12.2f", static_cast<double>(event.timeStamp - eventBase.timeStamp) * 1000'000 / clockPIOProgram);
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
				continue;
			}
			if (IsPinEnabled(pin)) {
				tout.Print(IsPinAsserted(bitsTransition, pin)?
					(IsPinAsserted(event.pinBitmap, pin)? waveStyle.strLowToHigh : waveStyle.strHighToLow) :
					(IsPinAsserted(event.pinBitmap, pin)? waveStyle.strHigh : waveStyle.strLow));
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
		if (Tickable::TickSub()) break;
		pEventPrev = &event;
	}
	if (nEventsToPrint > 0) printHeader();
	return *this;
}

const LogicAnalyzer& LogicAnalyzer::PrintSettings(Printable& tout) const
{
	int nEvents = GetEventCount();
	if (samplingInfo_.enabledFlag) {
		const PIO::StateMachine& sm = samplerTbl_[0].GetSM();
		tout.Printf("enabled%s pio%d", (nEvents == nRawEventMax_)? "(full)" : "", sm.pio.get_index());
	} else {
		tout.Printf("disabled ----");
	}
	tout.Printf(" target:%s", (target_ == Target::Internal)? "internal" : "external");
	do {
		bool firstFlag = true;
		tout.Printf(" pins:");
		uint pinStart = -1;
		uint pinEnd = -1;
		bool inSequence = false;	
		for (uint pin = 0; pin <= GPIO::NumPins; ++pin) {
			if (pin < GPIO::NumPins && IsPinEnabled(pin)) {
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
	tout.Printf(" events:%d/%d", nEvents, nRawEventMax_);
	tout.Println();
	return *this;
}

//------------------------------------------------------------------------------
// LogicAnalyzer::Sampler
//------------------------------------------------------------------------------
bool LogicAnalyzer::Sampler::AllocBuff(int nRawEventMax)
{
	nRawEventMax_ = nRawEventMax;
	rawEventBuff_.reset();
	rawEventBuff_.reset(new RawEvent[nRawEventMax]);
	return !!rawEventBuff_;
}

void LogicAnalyzer::Sampler::SetProgram(const PIO::Program& program, uint relAddrEntry, uint pinMin, int nPinsConsecutive)
{
	sm_.config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	sm_.set_program(program).set_listen_pins(pinMin, nPinsConsecutive).init_with_entry(relAddrEntry);
}

void LogicAnalyzer::Sampler::EnableDMA()
{
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
		.set_write_addr(rawEventBuff_.get())
		.set_trans_count_trig(nRawEventMax_ * sizeof(RawEvent) / sizeof(uint32_t));
}

void LogicAnalyzer::Sampler::DisableDMA()
{
	pChannel_->abort();
	pChannel_->unclaim();
	pChannel_ = nullptr;
	rawEventBuff_.reset();
}

int LogicAnalyzer::Sampler::GetRawEventCount() const
{
	return (reinterpret_cast<uint32_t>(pChannel_->get_write_addr()) -
			reinterpret_cast<uint32_t>(rawEventBuff_.get())) / sizeof(RawEvent);
}

}
