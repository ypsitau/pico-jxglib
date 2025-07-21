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
	uint32_t pinBitmap_;
	uint pinMin_;
	float usecReso_;
	int nEventsToPrint_;
	int nClocksPerLoop_;
public:
	LogicAnalyzer();
	~LogicAnalyzer();
public:
	LogicAnalyzer& Start();
	LogicAnalyzer& Restart();
	LogicAnalyzer& Stop();
	LogicAnalyzer& Clear();
	LogicAnalyzer& SetPins(uint32_t pinBitmap, uint32_t pinMin) { pinBitmap_ = pinBitmap; pinMin_ = pinMin; return *this; }
	LogicAnalyzer& SetResolution(float usecReso) { usecReso_ = usecReso; return *this; }
	LogicAnalyzer& SetEventCountToPrint(int nEventsToPrint) { nEventsToPrint_ = nEventsToPrint; return *this; }
	float GetResolution() const { return usecReso_; }
	int GetEventCount() const;
	const LogicAnalyzer& PrintWave(Printable& tout) const;
	const LogicAnalyzer& PrintSettings(Printable& tout) const;
	const Event& GetEvent(int iEvent) const { return eventBuff_.get()[iEvent]; }
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
	//int nBitsToDisp = 16;
	float clockPIOProgram = static_cast<float>(::clock_get_hz(clk_sys) / nClocksPerLoop_);
	int nEvents = ChooseMin(GetEventCount(), nEventsToPrint_);
	if (nEvents == 0) return *this;
	int iEventStart = GetEventCount() - nEvents;
	const Event& eventStart = GetEvent(iEventStart);
	do {
		tout.Printf("%14s", "");
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Printf(" P%-2d", iBit + pinMin_);
		}
		tout.Println();
	} while (0);
	do {
		tout.Printf("%14.3f", 0.);
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Print((eventStart.bits & (1 << iBit))? "   |" : " |  ");
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
					if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? "   |" : " |  ");
				}
				tout.Println();
			}
		} else {
			tout.Println();
			tout.Printf("%14s", "");
			uint32_t pinBitmap = pinBitmap_;
			for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
				if (pinBitmap & 1) tout.Print((eventPrev.bits & (1 << iBit))? "   :" : " :  ");
			}
			tout.Println();
			tout.Println();
		}
		uint32_t bitsTransition = event.bits ^ eventPrev.bits;	
		tout.Printf("%14.3f", static_cast<float>(event.timeStamp - eventStart.timeStamp) * 1000'000 / clockPIOProgram);
		uint32_t pinBitmap = pinBitmap_;
		for (int iBit = 0; pinBitmap; ++iBit, pinBitmap >>= 1) {
			if (pinBitmap & 1) tout.Print((bitsTransition & (1 << iBit))? " +-+" : (event.bits & (1 << iBit))? "   |" : " |  ");
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

LogicAnalyzer logicAnalyzer;

ShellCmd(la, "Logic Analyzer")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h', "prints this help"),
		Arg::OptString("pins",		'p', "pins to monitor", "PINS"),
		Arg::OptString("reso",		'r', "resolution in microseconds (default 1000)", "RESO"),
		Arg::OptString("events",	'e', "number of events to print (default 80)", "NUM"),
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
	const char* value;
	if (arg.GetString("pins", &value)) {
		Arg::EachNum eachNum(value, GPIO::NumPins - 1);
		if (!eachNum.CheckValidity()) {
			terr.Printf("invalid GPIO pin number: %s\n", value);
			return Result::Error;
		}
		uint pinMin = GPIO::NumPins;
		uint32_t pinBitmap = 0;
		int pin;
		while (eachNum.Next(&pin)) {
			if (pin < 0 || pin >= GPIO::NumPins) {
				terr.Printf("invalid GPIO pin number: %d\n", pin);
				return Result::Error;
			}
			pinBitmap |= (1 << pin);
			if (pinMin > pin) pinMin = pin;
		}
		pinBitmap >>= pinMin;
		logicAnalyzer.SetPins(pinBitmap, pinMin);
	}
	if (arg.GetString("reso", &value)) {
		char* endptr = nullptr;
		float usecReso = ::strtod(value, &endptr);
		if (endptr == value || *endptr != '\0' || usecReso <= 0) {
			terr.Printf("Invalid resolution: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetResolution(usecReso);
	}
	if (arg.GetString("events", &value)) {
		char* endptr = nullptr;
		int nEventsToPrint = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || nEventsToPrint <= 0) {
			terr.Printf("Invalid resolution: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetEventCountToPrint(nEventsToPrint);
	}
	if (argc < 2) {
		logicAnalyzer.PrintSettings(tout);
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
	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
