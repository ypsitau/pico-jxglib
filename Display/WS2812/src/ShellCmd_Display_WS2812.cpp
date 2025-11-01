#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Display/WS2812.h"

namespace jxglib::ShellCmd_Display_WS2812 {

Display::WS2812* pDisplay = nullptr;

ShellCmd_Named(display_ws2812, "display-ws2812", "initialize WS2812 display")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup  Set up WS2812 display with the given parameters:\n");
		terr.Printf("          {gpio:PIN [size:WxH]}\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	const char* value = nullptr;
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			if (pDisplay) {
				terr.Printf("WS2812 display already set up.\n");
				return Result::Error;
			}
			Size size;
			uint8_t seqDir = Image::SequencerDir::Normal;
			bool zigzagFlag = false;
			uint pinDIN = GPIO::InvalidPin;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				const char* valueSize = nullptr;
				if (Arg::GetAssigned(subcmd, "din", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinDIN = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "straight", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
					size = Size(num, 1);
				} else if (Arg::GetAssigned(subcmd, "straight-nw-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromNW; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-nw-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromNW; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-ne-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromNE; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-ne-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromNE; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-sw-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromSW; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-sw-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromSW; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-se-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromSE; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "straight-se-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromSE; valueSize = value;
				} else if (Arg::GetAssigned(subcmd, "zigzag-nw-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromNW; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-nw-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromNW; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-ne-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromNE; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-ne-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromNE; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-sw-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromNW; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-sw-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromNW; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-se-horz", &value)) {
					seqDir = Image::SequencerDir::HorzFromSE; valueSize = value; zigzagFlag = true;
				} else if (Arg::GetAssigned(subcmd, "zigzag-se-vert", &value)) {
					seqDir = Image::SequencerDir::VertFromSE; valueSize = value; zigzagFlag = true;
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
				if (valueSize && !size.Parse(valueSize)) {
					terr.Printf("invalid size: %s\n", valueSize);
					return Result::Error;
				}
			}
			if (pinDIN == GPIO::InvalidPin) {
				terr.Printf("din pin must be specified\n");
				return Result::Error;
			}
			if (size.IsZero()) {
				terr.Printf("display layout must be specified\n");
				return Result::Error;
			}
			pDisplay = new Display::WS2812(size.width, size.height, seqDir, zigzagFlag);
			pDisplay->Initialize(GPIO::Instance(pinDIN));
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

}

