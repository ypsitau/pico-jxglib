#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIO::Program program;
	program
	.program("hello")
	.L("loop")
		.pull()
		.out("pins",	1)
		.jmp("loop")
	.end();
	PIO::Resource resource(program);
	if (!resource.Claim()) {
		::printf("Failed to claim PIO resource\n");
		return 1;
	}
	PIO::StateMachine sm(resource);
	

	for (;;) ::tight_loop_contents();
}
