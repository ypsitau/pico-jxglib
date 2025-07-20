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
	void Initialize();
	void Start();
	void Stop();
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

void LogicAnalyzer::Initialize()
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
	Start();
}

void LogicAnalyzer::Start()
{
	pChannel_->set_config(config_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(eventBuff_.get())
		.set_trans_count_trig(nEventsMax);
}

void LogicAnalyzer::Stop()
{
	pChannel_->abort();
}

int LogicAnalyzer::GetEventCount() const
{
	return (reinterpret_cast<uint32_t>(pChannel_->get_write_addr()) - reinterpret_cast<uint32_t>(eventBuff_.get())) / sizeof(Event);
}

void LogicAnalyzer::PrintWave(Printable& tout) const
{
	uint32_t clockPIO = ::clock_get_hz(clk_sys) / 10;
	int nEvents = GetEventCount();
	for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
		const Event& event = eventBuff_.get()[iEvent];
		tout.Printf("%.2fusec %032b\n", static_cast<float>(event.timeStamp) * 1000000 / clockPIO, event.bits);
	}
}

LogicAnalyzer logicAnalyzer;

ShellCmd(check, "check logic analyzer")
{
	logicAnalyzer.PrintWave(tout);
	return Result::Success;
}

int main()
{
	::stdio_init_all();
	logicAnalyzer.Initialize();

	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
