#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Device/OV7670.h"

jxglib::Device::OV7670& ShellCmd_Device_OV7670_GetOV7670();

namespace jxglib::ShellCmd_Device_OV7670 {

ShellCmd(ov7670, "controls OV7670")
{
	Device::OV7670& ov7670 = ShellCmd_Device_OV7670_GetOV7670();
	bool errorFlag = false;
	auto ModifyRegBit = [&](const char* subcmd, const char* name, int reg, int iBit) -> bool {
		const char* value = nullptr;
		if (!Shell::Arg::GetAssigned(subcmd, name, &value)) {
			return false;
		} else if (!value) {
			tout.Printf("%s:%s\n", name, ov7670.ReadRegBit(reg, iBit)? "on" : "off");
		} else if (Shell::Arg::IsBoolTrue(value)) {
			ov7670.WriteRegBit(reg, iBit, 0b1);
		} else if (Shell::Arg::IsBoolFalse(value)) {
			ov7670.WriteRegBit(reg, iBit, 0b0);
		} else {
			terr.Printf("invalid value for %s: %s\n", name, value);
			errorFlag = true;
		}
		return true;
	};
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
		} else if (
				ModifyRegBit(subcmd, "ccir656",					Device::OV7670::Reg04_COM1,		6) ||
				ModifyRegBit(subcmd, "scale",					Device::OV7670::Reg0C_COM3,		3) ||
				ModifyRegBit(subcmd, "dcw",						Device::OV7670::Reg0C_COM3,		2) ||
				ModifyRegBit(subcmd, "color-bar",				Device::OV7670::Reg12_COM7,		1) ||
				ModifyRegBit(subcmd, "agc",						Device::OV7670::Reg13_COM8,		2) ||
				ModifyRegBit(subcmd, "awb",						Device::OV7670::Reg13_COM8,		1) ||
				ModifyRegBit(subcmd, "aec",						Device::OV7670::Reg13_COM8,		0) ||
				ModifyRegBit(subcmd, "mirror",					Device::OV7670::Reg1E_MVFP,		5) ||
				ModifyRegBit(subcmd, "vflip",					Device::OV7670::Reg1E_MVFP,		4) ||
				ModifyRegBit(subcmd, "black-sun",				Device::OV7670::Reg1E_MVFP,		2) ||
				ModifyRegBit(subcmd, "negative-image",			Device::OV7670::Reg3A_TSLB,		5) ||
				ModifyRegBit(subcmd, "auto-output-window",		Device::OV7670::Reg3A_TSLB,		0) ||
				ModifyRegBit(subcmd, "night-mode",				Device::OV7670::Reg3B_COM11,	7) ||
				ModifyRegBit(subcmd, "d56-auto-detection",		Device::OV7670::Reg3B_COM11,	4) ||
				ModifyRegBit(subcmd, "edge-enhancement-threshold-auto-adjustment",	Device::OV7670::Reg41_COM16,	5) ||
				ModifyRegBit(subcmd, "denoise-threshold-auto-adjustment",			Device::OV7670::Reg41_COM16,	4) ||
				ModifyRegBit(subcmd, "dsp-color-bar",			Device::OV7670::Reg42_COM17,	3) ||
				ModifyRegBit(subcmd, "auto-contrast-center",	Device::OV7670::Reg58_MTXS,		7) ||
				ModifyRegBit(subcmd, "lens-correction",			Device::OV7670::Reg66_LCC5,		2) ||
				ModifyRegBit(subcmd, "black-pixel-correction",	Device::OV7670::Reg76,			7) ||
				ModifyRegBit(subcmd, "white-pixel-correction",	Device::OV7670::Reg76,			6) ||
				ModifyRegBit(subcmd, "rgb444",					Device::OV7670::Reg8C_RGB444,	1) ||
				ModifyRegBit(subcmd, "ablc",					Device::OV7670::RegB1_ABLC1,	2)) {
			if (errorFlag) return Result::Error;
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

}
