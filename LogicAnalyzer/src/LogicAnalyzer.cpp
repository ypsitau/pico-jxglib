
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
	strHigh:		"   │",
	strHighIdle:	"   :",
	strLow:			" │  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" └─┐",
	strHighToLow:	" ┌─┘",
	formatHeader:	"GP%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy3 = {
	name:			"fancy3",
	strHigh:		"    │",
	strHighIdle:	"    :",
	strLow:			" │   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" └──┐",
	strHighToLow:	" ┌──┘",
	formatHeader:	" GP%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_fancy4 = {
	name:			"fancy4",
	strHigh:		"     │",
	strHighIdle:	"     :",
	strLow:			" │    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" └───┐",
	strHighToLow:	" ┌───┘",
	formatHeader:	" GP%-2d ",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple1 = {
	name:			"simple1",
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
	strHigh:		"   |",
	strHighIdle:	"   :",
	strLow:			" |  ",
	strLowIdle:		" :  ",
	strLowToHigh:	" +-+",
	strHighToLow:	" +-+",
	formatHeader:	"GP%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple3 = {
	name:			"simple3",
	strHigh:		"    |",
	strHighIdle:	"    :",
	strLow:			" |   ",
	strLowIdle:		" :   ",
	strLowToHigh:	" +--+",
	strHighToLow:	" +--+",
	formatHeader:	" GP%-2d",
};

const LogicAnalyzer::WaveStyle LogicAnalyzer::waveStyle_simple4 = {
	name:			"simple4",
	strHigh:		"     |",
	strHighIdle:	"     :",
	strLow:			" |    ",
	strLowIdle:		" :    ",
	strLowToHigh:	" +---+",
	strHighToLow:	" +---+",
	formatHeader:	" GP%-2d ",
};

LogicAnalyzer::LogicAnalyzer(int nEventsMax) : pChannelTbl_{nullptr}, nEventsMax_{nEventsMax},
		enabledFlag_{false}, pinBitmap_{0}, pinMin_{0}, nClocksPerLoop_{1}, usecReso_{1'000},
		printInfo_{80, PrintPart::Head, &waveStyle_fancy2}
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

LogicAnalyzer& LogicAnalyzer::Enable()
{
	if (pinBitmap_ == 0) return *this;
	if (enabledFlag_) Disable(); // disable if already enabled
	eventBuffTbl_[0].reset(new Event[nEventsMax_]);
	uint nPins = 0;	// nPins must be less than 32 to avoid auto-push during sampling
	for (uint32_t pinBitmap = pinBitmap_; pinBitmap != 0; pinBitmap >>= 1, ++nPins) ;
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
	smTbl_[0].set_program(program_).set_listen_pins(pinMin_, -1).init().set_enabled();
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
	enabledFlag_ = true;
	return *this;
}

LogicAnalyzer& LogicAnalyzer::Disable()
{
	if (enabledFlag_) {
		smTbl_[0].set_enabled(false);
		smTbl_[0].remove_program();
		pChannelTbl_[0]->abort();
		pChannelTbl_[0]->unclaim();
		enabledFlag_ = false;
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
	const WaveStyle& waveStyle = *printInfo_.pWaveStyle;
	auto printHeader = [&]() {
		// Print header
		tout.Printf("%14s ", "Time [usec]");
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Printf(waveStyle.formatHeader, iBit + pinMin_);
		}
		tout.Println();
	};
	float clockPIOProgram = static_cast<float>(::clock_get_hz(clk_sys) / nClocksPerLoop_);
	int nEvents =
		(printInfo_.part == PrintPart::Head)? ChooseMin(GetEventCount(), printInfo_.nEvents) :
		(printInfo_.part == PrintPart::Tail)?  ChooseMin(GetEventCount(), printInfo_.nEvents) :
		(printInfo_.part == PrintPart::All)? GetEventCount() : 0;
	if (nEvents == 0) return *this;
	int iEventStart =
		(printInfo_.part == PrintPart::Head)? 0 :
		(printInfo_.part == PrintPart::Tail)? GetEventCount() - nEvents :
		(printInfo_.part == PrintPart::All)? 0 : 0;
	int iEventBase = (iEventStart == 0 && nEvents > 1)? 1 : iEventStart;
	const Event& eventStart = GetEvent(iEventStart);
	printHeader();
	do {
		if (iEventStart == iEventBase) {
			tout.Printf("%14.3f", 0.);
		} else {
			tout.Printf("%14s", "");
		}
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Print((eventStart.bits & (1 << iBit))? waveStyle.strHigh : waveStyle.strLow);
		}
		tout.Println();
	} while (0);
	for (int i = 1; i < nEvents; ++i) {
		const Event& eventPrev = GetEvent(iEventStart + i - 1);
		const Event& event = GetEvent(iEventStart + i);
		if (eventPrev.bits == event.bits) continue; // skip if no change
		float delta = static_cast<float>(event.timeStamp - eventPrev.timeStamp) * 1000'000 / clockPIOProgram;
		if (delta == 0) continue; // skip if no time difference
		int nDelta = static_cast<int>(delta / usecReso_);
		if (nDelta < 40) {
			for (int i = 0; i < nDelta; ++i) {
				tout.Printf("%14s", "");
				uint32_t pinBitmap = pinBitmap_;
				for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
					if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? waveStyle.strHigh : waveStyle.strLow);
				}
				tout.Println();
			}
		} else {
			//tout.Println();
			tout.Printf("%14s", "");
			uint32_t pinBitmap = pinBitmap_;
			for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
				if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? waveStyle.strHighIdle : waveStyle.strLowIdle);
			}
			tout.Println();
			//tout.Println();
		}
		uint32_t bitsTransition = event.bits ^ eventPrev.bits;	
		tout.Printf("%14.3f", static_cast<float>(event.timeStamp - GetEvent(iEventBase).timeStamp) * 1000'000 / clockPIOProgram);
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (!(pinBitmap & 1)) {
				// nothing to do
			} else if (bitsTransition & (1 << iBit)) {
				tout.Print((event.bits & (1 << iBit))? waveStyle.strLowToHigh : waveStyle.strHighToLow);
			} else {
				tout.Print((event.bits & (1 << iBit))? waveStyle.strHigh : waveStyle.strLow);
			}
		}
		tout.Println();
		if (Tickable::TickSub()) break;
	}
	printHeader();
	return *this;
}

const LogicAnalyzer& LogicAnalyzer::PrintSettings(Printable& tout) const
{
	tout.Printf("pins:");
	if (pinBitmap_ == 0) {
		tout.Printf("none");
	} else {
		int i = 0;
		for (uint32_t pinBitmap = pinBitmap_; pinBitmap; pinBitmap >>= 1, ++i) {
			if (pinBitmap & 1) {
				tout.Printf((i == 0)? "%d" : ",%d", pinMin_ + i);
			}
		}
	}
	tout.Printf(" reso:%.2fusec style:%s recorded-events:%d\n", usecReso_, GetWaveStyle().name, GetEventCount());
	return *this;
}

}
