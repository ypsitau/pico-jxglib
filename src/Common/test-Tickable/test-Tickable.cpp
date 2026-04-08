#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

TickableEntry(TickableA, 100, Tickable::Priority::AboveNormal) {}
TickableEntry(TickableB, 100, Tickable::Priority::BelowNormal) {}
TickableEntry(TickableC, 100, Tickable::Priority::Lowest) {}
TickableEntry(TickableD, 100, Tickable::Priority::Lowest) {}
TickableEntry(TickableE, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableF, 100, Tickable::Priority::Normal) {}
TickableEntry(TickableG, 100, Tickable::Priority::Normal) {}
TickableEntry(TickableH, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableI, 100, Tickable::Priority::BelowNormal) {}
TickableEntry(TickableJ, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableK, 100, Tickable::Priority::BelowNormal) {}
TickableEntry(TickableL, 100, Tickable::Priority::AboveNormal) {}
TickableEntry(TickableM, 100, Tickable::Priority::Normal) {}
TickableEntry(TickableN, 100, Tickable::Priority::Normal) {}
TickableEntry(TickableO, 100, Tickable::Priority::AboveNormal) {}
TickableEntry(TickableP, 100, Tickable::Priority::Lowest) {}
TickableEntry(TickableQ, 100, Tickable::Priority::Normal) {}
TickableEntry(TickableR, 100, Tickable::Priority::AboveNormal) {}
TickableEntry(TickableS, 100, Tickable::Priority::Lowest) {}
TickableEntry(TickableT, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableU, 100, Tickable::Priority::AboveNormal) {}
TickableEntry(TickableV, 100, Tickable::Priority::BelowNormal) {}
TickableEntry(TickableW, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableX, 100, Tickable::Priority::Highest) {}
TickableEntry(TickableY, 100, Tickable::Priority::Lowest) {}
TickableEntry(TickableZ, 100, Tickable::Priority::BelowNormal) {}

int main(int argc, char* argv[])
{
	::stdio_init_all();
	::printf("--------\n");
	Tickable::PrintList(Stdio::Instance);
	for (;;) ::tight_loop_contents();
}
