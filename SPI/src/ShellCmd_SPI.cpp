//==============================================================================
// ShellCmd_SPI.cpp
//==============================================================================
#include "hardware/clocks.h"
#include "jxglib/SPI.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_SPI {

static bool WriteData(Printable& tout, Printable& terr, SPI& spi, const char* value);
static bool ReadData(Printable& tout, Printable& terr, SPI& spi, const char* value, uint8_t byteDummy);
static bool TransferData(Printable& tout, Printable& terr, SPI& spi, const char* value);
static void PrintConfig(Printable& tout, int iBus, const char* formatGPIO);

ShellCmd_Named(spi_, "spi", "controls SPI bus communication")
{
	enum class Func { None, SCK, MOSI, MISO };
	struct Info { int iBus; Func func; };
	static const Info infoTbl[] = {
		{  0, Func::MISO},	// GPIO0  SPI0 RX
		{ -1, Func::None },	// GPIO1  (not SPI function)
		{  0, Func::SCK  },	// GPIO2  SPI0 SCK
		{  0, Func::MOSI },	// GPIO3  SPI0 TX
		{  0, Func::MISO },	// GPIO4  SPI0 RX
		{ -1, Func::None },	// GPIO5  (not SPI function)
		{  0, Func::SCK  },	// GPIO6  SPI0 SCK
		{  0, Func::MOSI },	// GPIO7  SPI0 TX
		{  1, Func::MISO },	// GPIO8  SPI1 RX
		{ -1, Func::None },	// GPIO9  (not SPI function)
		{  1, Func::SCK  },	// GPIO10 SPI1 SCK
		{  1, Func::MOSI },	// GPIO11 SPI1 TX
		{  1, Func::MISO },	// GPIO12 SPI1 RX
		{ -1, Func::None },	// GPIO13 (not SPI function)
		{  1, Func::SCK  },	// GPIO14 SPI1 SCK
		{  1, Func::MOSI },	// GPIO15 SPI1 TX
		{  0, Func::MISO },	// GPIO16 SPI0 RX
		{ -1, Func::None },	// GPIO17 (not SPI function)
		{  0, Func::SCK  },	// GPIO18 SPI0 SCK
		{  0, Func::MOSI },	// GPIO19 SPI0 TX
		{  0, Func::MISO },	// GPIO20 SPI0 RX
		{ -1, Func::None },	// GPIO21 (not SPI function)
		{  0, Func::SCK  },	// GPIO22 SPI0 SCK
		{  0, Func::MOSI },	// GPIO23 SPI0 TX
		{  1, Func::MISO },	// GPIO24 SPI1 RX
		{ -1, Func::None },	// GPIO25 (not SPI function)
		{  1, Func::SCK  },	// GPIO26 SPI1 SCK
		{  1, Func::MOSI },	// GPIO27 SPI1 TX
		{  1, Func::MISO },	// GPIO28 SPI1 RX
		{ -1, Func::None },	// GPIO29 (not SPI function)
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("help-pin",	0x0,	"prints help for pin assignment"),
		Arg::OptBool("dumb",		0x0,	"no SPI operations, just remember the pins and settings"),
		Arg::OptBool("verbose",		'v',	"verbose output for debugging"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for SPI (function auto-detected)", "SCK,MOSI[,MISO]"),
		Arg::OptString("pin-cs",	'c',	"sets CS pin (any GPIO)", "CS_PIN"),
		Arg::OptString("baudrate",	'B',	"sets SPI baudrate (default: 1000000)", "BPS"),
		Arg::OptString("mode",		'm',	"sets SPI mode (0-3, default: 0)", "MODE"),
		Arg::OptString("cpol",		0x0,	"sets clock polarity (0 or 1, default: 0)", "CPOL"),
		Arg::OptString("cpha",		0x0,	"sets clock phase (0 or 1, default: 0)", "CPHA"),
		Arg::OptString("order",		0x0,	"sets bit order (msb or lsb, default: msb)", "ORDER"),
		Arg::OptString("dummy",		0x0,	"sets dummy byte for read operations (default: 0x00)", "BYTE"),
	};
	bool hasArgs = (argc > 1);
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "spi") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s BUS [OPTION]... [COMMAND]...\n", GetName());
			tout.Printf("  BUS: SPI bus number (0 or 1)\n");
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf(" sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf(" write:DATA           write DATA to SPI\n");
		tout.Printf(" read:N               read N bytes from SPI (requires MISO)\n");
		tout.Printf(" transfer:DATA        transfer DATA (write and read simultaneously, requires MISO)\n");
		tout.Printf(" cs:VALUE             set CS pin value (0, 1, lo, hi)\n");
		return Result::Success;
	}
	int nArgsSkip = 0;
	int iBus = -1;
	if (genericFlag) {
		if (argc >= 2) {
			// nothing to do
		} else if (hasArgs) {
			terr.Printf("SPI bus number is required\n");
			return Result::Error;		
		} else {
			for (int iBus = 0; iBus < 2; iBus++) PrintConfig(tout, iBus, "GPIO%-2d");
			return Result::Success;
		}
		iBus = ::strtol(argv[1], nullptr, 0);
		if (iBus < 0 || iBus > 1) {
			terr.Printf("Invalid SPI bus number: %s (must be 0 or 1)\n", argv[1]);
			return Result::Error;
		}
		nArgsSkip = 2;
	} else if (::strncmp(GetName(), "spi", 3) == 0) {
		iBus = ::strtoul(GetName() + 3, nullptr, 0);
		if (iBus < 0 || iBus > 1) {
			terr.Printf("Invalid SPI bus number in command name: %s\n", GetName());
			return Result::Error;
		}
		nArgsSkip = 1;
	}
	SPI::Config& config = SPI::get_instance(iBus).config;
	argc -= nArgsSkip;
	argv += nArgsSkip;
	if (arg.GetBool("help-pin")) {
		for (uint pin = 0; pin < count_of(infoTbl); ++pin) {
			const Info& info = infoTbl[pin];
			if (info.iBus == iBus) {
				tout.Printf("GPIO%-2d SPI%d %s\n", pin, info.iBus,
					(info.func == Func::SCK)? "SCK" :
					(info.func == Func::MOSI)? "MOSI" :
					(info.func == Func::MISO)? "MISO" : "None");
			}
		}
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("baudrate", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > ::clock_get_hz(clk_sys) / 2) {
			terr.Printf("Invalid baudrate: %s\n", value);
			return Result::Error;
		}
		config.baudrate = static_cast<uint>(num);
	}
	if (arg.GetString("mode", &value)) {
		int mode = ::strtol(value, nullptr, 0);
		if (mode < 0 || mode > 3) {
			terr.Printf("Invalid SPI mode: %s (must be 0-3)\n", value);
			return Result::Error;
		}
		// Set CPOL and CPHA based on mode using bit operations
		// Mode 0: CPOL=0, CPHA=0
		// Mode 1: CPOL=0, CPHA=1
		// Mode 2: CPOL=1, CPHA=0
		// Mode 3: CPOL=1, CPHA=1
		config.cpol = (mode & 2) ? SPI_CPOL_1 : SPI_CPOL_0;
		config.cpha = (mode & 1) ? SPI_CPHA_1 : SPI_CPHA_0;
	}
	if (arg.GetString("cpol", &value)) {
		int pol = ::strtol(value, nullptr, 0);
		if (pol == 0) {
			config.cpol = SPI_CPOL_0;
		} else if (pol == 1) {
			config.cpol = SPI_CPOL_1;
		} else {
			terr.Printf("Invalid clock polarity: %s (must be 0 or 1)\n", value);
			return Result::Error;
		}
	}
	if (arg.GetString("cpha", &value)) {
		int pha = ::strtol(value, nullptr, 0);
		if (pha == 0) {
			config.cpha = SPI_CPHA_0;
		} else if (pha == 1) {
			config.cpha = SPI_CPHA_1;
		} else {
			terr.Printf("Invalid clock phase: %s (must be 0 or 1)\n", value);
			return Result::Error;
		}
	}
	if (arg.GetString("order", &value)) {
		if (::strcasecmp(value, "msb") == 0) {
			config.order = SPI_MSB_FIRST;
		} else if (::strcasecmp(value, "lsb") == 0) {
			config.order = SPI_LSB_FIRST;
		} else {
			terr.Printf("Invalid bit order: %s (must be msb or lsb)\n", value);
			return Result::Error;
		}
	}
	if (arg.GetString("dummy", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num < 0 || num > 255) {
			terr.Printf("Invalid dummy byte: %s (must be 0-255)\n", value);
			return Result::Error;
		}
		config.byteDummy = static_cast<uint8_t>(num);
	}
	if (arg.GetString("pin", &value)) {
		Arg::EachNum each(value);
		int nNums;
		if (!each.CheckValidity(&nNums) || nNums < 2 || nNums > 3) {
			terr.Printf("Invalid pin specification: %s (expected 2-3 SPI pins: SCK,MOSI[,MISO])\n", value);
			return Result::Error;
		}
		
		// Reset SPI pin assignments (not CS)
		config.SCK = -1;
		config.MOSI = -1;
		config.MISO = -1;
		
		// Parse each pin and assign based on function
		int num;
		while (each.Next(&num)) {
			if (num < 0 || num > 27) {
				terr.Printf("Invalid GPIO number: %d\n", num);
				return Result::Error;
			}
			
			const Info& info = infoTbl[num];
			
			// Check if pin has a valid SPI function
			if (info.func == Func::None) {
				terr.Printf("GPIO%d does not have SPI function\n", num);
				return Result::Error;
			}
			
			// Check if pin belongs to the specified SPI bus
			if (info.iBus != iBus) {
				terr.Printf("GPIO%d belongs to SPI%d, but SPI%d was specified\n", 
					num, info.iBus, iBus);
				return Result::Error;
			}
			
			// Assign pin based on function
			switch (info.func) {
			case Func::SCK:
				if (config.SCK != -1) {
					terr.Printf("SCK pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.SCK, num);
					return Result::Error;
				}
				config.SCK = num;
				break;
			case Func::MOSI:
				if (config.MOSI != -1) {
					terr.Printf("MOSI pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.MOSI, num);
					return Result::Error;
				}
				config.MOSI = num;
				break;
			case Func::MISO:
				if (config.MISO != -1) {
					terr.Printf("MISO pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.MISO, num);
					return Result::Error;
				}
				config.MISO = num;
				break;
			}
		}
	}
	if (arg.GetString("pin-cs", &value)) {
		int csPin = ::strtol(value, nullptr, 0);
		if (csPin < 0 || csPin > 27) {
			terr.Printf("Invalid CS pin: %s (must be 0-27)\n", value);
			return Result::Error;
		}
		config.CS = csPin;
	}
	if (arg.GetBool("dumb")) return Result::Success;
	if (config.SCK != -1 && config.MOSI != -1) {
		// nothing to do
	} else if (argc > 0) {
		terr.Printf("SCK and MOSI pins must be configured for SPI bus %d\n", iBus);
		return Result::Error;
	} else {
		PrintConfig(tout, iBus, "GPIO%d");
		return Result::Success;
	}
	
	SPI& spi = (iBus == 0) ? SPI0 : SPI1;
	
	// Initialize SPI
	spi.init(config.baudrate);
	spi.set_format(8, config.cpol, config.cpha, config.order);
	
	// Configure GPIO pins
	::gpio_set_function(config.SCK, GPIO_FUNC_SPI);
	::gpio_set_function(config.MOSI, GPIO_FUNC_SPI);
	if (config.MISO != -1) {
		::gpio_set_function(config.MISO, GPIO_FUNC_SPI);
	}
	
	if (config.CS != -1) {
		::gpio_init(config.CS);
		::gpio_set_dir(config.CS, GPIO_OUT);
		::gpio_put(config.CS, true); // CS hi (inactive)
	}
	if (argc == 0) {
		PrintConfig(tout, iBus, "GPIO%d");
		return Result::Success;
	}
	int rtn = 0;
	Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	bool verboseFlag = arg.GetBool("verbose");
	while (const char* arg = each.Next()) {
		const char* value;
		if (Arg::GetAssigned(arg, "write", &value)) {
			if (verboseFlag) tout.Printf("write: %s\n", value);
			if (!WriteData(tout, terr, spi, value)) {
				rtn = 1;
				break;
			}
		} else if (Arg::GetAssigned(arg, "read", &value)) {
			if (config.MISO == -1) {
				terr.Printf("MISO pin not configured, cannot read\n");
				rtn = 1;
				break;
			}
			if (verboseFlag) tout.Printf("read: %s\n", value);
			if (!ReadData(tout, terr, spi, value, config.byteDummy)) {
				rtn = 1;
				break;
			}
		} else if (Arg::GetAssigned(arg, "transfer", &value)) {
			if (config.MISO == -1) {
				terr.Printf("MISO pin not configured, cannot transfer\n");
				rtn = 1;
				break;
			}
			if (verboseFlag) tout.Printf("transfer: %s\n", value);
			if (!TransferData(tout, terr, spi, value)) {
				rtn = 1;
				break;
			}
		} else if (Arg::GetAssigned(arg, "cs", &value)) {
			if (config.CS == -1) {
				terr.Printf("CS pin not configured\n");
				rtn = 1;
				break;
			}
			bool flag = false;
			if (::strcasecmp(value, "hi") == 0 || ::strcasecmp(value, "high") == 0 ||
				::strcasecmp(value, "true") == 0 || ::strcmp(value, "1") == 0) {
				flag = true;
			} else if (::strcasecmp(value, "lo") == 0 || ::strcasecmp(value, "low") == 0 ||
						::strcasecmp(value, "false") == 0 || ::strcmp(value, "0") == 0) {
				flag = false;
			} else {
				terr.Printf("unknown value: %s\n", value);
				return false;
			}
			if (verboseFlag) tout.Printf("cs: %s\n", flag? "hi" : "lo");
			::gpio_put(config.CS, flag);
		} else if (Arg::GetAssigned(arg, "sleep", &value)) {
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				rtn = 1;
				break;
			}
			if (verboseFlag) tout.Printf("sleep: %s\n", value);
			Tickable::Sleep(msec);
		} else {
			terr.Printf("Unknown command: %s\n", arg);
			rtn = 1;
			break;
		}
		if (Tickable::TickSub()) break;
	}
	return rtn;
}

bool WriteData(Printable& tout, Printable& terr, SPI& spi, const char* value)
{
	if (!value) {
		terr.Printf("No data to write\n");
		return false;
	}
	uint8_t data[512];
	int bytesToWrite = 0;
	Shell::Arg::EachNum each(value);
	if (!each.CheckValidity(&bytesToWrite)) {
		terr.Printf("Invalid data format: %s\n", value);
		return false;
	} else if (bytesToWrite == 0) {
		terr.Printf("No data to write\n");
		return false;
	} else if (bytesToWrite > sizeof(data)) {
		terr.Printf("Data size exceeds buffer limit (%zu bytes)\n", sizeof(data));
		return false;
	}
	int num;
	for (int i = 0; each.Next(&num); ++i) {
		if (num < 0 || num > 255) {
			terr.Printf("Invalid data value: %d\n", num);
			return false;
		}
		data[i] = static_cast<uint8_t>(num);
	}
	int bytesWritten = spi.write_blocking(data, bytesToWrite);
	if (bytesWritten != bytesToWrite) {
		terr.Printf("Failed to write all data (%d/%d bytes written)\n", bytesWritten, bytesToWrite);
		return false;
	}
	return true;
}

bool ReadData(Printable& tout, Printable& terr, SPI& spi, const char* value, uint8_t byteDummy)
{
	uint8_t data[512];
	int bytesToRead = sizeof(data);
	if (value) {
		int num = ::strtol(value, nullptr, 0);
		if (num < 1 || num > sizeof(data)) {
			terr.Printf("Invalid number of bytes to read: %s\n", value);
			return false;
		}
		bytesToRead = static_cast<int>(num);
	} else {
		terr.Printf("Number of bytes to read must be specified\n");
		return false;
	}
	
	// For read operation, we need to send dummy bytes
	int bytesRead = spi.read_blocking(byteDummy, data, bytesToRead);
	if (bytesRead != bytesToRead) {
		terr.Printf("Failed to read all data (%d/%d bytes read)\n", bytesRead, bytesToRead);
		return false;
	}
	Printable::DumpT dump(tout);
	dump.Addr(bytesToRead > dump.GetBytesPerRow())(data, bytesRead);
	return true;
}

bool TransferData(Printable& tout, Printable& terr, SPI& spi, const char* value)
{
	if (!value) {
		terr.Printf("No data to transfer\n");
		return false;
	}
	uint8_t txData[512];
	uint8_t rxData[512];
	int bytesToTransfer = 0;
	Shell::Arg::EachNum each(value);
	if (!each.CheckValidity(&bytesToTransfer)) {
		terr.Printf("Invalid data format: %s\n", value);
		return false;
	} else if (bytesToTransfer == 0) {
		terr.Printf("No data to transfer\n");
		return false;
	} else if (bytesToTransfer > sizeof(txData)) {
		terr.Printf("Data size exceeds buffer limit (%zu bytes)\n", sizeof(txData));
		return false;
	}
	int num;
	for (int i = 0; each.Next(&num); ++i) {
		if (num < 0 || num > 255) {
			terr.Printf("Invalid data value: %d\n", num);
			return false;
		}
		txData[i] = static_cast<uint8_t>(num);
	}
	int bytesTransferred = spi.write_read_blocking(txData, rxData, bytesToTransfer);
	if (bytesTransferred != bytesToTransfer) {
		terr.Printf("Failed to transfer all data (%d/%d bytes transferred)\n", bytesTransferred, bytesToTransfer);
		return false;
	}
	Printable::DumpT dump(tout);
	dump.Addr(bytesToTransfer > dump.GetBytesPerRow())(rxData, bytesTransferred);
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
	const SPI::Config& config = SPI::get_instance(iBus).config;
	int mode = (config.cpol << 1) | config.cpha;
	tout.Printf("SPI%d:", iBus);
	printPin("SCK", config.SCK);
	printPin("MOSI", config.MOSI);
	printPin("MISO", config.MISO);
	printPin("CS", config.CS);
	tout.Printf(" baudrate:%dbps mode:%d (cpol:%d cpha:%d) order:%s dummy:0x%02x\n",
		config.baudrate, mode, config.cpol, config.cpha, (config.order == SPI_MSB_FIRST)? "MSB" : "LSB", config.byteDummy);
}

ShellCmdAlias_Named(spi0_, "spi0", spi_)
ShellCmdAlias_Named(spi1_, "spi1", spi_)

}
