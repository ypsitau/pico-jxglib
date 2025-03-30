//==============================================================================
// CmdLine_BuiltIn.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/CmdLine.h"

namespace jxglib::CmdLine_BuiltIn {

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
	int nColsTerm, nRowsTerm;
	terminal.GetSize(&nColsTerm, &nRowsTerm);
	nColsTerm -= 8 + 2;
	int nCols = ((nColsTerm + 1) / 3) / 8 * 8;
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
	if (nCols > 0) {
		terminal.Dump.Cols(nCols).AddrStart(addr_).DigitsAddr(8)(reinterpret_cast<const void*>(addr_), bytes_);
	}
	addr_ += bytes_;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
CmdLineEntry(ticks)
{
	Tickable::PrintList(terminal);
}

//-----------------------------------------------------------------------------
// cmds
//-----------------------------------------------------------------------------
CmdLineEntry(cmds)
{
	CmdLine::PrintList(terminal);
}

//-----------------------------------------------------------------------------
// prompt str
//-----------------------------------------------------------------------------
CmdLineEntry(prompt)
{
	if (argc < 2) {
		terminal.Println(CmdLine::GetPrompt());
	} else {
		CmdLine::SetPrompt(argv[1]);
	}
}

}
