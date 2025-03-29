//==============================================================================
// BuiltInCommand.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/CmdLine.h"

namespace jxglib {

CmdLineEntry(d)
{
	uint32_t addr = 0x00000000;
	if (argc >= 2) {
		char* p = nullptr;
		addr = ::strtoul(argv[1], &p, 16);
	}
	terminal.Dump.AddrStart(addr).DigitsAddr(8)(reinterpret_cast<const void*>(addr), 64);
}

}
