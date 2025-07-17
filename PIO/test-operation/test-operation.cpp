#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	do {
		PIO::Program program;
		program
		.program("echo_back")
			.pull()
			.mov("isr", "osr")
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled().put(0x01234567).put(0x89abcdef);
		::printf("program: %s\n", program.GetName());
		::printf("%08x\n", sm.get());
		::printf("%08x\n", sm.get());
		sm.UnclaimResource();
	} while (0);
	for (;;) Tickable::Tick();
}
