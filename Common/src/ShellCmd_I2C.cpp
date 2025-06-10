#include "hardware/i2c.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

ShellCmd_Named(i2c_scan, "i2c-scan", "scans I2C bus and prints connected addresses")
{
	enum class Func { SDA, SCL };
	struct Info { int iBus; Func func; };
	static const Info infoTbl[] = {
		{ 0, Func::SDA }, // GPIO0  I2C0 SDA
		{ 0, Func::SCL }, // GPIO1  I2C0 SCL
		{ 1, Func::SDA }, // GPIO2  I2C1 SDA
		{ 1, Func::SCL }, // GPIO3  I2C1 SCL
		{ 0, Func::SDA }, // GPIO4  I2C0 SDA
		{ 0, Func::SCL }, // GPIO5  I2C0 SCL
		{ 1, Func::SDA }, // GPIO6  I2C1 SDA
		{ 1, Func::SCL }, // GPIO7  I2C1 SCL
		{ 0, Func::SDA }, // GPIO8  I2C0 SDA
		{ 0, Func::SCL }, // GPIO9  I2C0 SCL
		{ 1, Func::SDA }, // GPIO10 I2C1 SDA
		{ 1, Func::SCL }, // GPIO11 I2C1 SCL
		{ 0, Func::SDA }, // GPIO12 I2C0 SDA
		{ 0, Func::SCL }, // GPIO13 I2C0 SCL
		{ 1, Func::SDA }, // GPIO14 I2C1 SDA
		{ 1, Func::SCL }, // GPIO15 I2C1 SCL
		{ 0, Func::SDA }, // GPIO16 I2C0 SDA
		{ 0, Func::SCL }, // GPIO17 I2C0 SCL
		{ 1, Func::SDA }, // GPIO18 I2C1 SDA
		{ 1, Func::SCL }, // GPIO19 I2C1 SCL
		{ 0, Func::SDA }, // GPIO20 I2C0 SDA
		{ 0, Func::SCL }, // GPIO21 I2C0 SCL
		{ 1, Func::SDA }, // GPIO22 I2C1 SDA
		{ 1, Func::SCL }, // GPIO23 I2C1 SCL
		{ 0, Func::SDA }, // GPIO24 I2C0 SDA
		{ 0, Func::SCL }, // GPIO25 I2C0 SCL
		{ 1, Func::SDA }, // GPIO26 I2C1 SDA
		{ 1, Func::SCL }, // GPIO27 I2C1 SCL
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
		Arg::OptString("baudrate",	"b",	"sets I2C frequency (default: 100000)", "FREQ"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 3 || arg.GetBool("help")) {
		tout.Printf("usage: %s OPTION... GPIO_SDA GPIO_SCL\n\n", argv[0]);
        tout.Printf("scans I2C bus and prints connected addresses\n\n");
		tout.Printf("Options:\n");
		arg.PrintHelp(tout);
		return 0;
	}
	uint baudrate = 100'000; // default frequency
	const char* value = nullptr;
	if (arg.GetString("baudrate", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 1'000'000) {
			tout.Printf("Invalid frequency: %s\n", value);
			return 1;
		}
		baudrate = static_cast<uint>(num);
	}
	uint gpioSDA = -1, gpioSCL = -1;
	for (int iArg = 1; iArg <= 2; iArg++) {
		char* endptr;
		long num = ::strtol(argv[iArg], &endptr, 0);
		if (*endptr != '\0' || num < 0 || num > 27) {
			tout.Printf("Invalid GPIO number: %s\n", argv[iArg]);
			return 1;
		}
		if (infoTbl[num].func == Func::SCL) {
			gpioSCL = static_cast<uint>(num);
		} else { // Func::SDA
			gpioSDA = static_cast<uint>(num);
		}
	}
	if (gpioSDA == -1 || gpioSCL == -1 || infoTbl[gpioSDA].iBus != infoTbl[gpioSCL].iBus) {
		tout.Printf("Invalid pair of GPIOs for I2C: %s %s\n", argv[1], argv[2]);
		return 1;
	}
	int iBus = infoTbl[gpioSDA].iBus;
	i2c_inst_t* i2c = (iBus == 0) ? i2c0 : i2c1;
	::i2c_init(i2c, baudrate);
	::gpio_set_function(gpioSDA, GPIO_FUNC_I2C);
	::gpio_set_function(gpioSCL, GPIO_FUNC_I2C);
	::gpio_pull_up(gpioSDA);
	::gpio_pull_up(gpioSCL);
	tout.Printf("Bus Scan on I2C %d (SDA:GPIO%d SCL:GPIO%d) @ %d Hz\n", iBus, gpioSDA, gpioSCL, baudrate);
	tout.Printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	int iCol = 0;
	for (int addr = 0; addr < (1 << 7); ++addr) {
		if (iCol == 0) tout.Printf("%02x ", addr);
		if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78) {
			tout.Printf("-- ");
		} else {
			uint8_t rxdata;
			int rtn = i2c_read_blocking(i2c, addr, &rxdata, sizeof(rxdata), false);
			if (rtn < 0) {
				tout.Printf("-- ");
			} else {
				tout.Printf("%02x ", addr);
			}
		}
		iCol++;
		if (iCol == 16) {
			tout.Printf("\n");
			iCol = 0;
		}
	}
	return 0;
}
