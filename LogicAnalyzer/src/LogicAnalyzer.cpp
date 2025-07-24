
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

LogicAnalyzer::LogicAnalyzer(int nEventsMax) : pChannelTbl_{nullptr}, nEventsMax_{nEventsMax},
		samplingInfo_{false, 0, 0}, nClocksPerLoop_{1}, usecReso_{1'000},
		printInfo_{0, nullptr, 80, PrintPart::Head, &waveStyle_fancy2}
{}

LogicAnalyzer::~LogicAnalyzer()
{
	for (auto& pChannel : pChannelTbl_) {
		if (pChannel) {
			pChannel->unclaim();
			pChannel = nullptr;
		}
	}
}

LogicAnalyzer& LogicAnalyzer::UpdateSamplingInfo()
{
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

LogicAnalyzer& LogicAnalyzer::Enable()
{
	if (samplingInfo_.enabledFlag) Disable(); // disable if already enabled
	if (samplingInfo_.pinBitmap == 0) return *this;
	eventBuffTbl_[0].reset(new Event[nEventsMax_]);
	uint nPins = 0;	// nPins must be less than 32 to avoid auto-push during sampling
	for (uint32_t pinBitmap = samplingInfo_.pinBitmap; pinBitmap != 0; pinBitmap >>= 1, ++nPins) ;
	program_
	.program("logic_analyzer")
	.L("loop").wrap_target()
		.mov("x", "~osr")
		.jmp("x--", "no_wrap_around")
		.mov("osr", "~x")
	.entry()
		.mov("isr", "null")
		.in("pins", nPins)				// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("do_report")	[1]
	.L("no_wrap_around")
		.mov("osr", "~x")				// increment osr (counter) by 1
		.mov("isr", "null")
		.in("pins", nPins)				// save current pins state in isr (no auto-push)
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
	smTbl_[0].config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	smTbl_[0].set_program(program_).set_listen_pins(samplingInfo_.pinMin, -1).init().set_enabled();
	pChannelTbl_[0] = DMA::claim_unused_channel();
	configTbl_[0].set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(smTbl_[0].get_dreq_rx()) // set DREQ of StateMachine's rx
		.set_chain_to(*pChannelTbl_[0])    // disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	pChannelTbl_[0]->set_config(configTbl_[0])
		.set_read_addr(smTbl_[0].get_rxf())
		.set_write_addr(eventBuffTbl_[0].get())
		.set_trans_count_trig(nEventsMax_ * sizeof(Event) / sizeof(uint32_t));
	samplingInfo_.enabledFlag = true;
	return *this;
}

LogicAnalyzer& LogicAnalyzer::Disable()
{
	if (samplingInfo_.enabledFlag) {
		smTbl_[0].set_enabled(false);
		smTbl_[0].remove_program();
		pChannelTbl_[0]->abort();
		pChannelTbl_[0]->unclaim();
		samplingInfo_.enabledFlag = false;
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
	return (reinterpret_cast<uint32_t>(pChannelTbl_[0]->get_write_addr()) - reinterpret_cast<uint32_t>(eventBuffTbl_[0].get())) / sizeof(Event);
}

const LogicAnalyzer::Event& LogicAnalyzer::GetEvent(int iEvent) const
{
	return eventBuffTbl_[0].get()[iEvent];
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
		(printInfo_.part == PrintPart::Head)? ChooseMin(nEventsAll, printInfo_.nEvents) :
		(printInfo_.part == PrintPart::Tail)?  ChooseMin(nEventsAll, printInfo_.nEvents) :
		(printInfo_.part == PrintPart::All)? nEventsAll : 0;
	int iEventStart =
		(printInfo_.part == PrintPart::Head)? 0 :
		(printInfo_.part == PrintPart::Tail)? nEventsAll - nEventsToPrint :
		(printInfo_.part == PrintPart::All)? 0 : 0;
	int iEventBase = (nEventsToPrint == 0)? 0 : 1;
	const Event& eventStart = GetEvent(iEventStart);
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
				tout.Print(IsPinAsserted(eventStart.bits, pin)? waveStyle.strHigh : waveStyle.strLow);
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
	}
	for (int i = 1; i < nEventsToPrint; ++i) {
		const Event& eventPrev = GetEvent(iEventStart + i - 1);
		const Event& event = GetEvent(iEventStart + i);
		if (eventPrev.bits == event.bits) continue; // skip if no change
		double delta = static_cast<double>(event.timeStamp - eventPrev.timeStamp) * 1000'000 / clockPIOProgram;
		if (delta == 0) continue; // skip if no time difference
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
						tout.Print(IsPinAsserted(eventPrev.bits, pin)? waveStyle.strHigh : waveStyle.strLow);
					} else {
						tout.Print(waveStyle.strBlank);
					}
				}
				tout.Println();
			}
		} else {
			//tout.Println();
			tout.Printf("%12s", "");
			for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
				uint pin = printInfo_.pinTbl[iPin];
				if (pin == -1) {
					tout.Print(strBlank);
					continue;
				}
				if (IsPinEnabled(pin)) {
					tout.Print(IsPinAsserted(eventPrev.bits, pin)? waveStyle.strHighIdle : waveStyle.strLowIdle);
				} else {
					tout.Print(waveStyle.strBlank);
				}
			}
			tout.Println();
			//tout.Println();
		}
		uint32_t bitsTransition = event.bits ^ eventPrev.bits;	
		tout.Printf("%12.2f", static_cast<double>(event.timeStamp - GetEvent(iEventBase).timeStamp) * 1000'000 / clockPIOProgram);
		for (int iPin = 0; iPin < printInfo_.nPins; ++iPin) {
			uint pin = printInfo_.pinTbl[iPin];
			if (pin == -1) {
				tout.Print(strBlank);
				continue;
			}
			if (IsPinEnabled(pin)) {
				tout.Print(IsPinAsserted(bitsTransition, pin)?
					(IsPinAsserted(event.bits, pin)? waveStyle.strLowToHigh : waveStyle.strHighToLow) :
					(IsPinAsserted(event.bits, pin)? waveStyle.strHigh : waveStyle.strLow));
			} else {
				tout.Print(waveStyle.strBlank);
			}
		}
		tout.Println();
		if (Tickable::TickSub()) break;
	}
	if (nEventsToPrint > 0) printHeader();
	return *this;
}

const LogicAnalyzer& LogicAnalyzer::PrintSettings(Printable& tout) const
{
	int nEvents = GetEventCount();
	tout.Printf("%s", !samplingInfo_.enabledFlag? "disabled" : (nEvents < nEventsMax_)? "enabled" : "enabled(full)");
	do {
		bool firstFlag = true;
		tout.Printf(" - pins:");
		for (uint pin = 0; pin < GPIO::NumPins; ++pin) {
			if (IsPinEnabled(pin)) {
				tout.Printf(firstFlag? "%d" : ",%d", pin);
				firstFlag = false;
			}
		}
		if (firstFlag) tout.Print("none");
	} while (0);
	tout.Printf(" events:%d/%d", nEvents, nEventsMax_);
	tout.Println();
	return *this;
}

}
