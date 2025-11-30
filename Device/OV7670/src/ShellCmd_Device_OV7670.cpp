#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Device/OV7670.h"

jxglib::Device::OV7670& ShellCmd_Device_OV7670_GetOV7670();

namespace jxglib::ShellCmd_Device_OV7670 {

ShellCmd(ov7670, "controls OV7670")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	const char* value = nullptr;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup                setup a OV7670 device with the given parameters\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	Device::OV7670& ov7670 = ShellCmd_Device_OV7670_GetOV7670();
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				//const char* subcmd = pSubcmdChild->GetProc();
				//if (Arg::GetAssigned(subcmd, "din", &value)) {
				//	int num = ::strtol(value, nullptr, 0);
				//	if (num < 0 || num >= GPIO::NumPins) {
				//		terr.Printf("invalid GPIO number: %s\n", value);
				//		return Result::Error;
				//	}
				//	pinDIN = static_cast<uint>(num);
				//} else {
				//	terr.Printf("unknown sub command: %s\n", subcmd);
				//	return Result::Error;
				//}
			}
		} else if (::strcasecmp(subcmd, "dump") == 0) {
			uint8_t data[0xca];
			ov7670.ReadRegs(0x00, data, sizeof(data));
			Printable::DumpT dump(tout);
			dump(data, sizeof(data));
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

}
