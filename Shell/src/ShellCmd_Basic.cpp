//==============================================================================
// ShellCmd_Basic.cpp
//==============================================================================
#include <stdlib.h>
#include "pico/binary_info.h"
#include "hardware/clocks.h"
#include "jxglib/Shell.h"
#include "jxglib/BinaryInfo.h"

namespace jxglib::ShellCmd_Basic {

//-----------------------------------------------------------------------------
// about-platform
//-----------------------------------------------------------------------------
ShellCmd_Named(about_cpu, "about-platform", "prints information about the platform")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	tout.Printf("%s %s %d MHz\n", GetPlatformName(), GetCPUArchName(), ::clock_get_hz(clk_sys) / 1000000);
	tout.Printf("Flash  0x%08X-0x%08X %7d\n", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES, PICO_FLASH_SIZE_BYTES); 
	tout.Printf("SRAM   0x20000000-0x%p %7d\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
	return 0;
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Named(about_me, "about-me", "prints information about this own program")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	BinaryInfo::PrintProgramInformation(tout);    
	tout.Println();
	BinaryInfo::PrintFixedPinInformation(tout);    
	tout.Println();
	BinaryInfo::PrintBuildInformation(tout);    
	tout.Println();
	BinaryInfo::PrintMemoryMap(tout);    
	//BinaryInfo::Print(tout);
	return 0;
}

//-----------------------------------------------------------------------------
// dump addr bytes
//-----------------------------------------------------------------------------
class ShellCmd_dump : public Shell::Cmd {
private:
	uint32_t addr_;
	uint32_t bytes_;
	int bytesPerRow_;
	Printable::DumpT dump_;
public:
	static ShellCmd_dump Instance;
public:
	ShellCmd_dump(const char* name) : Shell::Cmd(name, "prints memory content at the specified address"),
			addr_{0x00000000}, bytes_{64}, bytesPerRow_{-1} {}
public:
	virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) override;
};

ShellCmd_dump ShellCmd_dump::Instance("dump");

int ShellCmd_dump::Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[])
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
		Arg::OptInt("addr-digits",	"",		"specifies address digits", "n"),
		Arg::OptBool("ascii",		"",		"prints ASCII characters"),
		Arg::OptBool("no-ascii",	"",		"prints no ASCII characters"),
		Arg::OptBool("no-dump",		"",		"skips actual dump, just modifies parameters"),
		Arg::OptInt("row-bytes",	"c",	"specifies number of bytes per row", "n"),
		Arg::OptBool("hex8",		"",		"prints data in 16-bit hex format"),
		Arg::OptBool("hex16",		"",		"prints data in 16-bit hex format"),
		Arg::OptBool("hex32",		"",		"prints data in 32-bit hex format"),
		Arg::OptBool("hex16be",		"",		"prints data in 16-bit big-endian hex format"),
		Arg::OptBool("hex32be",		"",		"prints data in 32-bit big-endian hex format"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [addr [bytes]]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 0;
	}
	if (arg.GetBool("ascii")) dump_.Ascii();
	if (arg.GetBool("no-ascii")) dump_.Ascii(false);
	int num;
	if (arg.GetInt("addr-digits", &num)) dump_.DigitsAddr(num);
	if (arg.GetInt("row-bytes", &bytesPerRow_)) {
		if (bytesPerRow_ > 32) {
			terr.Printf("invalid number of columns: %d\n", bytesPerRow_);
			return 1;
		}
	}
	if (arg.GetBool("hex8")) dump_.Data8Bit();
	if (arg.GetBool("hex16")) dump_.Data16Bit();
	if (arg.GetBool("hex32")) dump_.Data32Bit();
	if (arg.GetBool("hex16be")) dump_.Data16BitBE();
	if (arg.GetBool("hex32be")) dump_.Data32BitBE();
	int nColsOut, nRowsOut;
	tout.GetSize(&nColsOut, &nRowsOut);
	nColsOut -= 8 + 2;
	int bytesPerRow = (bytesPerRow_ > 0)? bytesPerRow_ : ((nColsOut + 1) / 3) / 8 * 8;
	if (bytesPerRow == 0) bytesPerRow = 8;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		addr_ = num;
	}
	if (argc >= 3) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[2], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		bytes_ = num;
	}
	dump_.SetPrintable(tout).BytesPerRow(bytesPerRow).AddrStart(addr_);
	if (!arg.GetBool("no-dump")) {
		dump_(reinterpret_cast<const void*>(addr_), bytes_);
		addr_ += bytes_;
	}
	return 0;
}

ShellCmdAlias(d, dump)

//-----------------------------------------------------------------------------
// .
//-----------------------------------------------------------------------------
ShellCmd_Named(dot, ".", "executes the given script file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <script>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* fileName = argv[1];
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		terr.Printf("cannot open file '%s'\n", fileName);
		return 1;
	}
	Shell::Instance.RunScript(tin, tout, terr, *pFile);
	return 0;
}

//-----------------------------------------------------------------------------
// echo
//-----------------------------------------------------------------------------
ShellCmd(echo, "prints the given text")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	"h",	"prints this help"),
		Arg::OptBool("no-eol",	"n",	"does not print end-of-line"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [text...]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 0;
	}
	for (int i = 1; i < argc; ++i) {
		if (i > 1) tout.Print(" ");
		tout.Print(argv[i]);
	}
	if (!arg.GetBool("no-eol")) tout.Println();
	return 0;
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	Shell::PrintHelp(tout);
	return 0;
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [<prompt>]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	if (argc < 2) {
		tout.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	tout.Printf("Tick Called Depth Max: %d\n", Tickable::GetTickCalledDepthMax());
	Tickable::PrintList(tout);
	return 0;
}

}
