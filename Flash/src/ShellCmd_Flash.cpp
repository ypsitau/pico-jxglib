//==============================================================================
// ShellCmd_Flash.cpp
//==============================================================================
#include "jxglib/Shell.h"
#include "jxglib/Flash.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_Flash {

ShellCmd_Named(flash_erase, "flash-erase", "erases flash memory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool driveNameFlag = (argc > 1 && FS::IsLegalDriveName(argv[1]));
	if (arg.GetBool("help") || (!driveNameFlag && argc != 3)) {
		terr.Printf("Usage: %s [OPTION]... ADDR SIZE\n", GetName());
		terr.Printf("       %s [OPTION]... DRIVENAME...\n", GetName());
		arg.PrintHelp(terr);
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	if (driveNameFlag) {
		for (Arg::Each each(argv[1], argv[argc]); const char* driveName = each.Next(); ) {
			FS::Drive* pDrive = FS::FindDrive(driveName);
			if (!pDrive) {
				terr.Printf("unknown drive: %s\n", driveName);
				return Result::Error;
			}
			uint32_t addr, bytes;
			if (!pDrive->GetFlashInfo(&addr, &bytes)) {
				terr.Printf("drive %s does not support flash erase\n", driveName);
				return Result::Error;		
			}
			pDrive->Unmount();
			Flash::Erase(addr & 0x0fff'ffff, bytes);
			tout.Printf("erased flash of drive %s 0x%08x-0x%08x %dbytes\n", driveName, addr, addr + bytes, bytes);
		}
	} else { // argc == 3
		const char* strAddr = argv[1];
		const char* strBytes = argv[2];
		char* endPtr;
		uint32_t addr = ::strtoul(strAddr, &endPtr, 0);
		if (*endPtr != '\0') {
			terr.Printf("invalid addr: %s\n", strAddr);
			return Result::Error;
		}
		uint32_t bytes = ::strtoul(strBytes, &endPtr, 0);
		if (*endPtr != '\0') {
			terr.Printf("invalid size: %s\n", strBytes);
			return Result::Error;
		}
		Flash::Erase(addr & 0x0fff'ffff, bytes);
		tout.Printf("erased flash 0x%08x-0x%08x %dbytes\n", addr, addr + bytes, bytes);
	}
	return Result::Success;
}

}
