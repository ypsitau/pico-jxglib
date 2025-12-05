#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Camera/OV7670.h"
#include "jxglib/I2C.h"

namespace jxglib::Camera::ShellCmd_Camera_OV7670 {

OV7670* pOV7670 = nullptr;

ShellCmd_Named(camera_ov7670, "camera-ov7670", "controls OV7670 camera module")
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
		terr.Printf(" setup                              setup a OV7670 camera module with the given parameters:\n");
		terr.Printf("                                    {reso:RESO format:FORMAT i2c:I2C_BUS\n");
		terr.Printf("                                     d0:PIN xclk:PIN pclk:PIN href:PIN vsync:PIN freq:FREQ}\n");
		terr.Printf(" dump                               dumps all OV7670 registers\n");
		terr.Printf(" ccir656:[on|off]                   enables/disables CCIR656 mode\n");
		terr.Printf(" scale:[on|off]                     enables/disables scaling\n");
		terr.Printf(" dcw:[on|off]                       enables/disables DCW\n");
		terr.Printf(" color-bar:[on|off]                 enables/disables color bar\n");
		terr.Printf(" agc:[on|off]                       enables/disables AGC\n");
		terr.Printf(" awb:[on|off]                       enables/disables AWB\n");
		terr.Printf(" aec:[on|off]                       enables/disables AEC\n");
		terr.Printf(" mirror:[on|off]                    enables/disables mirror\n");
		terr.Printf(" vflip:[on|off]                     enables/disables vertical flip\n");
		terr.Printf(" black-sun:[on|off]                 enables/disables black sun detection\n");
		terr.Printf(" negative-image:[on|off]            enables/disables negative image\n");
		terr.Printf(" auto-output-window:[on|off]        enables/disables auto output window\n");
		terr.Printf(" night-mode:[on|off]                enables/disables night mode\n");
		terr.Printf(" d56-auto-detection:[on|off]        enables/disables D56 auto detection\n");
		terr.Printf(" auto-sharpness:[on|off]            enables/disables auto sharpness\n");
		terr.Printf(" auto-denoise:[on|off]              enables/disables auto denoise\n");
		terr.Printf(" dsp-color-bar:[on|off]             enables/disables DSP color bar\n");
		terr.Printf(" uv-average:[on|off]                enables/disables UV average\n");
		terr.Printf(" auto-contrast-center:[on|off]      enables/disables auto contrast center\n");
		terr.Printf(" lens-correction:[on|off]           enables/disables lens correction\n");
		terr.Printf(" black-pixel-correction:[on|off]    enables/disables black pixel correction\n");
		terr.Printf(" white-pixel-correction:[on|off]    enables/disables white pixel correction\n");
		terr.Printf(" rgb444:[on|off]                    enables/disables RGB444\n");
		terr.Printf(" ablc:[on|off]                      enables/disables ABLC\n");
		terr.Printf(" brightness:[VALUE]                 gets/sets brightness (-127 to 127)\n");
		terr.Printf(" contrast:[VALUE]                   gets/sets contrast (0 to 255)\n");
		terr.Printf(" sharpness:[VALUE]                  gets/sets sharpness (0 to 31)\n");
		terr.Printf(" sharpness-max:[VALUE]              gets/sets sharpness max (0 to 31)\n");
		terr.Printf(" sharpness-min:[VALUE]              gets/sets sharpness min (0 to 31)\n");
		terr.Printf(" denoise:[VALUE]                    gets/sets denoise (0 to 255)\n");
		terr.Printf(" denoise-offset:[VALUE]             gets/sets denoise offset (0 to 255)\n");
		terr.Printf(" awb-blue:[VALUE]                   gets/sets AWB blue gain (0 to 255)\n");
		terr.Printf(" awb-red:[VALUE]                    gets/sets AWB red gain (0 to 255)\n");
		terr.Printf(" awb-green:[VALUE]                  gets/sets AWB green gain (0 to 255)\n");
		terr.Printf(" exposure:[VALUE]                   gets/sets exposure (0 to 65535)\n");
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
			if (pOV7670) {
				terr.Printf("OV7670 camera module is already setup.\n");
				return Result::Error;
			}
			OV7670::Resolution resolution = OV7670::QVGA;
			OV7670::Format format = OV7670::RGB565;
			int iI2C = -1;
			uint pinD0 = GPIO::InvalidPin;
			uint pinXCLK = GPIO::InvalidPin;
			uint pinPCLK = GPIO::InvalidPin;
			uint pinHREF = GPIO::InvalidPin;
			uint pinVSYNC = GPIO::InvalidPin;
			uint32_t freq = 24000000;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "i2c", &value)) {
					if (::strcasecmp(value, "0") == 0) {
						iI2C = 0;
					} else if (::strcasecmp(value, "1") == 0) {
						iI2C = 1;
					} else {
						terr.Printf("unknown i2c bus: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "d0", &value)) {
					if (!value) {
						terr.Printf("d0 pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinD0 = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "xclk", &value)) {
					if (!value) {
						terr.Printf("xlk pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinXCLK = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "pclk", &value)) {
					if (!value) {
						terr.Printf("plk pin not specified.\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinPCLK = static_cast<uint>(num);
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
			if (iI2C < 0) {
				terr.Printf("i2c bus is not specified.\n");
				return Result::Error;
			}
			if (pinD0 == GPIO::InvalidPin || pinXCLK == GPIO::InvalidPin || pinPCLK == GPIO::InvalidPin ||
				pinHREF == GPIO::InvalidPin || pinVSYNC == GPIO::InvalidPin) {
				terr.Printf("all of d0, xclk, pclk, href, and vsync pins must be specified.\n");
				return Result::Error;
			}
			I2C& i2c = I2C::get_instance(iI2C);
			pOV7670 = new OV7670(i2c, {
				D0: GPIO::Instance(pinD0), XCLK: GPIO::Instance(pinXCLK), PCLK: GPIO::Instance(pinPCLK),
				HREF: GPIO::Instance(pinHREF), VSYNC: GPIO::Instance(pinVSYNC)}, freq);
			pOV7670->Initialize();
			pOV7670->SetupRegisters();
			pOV7670->EnableColorMode(true);
			continue;
		}
		if (!pOV7670) {
			terr.Printf("OV7670 camera module is not initialized. Run 'ov7670 setup' first.\n");
			return Result::Error;
		}
		OV7670& ov7670 = *pOV7670;
		if (::strcasecmp(subcmd, "dump") == 0) {
			uint8_t data[0xca];
			ov7670.ReadRegs(0x00, data, sizeof(data));
			Printable::DumpT dump(tout);
			dump(data, sizeof(data));
		} else if (Arg::GetAssigned(subcmd, "reso", &value) || Arg::GetAssigned(subcmd, "resolution", &value)) {
			OV7670::Resolution resolution;
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
			ov7670.SetResolution(resolution);
		} else if (Arg::GetAssigned(subcmd, "format", &value)) {
			OV7670::Format format;
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
			ov7670.SetFormat(format);
		} else if (ModifyRegBit(subcmd, "ccir656",				OV7670::Reg04_COM1,		6) ||
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
		} else if (Arg::GetAssigned(subcmd, "dummy-pixels", &value)) {
			if (!value) {
				uint8_t high = (ov7670.ReadReg(OV7670::Reg2A_EXHCH) >> 4) & 0x0f;;
				uint8_t low = ov7670.ReadReg(OV7670::Reg2B_EXHCL);
				uint16_t dummyPixels = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
				tout.Printf("dummy-pixels:%u\n", dummyPixels);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 4095) {
					terr.Printf("invalid dummy-pixels value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg2A_EXHCH,
					(ov7670.ReadReg(OV7670::Reg2A_EXHCH) & 0x0f) | static_cast<uint8_t>((num >> 4) & 0xf0));
				ov7670.WriteReg(OV7670::Reg2B_EXHCL, static_cast<uint8_t>(num & 0xff));
			}
		} else if (Arg::GetAssigned(subcmd, "dummy-rows", &value)) {
			if (!value) {
				uint8_t high = ov7670.ReadReg(OV7670::Reg93_DM_LNH);
				uint8_t low = ov7670.ReadReg(OV7670::Reg92_DM_LNL);
				uint16_t dummyRows = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
				tout.Printf("dummy-rows:%u\n", dummyRows);
			} else {
				int num = ::strtol(value, nullptr, 0);
				if (num < 0 || num > 65535) {
					terr.Printf("invalid dummy-rows value: %s\n", value);
					return Result::Error;
				}
				ov7670.WriteReg(OV7670::Reg93_DM_LNH, static_cast<uint8_t>((num >> 8) & 0xff));
				ov7670.WriteReg(OV7670::Reg92_DM_LNL, static_cast<uint8_t>(num & 0xff));
			}
		} else if (Arg::GetAssigned(subcmd, "dummy-row-pos", &value)) {
			if (!value) {
				uint8_t num = (ov7670.ReadReg(OV7670::Reg4D_DM_POS) >> 7) & 0b1;
				tout.Printf("dummy-row-pos:%s\n", num? "after" : "before");
			} else if (::strcasecmp(value, "after") == 0) {
				ov7670.WriteRegBit(OV7670::Reg4D_DM_POS, 7, 0b1);
			} else if (::strcasecmp(value, "before") == 0) {
				ov7670.WriteRegBit(OV7670::Reg4D_DM_POS, 7, 0b0);
			} else {
				terr.Printf("invalid dummy-row-pos value: %s\n", value);
				return Result::Error;
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
