//==============================================================================
// Shell_BuiltIn.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Shell.h"

namespace jxglib::Shell_BuiltIn {

//-----------------------------------------------------------------------------
// d addr bytes
//-----------------------------------------------------------------------------
class ShellCmd_d : public Shell::Cmd {
private:
	uint32_t addr_;
	uint32_t bytes_;
public:
	static ShellCmd_d Instance;
public:
	ShellCmd_d() : Shell::Cmd("d", "prints memory content at the specified address"), addr_{0x00000000}, bytes_{64} {}
public:
	virtual void Run(Terminal& terminal, int argc, char* argv[]) override;
};

ShellCmd_d ShellCmd_d::Instance;

void ShellCmd_d::Run(Terminal& terminal, int argc, char* argv[])
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
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	Tickable::PrintList(terminal);
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	Shell::PrintHelp(terminal);
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	if (argc < 2) {
		terminal.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
}

//-----------------------------------------------------------------------------
// argtest
//-----------------------------------------------------------------------------
ShellCmd(argtest, "tests command line arguments")
{
	for (int i = 0; i < argc; i++) {
		terminal.Printf("argv[%d] \"%s\"\n", i, argv[i]);
	}
}

}
