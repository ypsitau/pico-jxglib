#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
class Sample01_skeleton {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample01_skeleton::Run()
{
	program_
	.program("Sample01_skeleton")
	.end();
	sm_.set_program(program_)
		.init()
		.set_enabled();
}

//-----------------------------------------------------------------------------
class Sample02_set {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample02_set::Run()
{
	program_
	.program("Sample02_set")
	.wrap_target()
		.set("pins",	0b1)
		.set("pins",	0b0)
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_set_pin(GPIO15)
		.config_set_clkdiv(65536)	// Pico: 125MHz / 65536 = 1907 Hz, Pico2: 150MHz / 65536 = 2288 Hz
		.init()
		.set_enabled();
}

//-----------------------------------------------------------------------------
class Sample03_sideset {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample03_sideset::Run()
{
	program_
	.program("Sample03_sideset")
	.side_set(1)
	.wrap_target()
		.nop()		.side(0b1)
		.nop()		.side(0b0)
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_sideset_pin(GPIO15)
		.config_set_clkdiv(65536)	// Pico: 125MHz / 65536 = 1907 Hz, Pico2: 150MHz / 65536 = 2288 Hz
		.init()
		.set_enabled();
}

//-----------------------------------------------------------------------------
class Sample04_mov {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample04_mov::Run()
{
	program_
	.program("Sample04_mov")
		.set("x",		0b1)
		.set("y",		0b0)
	.wrap_target()
		.mov("pins",	"x")
		.mov("pins",	"y")
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_out_pin(GPIO15)	// mov uses out pins
		.config_set_clkdiv(65536)	// Pico: 125MHz / 65536 = 1907 Hz, Pico2: 150MHz / 65536 = 2288 Hz
		.init()
		.set_enabled();
}

//-----------------------------------------------------------------------------
class Sample05_out {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample05_out::Run()
{
	program_
	.program("Sample05_out")
		.pull()
		.mov("x",		"osr")
	.L("loop")
		.nop()
	.wrap_target()
		.out("pins",	1)
		.jmp("!osre",	"loop")
		.mov("osr",		"x")
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_out_pin(GPIO15)
		.config_set_clkdiv(65536)	// Pico: 125MHz / 65536 = 1907 Hz, Pico2: 150MHz / 65536 = 2288 Hz
		.init()
		.set_enabled()
		.put(0b10101010101010101010101010101010);
}

//-----------------------------------------------------------------------------
class Sample50_blink {
private:
	PIO::Program program_;
	PIO::StateMachine sm_;
public:
	void Run();
};

void Sample50_blink::Run()
{
	program_
	.program("Sample50_blink")
		.pull()						// osr <- txfifo
		.out("y",		32)			// y[31:0] <- osr[31:0], osr[31:0] <- 0
	.wrap_target()
		.mov("x",		"y")
		.set("pins",	0b1)
	.L("loop1")
		.jmp("x--",		"loop1")	// Delay for (x + 1) cycles
		.mov("x",		"y")
		.set("pins",	0b0)
	.L("loop2")
		.jmp("x--",		"loop2")	// Delay for (x + 1) cycles
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_set_pin(GPIO15)
		.init()
		.set_enabled();
	int freq = 2; // Hz
	sm_.put((::clock_get_hz(clk_sys) / (freq * 2)) - 3);
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(true);
	//Sample01_skeleton sample;
	//Sample02_set sample;
	//Sample03_sideset sample;
	//Sample04_mov sample;
	Sample05_out sample;
	//Sample50_blink sample;
	sample.Run();
	for (;;) {
		Tickable::Tick();
	}
}
