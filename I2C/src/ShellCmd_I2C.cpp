//==============================================================================
// ShellCmd_I2C.cpp
//==============================================================================
#include "jxglib/I2C.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_I2C {

struct Config {
	uint SDA = -1;
	uint SCL = -1;
	uint freq = 100'000;
	uint32_t msecTimeout = 300;
	bool pullupFlag = true;  // Enable pull-up resistors by default
};

static Config configTbl[2];  // I2C0 and I2C1 configurations

static const int bytesDataBuff = 8192;
static uint8_t* dataBuff = nullptr;

static void AllocDataBuff();
static void ScanBus(Printable& tout, Printable& terr, I2C& i2c, int iBus, const Config& config);
static bool WriteData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop, uint32_t msecTimeout);
static bool ReadData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop, uint32_t msecTimeout);
static void PrintConfig(Printable& tout, int iBus, const char* formatGPIO);

ShellCmd_Named(i2c_, "i2c", "controls I2C bus communication")
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
		{ 0, Func::SDA }, // GPIO28 I2C0 SDA
		{ 0, Func::SCL }, // GPIO29 I2C0 SCL
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("help-pin",	0x0,	"prints help for pin assignment"),
		Arg::OptBool("dumb",		0x0,	"no I2C operations, just remember the pins and frequency"),
		Arg::OptBool("verbose",		'v',	"verbose output for debugging"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for I2C", "SDA,SCL"),
		Arg::OptString("freq",		'f',	"sets I2C frequency (default: 100000)", "FREQ"),
		Arg::OptString("pullup",	0x0,	"enables/disables internal pull-up resistors (default: enabled)", "on|off"),
		Arg::OptString("timeout",	't',	"sets timeout for I2C operations (default: 300)", "MSEC"),
	};
	bool hasArgs = (argc > 1);
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "i2c") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s BUS [OPTION]... scan\n", GetName());
			tout.Printf("       %s BUS [OPTION]... ADDR [COMMAND]...\n", GetName());
			tout.Printf("  BUS: I2C bus number (0 or 1)\n");
		} else {
			tout.Printf("Usage: %s [OPTION]... scane\n", GetName());
			tout.Printf("       %s [OPTION]... ADDR [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  scan                 scan I2C bus for connected devices\n");
		tout.Printf("  write:DATA           write DATA to I2C address ADDR\n");
		tout.Printf("  read:N               read N bytes from I2C address ADDR\n");
		return Result::Success;
	}
	int nArgsSkip = 0;
	int iBus = -1;
	if (genericFlag) {
		if (argc >= 2) {
			// nothing to do
		} else if (hasArgs) {
			terr.Printf("I2C bus number is required\n");
			return Result::Error;		
		} else {
			for (int iBus = 0; iBus < 2; iBus++) PrintConfig(tout, iBus, "GPIO%-2d");
			return Result::Success;
		}
		char* endptr;
		iBus = ::strtol(argv[1], &endptr, 0);
		if (*endptr != '\0') {
			terr.Printf("Specify I2C bus number (0 or 1)\n");
			return Result::Error;
		} else if (iBus < 0 || iBus > 1) {
			terr.Printf("Invalid I2C bus number: %s (must be 0 or 1)\n", argv[1]);
			return Result::Error;
		}
		nArgsSkip = 2;
	} else if (::strncmp(GetName(), "i2c", 3) == 0) {
		iBus = ::strtoul(GetName() + 3, nullptr, 0);
		if (iBus < 0 || iBus > 1) {
			terr.Printf("Invalid I2C bus number in command name: %s\n", GetName());
			return Result::Error;
		}
		nArgsSkip = 1;
	}
	Config& config = configTbl[iBus];
	argc -= nArgsSkip;
	argv += nArgsSkip;
	if (arg.GetBool("help-pin")) {
		for (uint pin = 0; pin < count_of(infoTbl); ++pin) {
			const Info& info = infoTbl[pin];
			if (info.iBus == iBus) {
				tout.Printf("GPIO%-2d I2C%d %s\n", pin, info.iBus, (info.func == Func::SDA)? "SDA" : "SCL");
			}
		}
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("freq", &value)) {
		if (!value) {
			terr.Printf("Frequency value is required\n");
			return Result::Error;
		}
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 1'000'000) {
			terr.Printf("Invalid frequency: %s\n", value);
			return Result::Error;
		}
		config.freq = static_cast<uint>(num);
	}
	if (arg.GetString("timeout", &value)) {
		if (!value) {
			terr.Printf("Timeout value is required\n");
			return Result::Error;
		}
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 10'000) {
			terr.Printf("Invalid timeout: %s\n", value);
			return Result::Error;
		}
		config.msecTimeout = static_cast<uint32_t>(num);
	}
	if (arg.GetString("pin", &value)) {
		if (!value) {
			terr.Printf("Pin specification is required (use 'SDA,SCL')\n");
			return Result::Error;
		}
		Arg::EachNum each(value);
		int nNums;
		if (!each.CheckValidity(&nNums) || nNums != 2) {
			terr.Printf("Invalid pin specification: %s (expected 2 I2C pins: SDA,SCL)\n", value);
			return Result::Error;
		}
		
		// Reset I2C pin assignments
		config.SDA = -1;
		config.SCL = -1;
		
		// Parse each pin and assign based on function
		int num;
		while (each.Next(&num)) {
			if (num < 0 || num > 27) {
				terr.Printf("Invalid GPIO number: %d\n", num);
				return Result::Error;
			}
			
			const Info& info = infoTbl[num];
			
			// Check if pin belongs to the specified I2C bus
			if (info.iBus != iBus) {
				terr.Printf("GPIO%d belongs to I2C%d, but I2C%d was specified\n", 
					num, info.iBus, iBus);
				return Result::Error;
			}
			
			// Assign pin based on function
			if (info.func == Func::SCL) {
				if (config.SCL != -1) {
					terr.Printf("SCL pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.SCL, num);
					return Result::Error;
				}
				config.SCL = static_cast<uint>(num);
			} else { // Func::SDA
				if (config.SDA != -1) {
					terr.Printf("SDA pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.SDA, num);
					return Result::Error;
				}
				config.SDA = static_cast<uint>(num);
			}
		}
	}
	if (arg.GetString("pullup", &value)) {
		if (!value) {
			terr.Printf("Pull-up option is required (use 'on' or 'off')\n");
			return Result::Error;
		} else if (::strcasecmp(value, "on") == 0 || ::strcasecmp(value, "yes") == 0 || ::strcasecmp(value, "true") == 0) {
			config.pullupFlag = true;
		} else if (::strcasecmp(value, "off") == 0 || ::strcasecmp(value, "no") == 0 || ::strcasecmp(value, "false") == 0) {
			config.pullupFlag = false;
		} else {
			terr.Printf("Invalid pull-up option: %s (expected 'on' or 'off')\n", value);
			return Result::Error;
		}
	}
	if (arg.GetBool("dumb")) return Result::Success;
	if (config.SDA != -1 && config.SCL != -1) {
		// nothing to do
	} else if (argc > 0) {
		terr.Printf("SCL and SDA pins must be configured for I2C bus %d\n", iBus);
		return Result::Error;
	} else {
		PrintConfig(tout, iBus, "GPIO%d");
		return Result::Success;
	}
	
	I2C& i2c = (iBus == 0)? I2C0 : I2C1;
	i2c.init(config.freq);
	::gpio_set_function(config.SDA, GPIO_FUNC_I2C);
	::gpio_set_function(config.SCL, GPIO_FUNC_I2C);
	if (config.pullupFlag) {
		::gpio_pull_up(config.SDA);
		::gpio_pull_up(config.SCL);
	} else {
		::gpio_disable_pulls(config.SDA);
		::gpio_disable_pulls(config.SCL);
	}
	int rtn = 0;
	bool verboseFlag = arg.GetBool("verbose");
	if (argc == 0) {
		PrintConfig(tout, iBus, "GPIO%d");
		return Result::Success;
	} else if (::strcasecmp(argv[0], "scan") == 0) {
		ScanBus(tout, terr, i2c, iBus, config);
	} else {
		char* endptr;
		int num = ::strtol(argv[0], &endptr, 0);
		if (*endptr != '\0') {
			terr.Printf("Specify I2C address\n");
			return Result::Error;
		} else if (num < 0 || num > 127) {
			terr.Printf("Invalid I2C address: %s\n", argv[0]);
			return Result::Error;
		}
		uint8_t addr = static_cast<uint8_t>(num);
		Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
		if (!each.Initialize()) {
			terr.Printf("%s\n", each.GetErrorMsg());
			return Result::Error;
		}
		while (const char* arg = each.Next()) {
			const char* value;
			if (Arg::GetAssigned(arg, "write", &value)) {
				if (verboseFlag) tout.Printf("write: %s\n", value);
				if (!WriteData(tout, terr, i2c, addr, value, false, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "write-c", &value)) {
				if (verboseFlag) tout.Printf("write-c: %s\n", value);
				if (!WriteData(tout, terr, i2c, addr, value, true, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read", &value)) {
				if (verboseFlag) tout.Printf("read: %s\n", value);
				if (!ReadData(tout, terr, i2c, addr, value, false, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read-c", &value)) {
				if (verboseFlag) tout.Printf("read-c: %s\n", value);
				if (!ReadData(tout, terr, i2c, addr, value, true, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "sleep", &value)) {
				if (verboseFlag) tout.Printf("sleep: %s\n", value);
				int msec = ::strtol(value, nullptr, 0);
				if (msec <= 0) {
					terr.Printf("Invalid sleep duration: %s\n", value);
					rtn = 1;
					break;
				}
				Tickable::Sleep(msec);
			} else {
				terr.Printf("Unknown command: %s\n", arg);
				rtn = 1;
				break;
			}
			if (Tickable::TickSub()) break;
		}
	}
	return rtn;
}

void ScanBus(Printable& tout, Printable& terr, I2C& i2c, int iBus, const Config& config)
{
	bool nostop = false;
	tout.Printf("Bus Scan on I2C%d\n", i2c.get_index());
	tout.Printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	int iCol = 0;
	for (uint8_t addr = 0; addr <= 0x7f; ++addr) {
		if (iCol == 0) tout.Printf("%02x ", addr);
		uint8_t rxdata;
		int bytesRead = i2c.read_blocking(addr, &rxdata, sizeof(rxdata), nostop);
		if (bytesRead <= 0) {
			tout.Printf("-- ");
		} else {
			tout.Printf("%02x ", addr);
		}
		iCol++;
		if (iCol == 16) {
			tout.Printf("\n");
			iCol = 0;
		}
	}
}

void AllocDataBuff()
{
	if (!dataBuff) dataBuff = new uint8_t[bytesDataBuff];
	if (!dataBuff) ::panic("Failed to allocate data buffer for I2C operations");
}

bool WriteData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop, uint32_t msecTimeout)
{
	if (!value) {
		terr.Printf("No data to write\n");
		return false;
	}
	AllocDataBuff();
	int bytesToWrite = 0;
	Shell::Arg::EachNum each(value);
	if (!each.CheckValidity(&bytesToWrite)) {
		terr.Printf("Invalid data format: %s\n", value);
		return false;
	} else if (bytesToWrite == 0) {
		terr.Printf("No data to write\n");
		return false;
	} else if (bytesToWrite > bytesDataBuff) {
		terr.Printf("Data size exceeds buffer limit (%zu bytes)\n", bytesDataBuff);
		return false;
	}
	int num;
	for (int i = 0; each.Next(&num); ++i) {
		if (num < 0 || num > 255) {
			terr.Printf("Invalid data value: %d\n", num);
			return false;
		}
		dataBuff[i] = static_cast<uint8_t>(num);
	}
	int bytesWritten = i2c.write_blocking_until(addr, dataBuff, bytesToWrite, nostop, ::make_timeout_time_ms(msecTimeout));
	if (bytesWritten < 0) {
		terr.Printf("Failed to write data to address 0x%02x\n", addr);
		return false;
	}
	return true;
}

bool ReadData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop, uint32_t msecTimeout)
{
	AllocDataBuff();
	int bytesToRead = bytesDataBuff;
	if (value) {
		int num = ::strtol(value, nullptr, 0);
		if (num < 1 || num > bytesDataBuff) {
			terr.Printf("Invalid number of bytes to read: %s\n", value);
			return false;
		}
		bytesToRead = static_cast<int>(num);
	}
	int bytesRead = i2c.read_blocking_until(addr, dataBuff, bytesToRead, nostop, ::make_timeout_time_ms(msecTimeout));
	if (bytesRead < 0) {
		terr.Printf("Failed to read data from address 0x%02x\n", addr);
		return false;
	}
	Printable::DumpT dump(tout);
	dump.Addr(bytesToRead > dump.GetBytesPerRow())(dataBuff, bytesRead);
	return true;
}

void PrintConfig(Printable& tout, int iBus, const char* formatGPIO)
{
	auto printPin = [&](const char* name, uint pin) {
		tout.Printf(" %s:", name);
		if (pin != -1) {
			tout.Printf(formatGPIO, pin);
		} else {
			tout.Printf("------");
		}
	};
	const Config& config = configTbl[iBus];
	tout.Printf("I2C%d:", iBus);
	printPin("SDA", config.SDA);
	printPin("SCL", config.SCL);
	tout.Printf(" freq:%dHz pullup:%s timeout:%dmsec\n", config.freq, config.pullupFlag ? "on" : "off", config.msecTimeout);
}

ShellCmdAlias_Named(i2c0_, "i2c0", i2c_)
ShellCmdAlias_Named(i2c1_, "i2c1", i2c_)

}
