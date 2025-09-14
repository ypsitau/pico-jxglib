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
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	tout.Printf("%s %s %d MHz\n", GetPlatformName(), GetCPUArchName(), ::clock_get_hz(clk_sys) / 1000000);
	tout.Printf("Flash  0x%08X-0x%08X %7d\n", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES, PICO_FLASH_SIZE_BYTES); 
	tout.Printf("SRAM   0x20000000-0x%p %7d\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
	return Result::Success;
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Named(about_me, "about-me", "prints information about this own program")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	BinaryInfo::PrintProgramInformation(tout);    
	tout.Println();
	BinaryInfo::PrintFixedPinInformation(tout);    
	tout.Println();
	BinaryInfo::PrintBuildInformation(tout);    
	tout.Println();
	BinaryInfo::PrintMemoryMap(tout);    
	//BinaryInfo::Print(tout);
	return Result::Success;
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
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptInt("addr-digits",	0x0,	"specifies address digits", "n"),
		Arg::OptBool("addr",		0x0,	"prints address"),
		Arg::OptBool("ascii",		0x0,	"prints ASCII characters"),
		Arg::OptBool("no-addr",		0x0,	"prints no address"),
		Arg::OptBool("no-ascii",	0x0,	"prints no ASCII characters"),
		Arg::OptBool("no-dump",		0x0,	"skips actual dump, just modifies parameters"),
		Arg::OptInt("row-bytes",	'c',	"specifies number of bytes per row", "n"),
		Arg::OptBool("hex8",		0x0,	"prints data in 16-bit hex format"),
		Arg::OptBool("hex16",		0x0,	"prints data in 16-bit hex format"),
		Arg::OptBool("hex32",		0x0,	"prints data in 32-bit hex format"),
		Arg::OptBool("hex16be",		0x0,	"prints data in 16-bit big-endian hex format"),
		Arg::OptBool("hex32be",		0x0,	"prints data in 32-bit big-endian hex format"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [ADDR [BYTES]]\n", GetName());
		terr.Printf("       %s [OPTION]... FILE\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	if (arg.GetBool("addr")) dump_.Addr();
	if (arg.GetBool("no-addr")) dump_.Addr(false);
	if (arg.GetBool("ascii")) dump_.Ascii();
	if (arg.GetBool("no-ascii")) dump_.Ascii(false);
	int num;
	if (arg.GetInt("addr-digits", &num)) dump_.DigitsAddr(num);
	if (arg.GetInt("row-bytes", &bytesPerRow_)) {
		if (bytesPerRow_ > 32) {
			terr.Printf("invalid number of columns: %d\n", bytesPerRow_);
			return Result::Error;
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
	bool readFromFile = false;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p == '\0') {
			addr_ = num;
			if (argc >= 3) {
				uint32_t num = ::strtoul(argv[2], &p, 0);
				if (*p != '\0') {
					terr.Printf("invalid number\n");
					return Result::Error;
				}
				bytes_ = num;
			}
		} else {
			readFromFile = true;;
		}
	}
	dump_.SetPrintable(tout).BytesPerRow(bytesPerRow);
	if (arg.GetBool("no-dump")) {
		// nothing to do
	} else if (readFromFile) {
		uint8_t buff[512];
		for (Arg::EachGlob argIter(argv[1], argv[argc]); const char* fileName = argIter.Next(); ) {
			std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
			if (!pFile) {
				terr.Printf("cannot open file '%s'\n", argv[1]);
				return Result::Error;
			}
			int bytesToRead = sizeof(buff) / bytesPerRow * bytesPerRow;
			int bytesRead;
			uint32_t addr = 0;
			while ((bytesRead = pFile->Read(buff, bytesToRead)) > 0) {
				dump_.AddrStart(addr)(buff, bytesRead);
				addr += bytesRead;
			}
		}
	} else {
		dump_.AddrStart(addr_)(reinterpret_cast<const void*>(addr_), bytes_);
		addr_ += bytes_;
	}
	return Result::Success;
}

ShellCmdAlias(d, dump)

//-----------------------------------------------------------------------------
// .
//-----------------------------------------------------------------------------
ShellCmd_Named(dot, ".", "executes the given script file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [SCRIPT]\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	const char* fileName = argv[1];
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		terr.Printf("cannot open file '%s'\n", fileName);
		return Result::Error;
	}
	Shell::Instance.RunScript(tin, tout, terr, *pFile);
	return Result::Success;
}

//-----------------------------------------------------------------------------
// echo
//-----------------------------------------------------------------------------
ShellCmd(echo, "prints the given text")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptBool("no-eol",	'n',	"does not print end-of-line"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [TEXT]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	for (int i = 1; i < argc; ++i) {
		if (i > 1) tout.Print(" ");
		tout.Print(argv[i]);
	}
	if (!arg.GetBool("no-eol")) tout.Println();
	return Result::Success;
}

//-----------------------------------------------------------------------------
// echo-bin
//-----------------------------------------------------------------------------
ShellCmd_Named(echo_bin, "echo-bin", "generates binary data from the given arguments")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("hex",			'x',	"prints data in hex format"),
		//Arg::OptBool("int8",		0x0,	"generates data in 8-bit integer format"),
		//Arg::OptBool("int16",		0x0,	"generates data in 16-bit integer format"),
		//Arg::OptBool("int32",		0x0,	"generates data in 32-bit integer format"),
		//Arg::OptBool("int16be",	0x0,	"generates data in 16-bit big-endian integer format"),
		//Arg::OptBool("int32be",	0x0,	"generates data in 32-bit big-endian integer format"),
		//Arg::OptBool("uint8",		0x0,	"generates data in unsinged 8-bit integer format"),
		//Arg::OptBool("uint16",	0x0,	"generates data in unsinged 16-bit integer format"),
		//Arg::OptBool("uint32",	0x0,	"generates data in unsinged 32-bit integer format"),
		//Arg::OptBool("uint16be",	0x0,	"generates data in unsinged 16-bit big-endian integer format"),
		//Arg::OptBool("uint32be",	0x0,	"generates data in unsinged 32-bit big-endian integer format"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [NUMBER]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	bool hexFlag = arg.GetBool("hex");
	Arg::EachNum eachNum(argv[1], argv[argc]);
	if (!eachNum.CheckValidity()) {
		terr.Printf("%s\n", eachNum.GetErrorMsg());
		return Result::Error;
	}
	int value;
	if (hexFlag) {
		int iCol = 0;
		while (eachNum.Next(&value)) {
			if (value < 0 || value > 255) {
				terr.Printf("value out of range: %d\n", value);
				return Result::Error;
			}
			tout.Printf("%s%02X%s", (iCol == 0)? "" : " ", value, (iCol == 15)? "\n" : "");
			if (++iCol >= 16) iCol = 0;
		}
		if (iCol > 0 && hexFlag) tout.Println();
	} else {
		while (eachNum.Next(&value)) {
			if (value < 0 || value > 255) {
				terr.Printf("value out of range: %d\n", value);
				return Result::Error;
			}
			tout.PutChar(value);
		}
	}
	return Result::Success;
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("simple",		's',	"prints only command names"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	bool simpleFlag = arg.GetBool("simple");
	Shell::PrintHelp(tout, simpleFlag);
	return Result::Success;
}

//-----------------------------------------------------------------------------
// history
//-----------------------------------------------------------------------------
ShellCmd(history, "prints the command history")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	Shell::PrintHistory(tout);
	return Result::Success;
}

