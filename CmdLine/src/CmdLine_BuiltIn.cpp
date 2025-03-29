//==============================================================================
// CmdLine_BuiltIn.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/CmdLine.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// d addr bytes
//-----------------------------------------------------------------------------
class CmdLineEntry_d : public CmdLine::Entry {
private:
	uint32_t addr_;
	uint32_t bytes_;
public:
	static CmdLineEntry_d Instance;
public:
	CmdLineEntry_d() : CmdLine::Entry("d"), addr_{0x00000000}, bytes_{64} {}
public:
	virtual void Run(Terminal& terminal, int argc, char* argv[]) override;
};

CmdLineEntry_d CmdLineEntry_d::Instance;

void CmdLineEntry_d::Run(Terminal& terminal, int argc, char* argv[])
{
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terminal.Printf("invalid number\n");
			return;
		}
		addr_ = num;
	}
	if (argc >= 3) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[2], &p, 0);
		if (*p != '\0') {
			terminal.Printf("invalid number\n");
			return;
		}
		bytes_ = num;
	}
	terminal.Dump.AddrStart(addr_).DigitsAddr(8)(reinterpret_cast<const void*>(addr_), bytes_);
	addr_ += bytes_;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
CmdLineEntry(ticks)
{
	Tickable::PrintList(terminal);
}

}
