#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"

using namespace jxglib;

class LogicAnalyzer {
public:
	struct Event {
		uint32_t timeStamp;
		uint32_t bits;
	};
	static const uint nEventsMax = 2048;
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
	DMA::ChannelConfig config_;
	DMA::Channel* pChannel_;
	std::unique_ptr<Event> eventBuff_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& Start();
	LogicAnalyzer& Restart();
	LogicAnalyzer& Stop();
	LogicAnalyzer& Clear();
	int GetEventCount() const;
	void PrintWave(Printable& tout) const;
};

LogicAnalyzer::LogicAnalyzer() : pChannel_{nullptr}
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
	eventBuff_.reset(new Event[nEventsMax]);
	const GPIO& gpio = GPIO2;
	uint nPins = 28;
	program_
	.program("logic_analyzer")
	.L("loop").wrap_target()
		.mov("x", "~osr")
		.jmp("x--", "no_wrap_around")
		.mov("osr", "~x")
	.entry()
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("do_report")	[1]
	.L("no_wrap_around")
		.mov("osr", "~x")			// increment osr (counter) by 1
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("x!=y", "do_report")	// if pins state changed, report it
		.jmp("loop")				[2]
	.L("do_report")
		.in("osr", 32)				// auto-push osr (counter)
		.in("x", 32)				// auto-push x (current pins state)
		.mov("y", "x")				// save current pins state in y
	.wrap()
	.end();
	sm_.config.set_in_shift_left(true, 32); // shift left, autopush enabled, push threshold 32
	sm_.set_program(program_).set_listen_pins(gpio, -1).init().set_enabled();
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
		.set_trans_count_trig(nEventsMax);
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

void LogicAnalyzer::PrintWave(Printable& tout) const
{
	int nBitsToDisp = 16;
	float tick = 1'000; // usec
	float clockPIO = static_cast<float>(::clock_get_hz(clk_sys) / 10);
	int nEvents = GetEventCount();
	const Event& event = eventBuff_.get()[0];
	tout.Printf("%*s", 14, "");
	for (int iBit = 0; iBit < nBitsToDisp; ++iBit) {
		tout.Print((event.bits & (1 << iBit))? "   |" : " |  ");
	}
	tout.Println();
	uint32_t timeStampStart = eventBuff_.get()[1].timeStamp;
	for (int iEvent = 1; iEvent < nEvents; ++iEvent) {
		const Event& eventPrev = eventBuff_.get()[iEvent - 1];
		const Event& event = eventBuff_.get()[iEvent];
		if (eventPrev.bits == event.bits) continue; // skip if no change
		float delta = static_cast<float>(event.timeStamp - eventPrev.timeStamp) * 1000'000 / clockPIO;
		if (delta == 0) continue; // skip if no time difference
		int nDelta = static_cast<int>(delta / tick);
		if (nDelta < 40) {
			for (int i = 0; i < nDelta; ++i) {
				tout.Printf("%*s", 14, "");
				for (int iBit = 0; iBit < nBitsToDisp; ++iBit) {
					tout.Print((eventPrev.bits & (1 << iBit))? "   |" : " |  ");
				}
				tout.Println();
			}
		} else {
			tout.Printf("%*s", 14, "");
			for (int iBit = 0; iBit < nBitsToDisp; ++iBit) {
				tout.Print((eventPrev.bits & (1 << iBit))? "   :" : " :  ");
			}
			tout.Println();
		}
		uint32_t bitsTransition = event.bits ^ eventPrev.bits;	
		tout.Printf("%14.3f", static_cast<float>(event.timeStamp - timeStampStart) * 1000'000 / clockPIO);
		for (int iBit = 0; iBit < nBitsToDisp; ++iBit) {
			tout.Print((bitsTransition & (1 << iBit))? " +-+" : (event.bits & (1 << iBit))? "   |" : " |  ");
		}
		tout.Println();
	}
}

LogicAnalyzer logicAnalyzer;

ShellCmd(la, "Logic Analyzer")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h', "prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "pwm") == 0);
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		return Result::Success;
	}
	if (argc < 2) {
		int nEvents = logicAnalyzer.GetEventCount();
		tout.Printf("Number of events: %d\n", nEvents);
		return Result::Success;
	}
	const char* subCmd = argv[1];
	if (::strcasecmp(subCmd, "start") == 0) {
		logicAnalyzer.Start();
		tout.Println("Logic Analyzer started.");
	} else if (::strcasecmp(subCmd, "restart") == 0) {
		logicAnalyzer.Restart();
		tout.Println("Logic Analyzer restarted.");
	} else if (::strcmp(subCmd, "stop") == 0) {
		logicAnalyzer.Stop();
		tout.Println("Logic Analyzer stopped.");
	} else if (::strcmp(subCmd, "wave") == 0) {
		logicAnalyzer.PrintWave(tout);
	} else {
		tout.Printf("Unknown command: %s\n", subCmd);
		return Result::Error;
	}
	return Result::Success;
}

int main()
{
	::stdio_init_all();
	logicAnalyzer.Start();

	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
