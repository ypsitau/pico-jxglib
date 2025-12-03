#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Camera/OV7670.h"

namespace jxglib::Camera::ShellCmd_Camera_OV7670 {

OV7670* pOV7670 = nullptr;

ShellCmd(ov7670, "controls OV7670")
{
	bool errorFlag = false;
	auto ModifyRegBit = [&](const char* subcmd, const char* name, int reg, int iBit) -> bool {
		OV7670& ov7670 = *pOV7670;
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
		terr.Printf(" setup  setup a OV7670 device with the given parameters\n");
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
			OV7670::Resolution resolution = OV7670::QVGA;
			OV7670::Format format = OV7670::RGB565;
			i2c_inst_t* i2c = nullptr;
			uint pinDIN0 = GPIO::InvalidPin;
			uint pinXLK = GPIO::InvalidPin;
			uint pinPLK = GPIO::InvalidPin;
			uint pinHREF = GPIO::InvalidPin;
			uint pinVSYNC = GPIO::InvalidPin;
			uint32_t freq = 24000000;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "reso", &value) || Arg::GetAssigned(subcmd, "resolution", &value)) {
					if (::strcasecmp(value, "vga") == 0) {
						resolution = OV7670::VGA;
					} else if (::strcasecmp(value, "qvga") == 0) {
						resolution = OV7670::QVGA;
					} else if (::strcasecmp(value, "qqvga") == 0) {
						resolution = OV7670::QQVGA;
					} else if (::strcasecmp(value, "cif") == 0) {
						resolution = OV7670::CIF;
					} else if (::strcasecmp(value, "qcif") == 0) {
						resolution = OV7670::QCIF;
					} else if (::strcasecmp(value, "qqcif") == 0) {
						resolution = OV7670::QQCIF;
					} else {
						terr.Printf("unknown resolution: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "format", &value)) {
					if (::strcasecmp(value, "raw-rgb") == 0) {
						format = OV7670::RawBayerRGB;
					} else if (::strcasecmp(value, "processed-rgb") == 0) {
						format = OV7670::ProcessedBayerRGB;
					} else if (::strcasecmp(value, "yuv422") == 0) {
						format = OV7670::YUV422;
					} else if (::strcasecmp(value, "grb422") == 0) {
						format = OV7670::GRB422;
					} else if (::strcasecmp(value, "rgb565") == 0) {
						format = OV7670::RGB565;
					} else if (::strcasecmp(value, "rgb555") == 0) {
						format = OV7670::RGB555;
					} else if (::strcasecmp(value, "rgb444") == 0) {
						format = OV7670::RGB444;
					} else {
						terr.Printf("unknown format: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "i2c", &value)) {
					if (::strcasecmp(value, "0") == 0) {
						i2c = i2c0;
					} else if (::strcasecmp(value, "1") == 0) {
						i2c = i2c1;
					} else {
						terr.Printf("unknown i2c bus: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "din0", &value) ||
							Arg::GetAssigned(subcmd, "data0", &value)) {
					if (!value) {
						terr.Printf("din0 pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinDIN0 = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "xlk", &value) ||
							Arg::GetAssigned(subcmd, "xclk", &value)) {
					if (!value) {
						terr.Printf("xlk pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinXLK = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "plk", &value) ||
							Arg::GetAssigned(subcmd, "pclk", &value)) {
					if (!value) {
						terr.Printf("plk pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinPLK = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "href", &value)) {
					if (!value) {
						terr.Printf("href pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinHREF = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "vsync", &value)) {
					if (!value) {
						terr.Printf("vsync pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinVSYNC = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "freq", &value)) {
					if (!value) {
						terr.Printf("freq not specified.\n");
						return Result::Error;
					}
					freq = ::strtoul(value, nullptr, 0);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!i2c) {
				terr.Printf("i2c bus is not specified.\n");
				return Result::Error;
			}
			if (pinDIN0 == GPIO::InvalidPin || pinXLK == GPIO::InvalidPin || pinPLK == GPIO::InvalidPin ||
				pinHREF == GPIO::InvalidPin || pinVSYNC == GPIO::InvalidPin) {
				terr.Printf("all of din0, xlk, plk, href, and vsync pins must be specified.\n");
				return Result::Error;
			}
			if (pOV7670) delete pOV7670;
			pOV7670 = new OV7670(resolution, format, i2c0, {
				DIN0: GPIO::Instance(pinDIN0), XLK: GPIO::Instance(pinXLK), PLK: GPIO::Instance(pinPLK),
				HREF: GPIO::Instance(pinHREF), VSYNC: GPIO::Instance(pinVSYNC)}, freq);
			pOV7670->Initialize();
			pOV7670->SetupRegisters();
			pOV7670->EnableColorMode(true);
			continue;
		}
		if (!pOV7670) {
			terr.Printf("OV7670 device is not initialized. Run 'ov7670 setup' first.\n");
			return Result::Error;
		}
		OV7670& ov7670 = *pOV7670;
		if (::strcasecmp(subcmd, "dump") == 0) {
			uint8_t data[0xca];
			ov7670.ReadRegs(0x00, data, sizeof(data));
			Printable::DumpT dump(tout);
			dump(data, sizeof(data));
		} else if (
				ModifyRegBit(subcmd, "ccir656",					OV7670::Reg04_COM1,		6) ||
				ModifyRegBit(subcmd, "scale",					OV7670::Reg0C_COM3,		3) ||
				ModifyRegBit(subcmd, "dcw",						OV7670::Reg0C_COM3,		2) ||
				ModifyRegBit(subcmd, "color-bar",				OV7670::Reg12_COM7,		1) ||
				ModifyRegBit(subcmd, "agc",						OV7670::Reg13_COM8,		2) ||
				ModifyRegBit(subcmd, "awb",						OV7670::Reg13_COM8,		1) ||
				ModifyRegBit(subcmd, "aec",						OV7670::Reg13_COM8,		0) ||
				ModifyRegBit(subcmd, "mirror",					OV7670::Reg1E_MVFP,		5) ||
				ModifyRegBit(subcmd, "vflip",					OV7670::Reg1E_MVFP,		4) ||
				ModifyRegBit(subcmd, "black-sun",				OV7670::Reg1E_MVFP,		2) ||
				ModifyRegBit(subcmd, "negative-image",			OV7670::Reg3A_TSLB,		5) ||
				ModifyRegBit(subcmd, "auto-output-window",		OV7670::Reg3A_TSLB,		0) ||
				ModifyRegBit(subcmd, "night-mode",				OV7670::Reg3B_COM11,	7) ||
				ModifyRegBit(subcmd, "d56-auto-detection",		OV7670::Reg3B_COM11,	4) ||
				ModifyRegBit(subcmd, "auto-sharpness",			OV7670::Reg41_COM16,	5) ||
				ModifyRegBit(subcmd, "auto-denoise",			OV7670::Reg41_COM16,	4) ||
				ModifyRegBit(subcmd, "dsp-color-bar",			OV7670::Reg42_COM17,	3) ||
				ModifyRegBit(subcmd, "uv-average",				OV7670::Reg4B,			0) ||
				ModifyRegBit(subcmd, "auto-contrast-center",	OV7670::Reg58_MTXS,		7) ||
				ModifyRegBit(subcmd, "lens-correction",			OV7670::Reg66_LCC5,		2) ||
				ModifyRegBit(subcmd, "black-pixel-correction",	OV7670::Reg76,			7) ||
				ModifyRegBit(subcmd, "white-pixel-correction",	OV7670::Reg76,			6) ||
				ModifyRegBit(subcmd, "rgb444",					OV7670::Reg8C_RGB444,	1) ||
				ModifyRegBit(subcmd, "ablc",					OV7670::RegB1_ABLC1,	2)) {
			if (errorFlag) return Result::Error;
		} else if (Arg::GetAssigned(subcmd, "brightness", &value)) {
			if (!value) {
				int8_t num = static_cast<int8_t>(ov7670.ReadReg(OV7670::Reg55_BRIGHT));
				tout.Printf("brightness:%s%d\n", (num & (0b1 << 7))? "-" : "", num & 0x7f);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < -127 || num > 127) {
					terr.Printf("invalid brightness value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg55_BRIGHT,
					(num < 0)? ((0b1 << 7) | static_cast<uint8_t>(-num)) : static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "contrast", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg56_CONTRAS);
				tout.Printf("contrast:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid contrast value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg56_CONTRAS, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "sharpness", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg3F_EDGE);
				tout.Printf("sharpness:%u\n", num & 0x1f);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 31) {
					terr.Printf("invalid sharpness value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg3F_EDGE, static_cast<uint8_t>(num & 0x1f));
			}
		} else if (Arg::GetAssigned(subcmd, "sharpness-max", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg75);
				tout.Printf("sharpness-max:%u\n", num & 0x1f);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 31) {
					terr.Printf("invalid sharpness-max value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg75, static_cast<uint8_t>(num & 0x1f));
			}
		} else if (Arg::GetAssigned(subcmd, "sharpness-min", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg76);
				tout.Printf("sharpness-min:%u\n", num & 0x1f);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 31) {
					terr.Printf("invalid sharpness-min value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg76,
					(ov7670.ReadReg(OV7670::Reg76) & 0b11100000) | static_cast<uint8_t>(num & 0x1f));
			}
		} else if (Arg::GetAssigned(subcmd, "denoise", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg4C_DNSTH);
				tout.Printf("denoise:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid denoise value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg4C_DNSTH, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "denoise-offset", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg77);
				tout.Printf("denoise-offset:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid denoise-offset value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg77, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "awb-blue", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg01_BLUE);
				tout.Printf("awb-blue:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid awb-blue value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg01_BLUE, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "awb-red", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg02_RED);
				tout.Printf("awb-red:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid awb-red value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg02_RED, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "awb-green", &value)) {
			if (!value) {
				uint8_t num = ov7670.ReadReg(OV7670::Reg6A_GGAIN);
				tout.Printf("awb-green:%u\n", num);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 255) {
					terr.Printf("invalid awb-green value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg6A_GGAIN, static_cast<uint8_t>(num));
			}
		} else if (Arg::GetAssigned(subcmd, "exposure", &value)) {
			if (!value) {
				uint8_t high = ov7670.ReadReg(OV7670::Reg07_AECHH);
				uint8_t low = ov7670.ReadReg(OV7670::Reg10_AECH);
				uint16_t exposure = (static_cast<uint16_t>(high) << 10) | (static_cast<uint16_t>(low) << 2) |
					(ov7670.ReadReg(OV7670::Reg04_COM1) & 0b11);
				tout.Printf("exposure:%u\n", exposure);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 65535) {
					terr.Printf("invalid exposure value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg07_AECHH, static_cast<uint8_t>((num >> 10) & 0xff));
				ov7670.WriteReg(OV7670::Reg10_AECH, static_cast<uint8_t>((num >> 2) & 0xff));
				ov7670.WriteReg(OV7670::Reg04_COM1,
					(ov7670.ReadReg(OV7670::Reg04_COM1) & 0b11111100) | static_cast<uint8_t>(num & 0b11));
			}
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

}