//-----------------------------------------------------------------------------
// set
//-----------------------------------------------------------------------------
ShellCmd(set, "set environment variable")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [KEY [VALUE]]\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	Dict& dict = Shell::Instance.GetDict();
	if (argc < 2) {
		for (const Dict::Entry* pEntry = dict.GetFirst(); pEntry; pEntry = pEntry->GetNext()) {
			tout.Printf("%s=%s\n", pEntry->GetKey(), pEntry->GetValue());
		}
	} else if (argc < 3) {
		const char* value = dict.Lookup(argv[1]);
		if (value) {
			tout.Printf("%s\n", value);
		} else {
			terr.Printf("no such variable: %s\n", argv[1]);
			return Result::Error;
		}
	} else {
		dict.SetValue(argv[1], argv[2]);
	}
	return Result::Success;
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [PROMPT]\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Variables:\n");
		terr.Printf("  %%d - current drive\n");
		terr.Printf("  %%w - current directory\n");
		terr.Printf("  %%p - platform name\n");
		terr.Printf("  %%Y - year (4 digits)\n");
		terr.Printf("  %%y - year (2 digits)\n");
		terr.Printf("  %%M - month (1-12)\n");
		terr.Printf("  %%D - day (1-31)\n");
		terr.Printf("  %%h - hour (0-23)\n");
		terr.Printf("  %%H - hour (1-12)\n");
		terr.Printf("  %%m - minute (0-59)\n");
		terr.Printf("  %%s - second (0-59)\n");
		terr.Printf("  %%A - AM/PM\n");
		return Result::Error;
	}
	if (argc < 2) {
		tout.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
	return Result::Success;
}

//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
ShellCmd(sleep, "sleeps for the specified time in milliseconds")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [MS]\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	if (argc < 2) {
		terr.Printf("missing sleep time in milliseconds\n");
		return Result::Error;
	}
	char* endptr;
	int msec = ::strtol(argv[1], &endptr, 10);
	if (*endptr != '\0' || msec < 0) {
		terr.Printf("invalid sleep time: %s\n", argv[1]);
		return Result::Error;
	}
	Shell::Sleep(msec);
	return Result::Success;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Error;
	}
	tout.Printf("Tick Called Depth Max: %d\n", Tickable::GetTickCalledDepthMax());
	Tickable::PrintList(tout);
	return Result::Success;
}

}
