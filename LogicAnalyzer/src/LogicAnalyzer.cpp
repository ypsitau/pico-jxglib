
//==============================================================================
// LogicAnalyzer.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LogicAnalyzer
//------------------------------------------------------------------------------
const LogicAnalyzer::WavePattern LogicAnalyzer::wavePattern_Fancy = {
	strHigh:		"    │",
	strHighIdle:	"    :",
	strLow:			"  │  ",
	strLowIdle:		"  :  ",
	strLowToHigh:	"  └─┐",
	strHighToLow:	"  ┌─┘",
	formatHeader:	" GP%-2d",
};

const LogicAnalyzer::WavePattern LogicAnalyzer::wavePattern_Simple = {
	strHigh:		"    |",
	strHighIdle:	"    :",
	strLow:			"  |  ",
	strLowIdle:		"  :  ",
	strLowToHigh:	"  +-+",
	strHighToLow:	"  +-+",
	formatHeader:	" GP%-2d",
};

LogicAnalyzer::LogicAnalyzer() : pChannel_{nullptr}, usecReso_{1'000}, nEventsToPrint_{80}, nClocksPerLoop_{1}
{}

LogicAnalyzer::~LogicAnalyzer()
{
	if (pChannel_) {
		pChannel_->unclaim();
		pChannel_ = nullptr;
	}
}

LogicAnalyzer& LogicAnalyzer::Start()
{
	if (pinBitmap_ == 0) return *this;
	eventBuff_.reset(new Event[nEventsMax]);
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
	sm_.config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	sm_.set_program(program_).set_listen_pins(pinMin_, -1).init().set_enabled();
	pChannel_ = DMA::claim_unused_channel();
	config_.set_enable(true)
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
	pChannel_->set_config(config_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(eventBuff_.get())
		.set_trans_count_trig(nEventsMax * sizeof(Event) / sizeof(uint32_t));
	return *this;
}

LogicAnalyzer& LogicAnalyzer::Restart()
{
	sm_.remove_program();
	pChannel_->unclaim();
	Start();
	return *this;
}

LogicAnalyzer& LogicAnalyzer::Stop()
{
	pChannel_->abort();
	return *this;
}

LogicAnalyzer& LogicAnalyzer::Clear()
{
	pChannel_->set_config(config_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(eventBuff_.get())
		.set_trans_count(nEventsMax);
	return *this;
}

int LogicAnalyzer::GetEventCount() const
{
	return (reinterpret_cast<uint32_t>(pChannel_->get_write_addr()) - reinterpret_cast<uint32_t>(eventBuff_.get())) / sizeof(Event);
}

const LogicAnalyzer& LogicAnalyzer::PrintWave(Printable& tout) const
{
	//const WavePattern* pWavePattern = &wavePattern_Simple;
	const WavePattern* pWavePattern = &wavePattern_Fancy;
	float clockPIOProgram = static_cast<float>(::clock_get_hz(clk_sys) / nClocksPerLoop_);
	int nEvents = ChooseMin(GetEventCount(), nEventsToPrint_);
	if (nEvents == 0) return *this;
	int iEventStart = GetEventCount() - nEvents;
	const Event& eventStart = GetEvent(iEventStart);
	do {
		// Print header
		tout.Printf("%14s ", "Time [usec]");
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Printf(pWavePattern->formatHeader, iBit + pinMin_);
		}
		tout.Println();
	} while (0);
	do {
		tout.Printf("%14.3f", 0.);
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Print((eventStart.bits & (1 << iBit))? pWavePattern->strHigh : pWavePattern->strLow);
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
					if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? pWavePattern->strHigh : pWavePattern->strLow);
				}
				tout.Println();
			}
		} else {
			tout.Println();
			tout.Printf("%14s", "");
			uint32_t pinBitmap = pinBitmap_;
			for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
				if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? pWavePattern->strHighIdle : pWavePattern->strLowIdle);
			}
			tout.Println();
			tout.Println();
		}
		uint32_t bitsTransition = event.bits ^ eventPrev.bits;	
		tout.Printf("%14.3f", static_cast<float>(event.timeStamp - eventStart.timeStamp) * 1000'000 / clockPIOProgram);
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (!(pinBitmap & 1)) {
				// nothing to do
			} else if (bitsTransition & (1 << iBit)) {
				tout.Print((event.bits & (1 << iBit))? pWavePattern->strLowToHigh : pWavePattern->strHighToLow);
			} else {
				tout.Print((event.bits & (1 << iBit))? pWavePattern->strHigh : pWavePattern->strLow);
			}
		}
		tout.Println();
		if (Tickable::TickSub()) break;
	}
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
	tout.Printf(" reso:%.2fusec recorded-events:%d\n", usecReso_, GetEventCount());
	return *this;
}

}
