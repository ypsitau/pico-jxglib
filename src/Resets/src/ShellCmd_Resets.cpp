//==============================================================================
// ShellCmd_Resets.cpp
//==============================================================================
#include "jxglib/Shell.h"
#include "jxglib/Resets.h"

namespace jxglib::ShellCmd_Resets {

struct ResetBlock {
	const char* name;
	int iBit;
public:
	static int NameToBit(const char* name);
	static const char* BitToName(int iBit);
};

#if defined(PICO_RP2040)
static const ResetBlock resetBlockTbl[] = {
	{"adc",			RESET_ADC},
	{"busctrl",		RESET_BUSCTRL},
	{"dma",			RESET_DMA},
	{"i2c0",		RESET_I2C0},
	{"i2c1",		RESET_I2C1},
	{"io_bank0",	RESET_IO_BANK0},
	{"io_qspi",		RESET_IO_QSPI},
	{"jtag",		RESET_JTAG},
	{"pads_bank0",	RESET_PADS_BANK0},
	{"pads_qspi",	RESET_PADS_QSPI},
	{"pio0",		RESET_PIO0},
	{"pio1",		RESET_PIO1},
	{"pll_sys",		RESET_PLL_SYS},
	{"pll_usb",		RESET_PLL_USB},
	{"pwm",			RESET_PWM},
	{"rtc",			RESET_RTC},
	{"spi0",		RESET_SPI0},
	{"spi1",		RESET_SPI1},
	{"syscfg",		RESET_SYSCFG},
	{"sysinfo",		RESET_SYSINFO},
	{"tbman",		RESET_TBMAN},
	{"timer",		RESET_TIMER},
	{"uart0",		RESET_UART0},
	{"uart1",		RESET_UART1},
	{"usbctrl",		RESET_USBCTRL}
};
#endif
#if defined(PICO_RP2350)
static const ResetBlock resetBlockTbl[] = {
	{"adc",			RESET_ADC},
	{"busctrl",		RESET_BUSCTRL},
	{"dma",			RESET_DMA},
	{"hstx",		RESET_HSTX},
	{"i2c0",		RESET_I2C0},
	{"i2c1",		RESET_I2C1},
	{"io_bank0",	RESET_IO_BANK0},
	{"io_qspi",		RESET_IO_QSPI},
	{"jtag",		RESET_JTAG},
	{"pads_bank0",	RESET_PADS_BANK0},
	{"pads_qspi",	RESET_PADS_QSPI},
	{"pio0",		RESET_PIO0},
	{"pio1",		RESET_PIO1},
	{"pio2",		RESET_PIO2},
	{"pll_sys",		RESET_PLL_SYS},
	{"pll_usb",		RESET_PLL_USB},
	{"pwm",			RESET_PWM},
	{"sha256",		RESET_SHA256},
	{"spi0",		RESET_SPI0},
	{"spi1",		RESET_SPI1},
	{"syscfg",		RESET_SYSCFG},
	{"sysinfo",		RESET_SYSINFO},
	{"tbman",		RESET_TBMAN},
	{"timer0",		RESET_TIMER0},
	{"timer1",		RESET_TIMER1},
	{"trng",		RESET_TRNG},
	{"uart0",		RESET_UART0},
	{"uart1",		RESET_UART1},
	{"usbctrl",		RESET_USBCTRL}
};
#endif

ShellCmd(reset, "resets specified blocks")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("quiet",		'q',	"quiet mode"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		tout.Printf("Usage: %s [OPTION]... BLOCK...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Available blocks:\n");
		int nCols = 0;
		for (const ResetBlock& block : resetBlockTbl) {
			int len = ::strlen(block.name) + 1;
			if (nCols + len > 70) {
				tout.Println();
				nCols = 0;
			}
			nCols += len;
			tout.Printf(" %s", block.name);
		}
		tout.Println();
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Arg::Each each(argv[1], argv[argc]);
	bool quietFlag = arg.GetBool("quiet");
	uint mask = 0;
	while (const char* arg = each.Next()) {
		int iBit = ResetBlock::NameToBit(arg);
		if (iBit < 0) {
			terr.Printf("Unknown reset block: %s\n", arg);
			return Result::Error;
		}
		mask |= (1 << iBit);
	}
	ResetUnresetBlockingMask(mask);
	if (!quietFlag) {
		tout.Printf("reset:");
		for (int iBit = 0; iBit < 32; iBit++) {
			if (mask & (1 << iBit)) {
				tout.Printf(" %s", ResetBlock::BitToName(iBit));
			}
		}
		tout.Println();
	}
	return Result::Success;
}

int ResetBlock::NameToBit(const char* name)
{
	for (const ResetBlock& block : resetBlockTbl) {
		if (::strcasecmp(name, block.name) == 0) return block.iBit;
	}
	return -1;
}

const char* ResetBlock::BitToName(int iBit)
{
	for (const ResetBlock& block : resetBlockTbl) {
		if (block.iBit == iBit) return block.name;
	}
	return "unknown";
}

}
