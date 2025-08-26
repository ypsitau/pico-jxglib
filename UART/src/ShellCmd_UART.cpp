//==============================================================================
// ShellCmd_UART.cpp
//==============================================================================
#include "jxglib/UART.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include <cctype>

namespace jxglib::ShellCmd_UART {

struct Config {
	uint pinTX = GPIO::InvalidPin;
	uint pinRX = GPIO::InvalidPin;
	uint baudrate = 115200;
	uint dataBits = 8;
	uint stopBits = 1;
	uart_parity_t parity = UART_PARITY_NONE;
	bool flow_control = false;
	uint32_t msecTimeout = 1000;
};

static Config configTbl[2];  // UART0 and UART1 configurations

static const int bytesDataBuff = 8192;
static uint8_t* dataBuff = nullptr;

static bool WriteData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout);
static bool ReadData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout);
static void AllocDataBuff();
static void PrintConfig(Printable& tout, int iUART, const char* formatGPIO);

ShellCmd_Named(uart_, "uart", "controls UART communication")
{
	enum class Func { TX, RX, CTS, RTS };
	struct Info { int iUART; Func func; };
	static const Info infoTbl[] = {
		{ 0, Func::TX },   // GPIO0  UART0 TX
		{ 0, Func::RX },   // GPIO1  UART0 RX
		{ 0, Func::CTS },  // GPIO2  UART0 CTS
		{ 0, Func::RTS },  // GPIO3  UART0 RTS
		{ 1, Func::TX },   // GPIO4  UART1 TX
		{ 1, Func::RX },   // GPIO5  UART1 RX
		{ 1, Func::CTS },  // GPIO6  UART1 CTS
		{ 1, Func::RTS },  // GPIO7  UART1 RTS
		{ 1, Func::TX },   // GPIO8  UART1 TX
		{ 1, Func::RX },   // GPIO9  UART1 RX
		{ 1, Func::CTS },  // GPIO10 UART1 CTS
		{ 1, Func::RTS },  // GPIO11 UART1 RTS
		{ 0, Func::TX },   // GPIO12 UART0 TX
		{ 0, Func::RX },   // GPIO13 UART0 RX
		{ 0, Func::CTS },  // GPIO14 UART0 CTS
		{ 0, Func::RTS },  // GPIO15 UART0 RTS
		{ 0, Func::TX },   // GPIO16 UART0 TX
		{ 0, Func::RX },   // GPIO17 UART0 RX
		{ 0, Func::CTS },  // GPIO18 UART0 CTS
		{ 0, Func::RTS },  // GPIO19 UART0 RTS
		{ 1, Func::TX },   // GPIO20 UART1 TX
		{ 1, Func::RX },   // GPIO21 UART1 RX
		{ 1, Func::CTS },  // GPIO22 UART1 CTS
		{ 1, Func::RTS },  // GPIO23 UART1 RTS
		{ 1, Func::TX },   // GPIO24 UART1 TX
		{ 1, Func::RX },   // GPIO25 UART1 RX
		{ 1, Func::CTS },  // GPIO26 UART1 CTS
		{ 1, Func::RTS },  // GPIO27 UART1 RTS
		{ 0, Func::TX },   // GPIO28 UART0 TX
		{ 0, Func::RX },   // GPIO29 UART0 RX
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("help-pin",	0x0,	"prints help for pin assignment"),
		Arg::OptBool("dumb",		0x0,	"no UART operations, just remember the pins and settings"),
		Arg::OptBool("verbose",		'v',	"verbose output for debugging"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for UART", "TX,RX"),
		Arg::OptString("baudrate",    'b',	"sets UART baudrate (default: 115200)", "RATE"),
		Arg::OptString("frame",		'f',	"sets frame format in DPS where D (5-8), P (n,e,o) and S (1,2) (default: 8n1)", "FRAME"),
		Arg::OptBool("flow-control", 0x0,	"enables hardware flow control"),
		Arg::OptString("timeout",	't',	"sets timeout for UART operations (default: 1000)", "MSEC"),
	};
	bool hasArgs = (argc > 1);
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "uart") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s BUS [OPTION]... [COMMAND]...\n", GetName());
			tout.Printf("  BUS: UART number (0 or 1)\n");
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Commands:\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  write:DATA           write DATA to UART\n");
		tout.Printf("  read[:N]             read N bytes from UART (default: until timeout)\n");
		tout.Printf("  flush                flush UART buffers\n");
		return Result::Success;
	}
	int nArgsSkip = 0;
	int iUART = -1;
	if (genericFlag) {
		if (argc >= 2) {
			// nothing to do
		} else if (hasArgs) {
			terr.Printf("UART number is required\n");
			return Result::Error;		
		} else {
			for (int iUART = 0; iUART < 2; iUART++) PrintConfig(tout, iUART, "GPIO%-2d");
			return Result::Success;
		}
		char* endptr;
		iUART = ::strtol(argv[1], &endptr, 0);
		if (*endptr != '\0') {
			terr.Printf("Specify UART number (0 or 1)\n");
			return Result::Error;
		} else if (iUART < 0 || iUART > 1) {
			terr.Printf("Invalid UART number: %s (must be 0 or 1)\n", argv[1]);
			return Result::Error;
		}
		nArgsSkip = 2;
	} else if (::strncmp(GetName(), "uart", 4) == 0) {
		iUART = ::strtoul(GetName() + 4, nullptr, 0);
		if (iUART < 0 || iUART > 1) {
			terr.Printf("Invalid UART number in command name: %s\n", GetName());
			return Result::Error;
		}
		nArgsSkip = 1;
	}
	Config& config = configTbl[iUART];
	argc -= nArgsSkip;
	argv += nArgsSkip;
	if (arg.GetBool("help-pin")) {
		for (uint pin = 0; pin < count_of(infoTbl); ++pin) {
			const Info& info = infoTbl[pin];
			if (info.iUART == iUART) {
				const char* funcName = (info.func == Func::TX) ? "TX" :
						(info.func == Func::RX) ? "RX" : (info.func == Func::CTS) ? "CTS" : "RTS";
				tout.Printf("GPIO%-2d UART%d %s\n", pin, info.iUART, funcName);
			}
		}
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("baudrate", &value)) {
		if (!value) {
			terr.Printf("Baudrate value is required\n");
			return Result::Error;
		}
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 3'000'000) {
			terr.Printf("Invalid baudrate: %s\n", value);
			return Result::Error;
		}
		config.baudrate = static_cast<uint>(num);
	}
	if (arg.GetString("frame", &value)) {
		if (!value) {
			terr.Printf("Frame format is required (e.g., 8n1, 7e1, 8o2)\n");
			return Result::Error;
		}
		if (::strlen(value) != 3) {
			terr.Printf("invalid frame format\n");
			return false;
		}
		char ch = value[0];
		if ('5' <= ch && ch <= '9') {
			config.dataBits = ch - '0';
		} else {
			terr.Printf("invalid data bits in frame\n");
			return false;
		}
		ch = ::tolower(value[1]);
		if (ch == 'n') {
			config.parity = UART_PARITY_NONE;
		} else if (ch == 'e') {
			config.parity = UART_PARITY_EVEN;
		} else if (ch == 'o') {
			config.parity = UART_PARITY_ODD;
		} else {
			terr.Printf("invalid parity in frame\n");
			return false;
		}
		ch = value[2];
		if ('1' <= ch && ch <= '2') {
			config.stopBits = ch - '0';
		} else {
			terr.Printf("invalid stop bits in frame\n");
			return false;
		}
#if 0
		if (::strlen(value) != 3) {
			terr.Printf("Invalid frame format: %s (expected format: DBSParity, e.g., 8n1)\n", value);
			return Result::Error;
		}
		// Parse data bits (first character)
		int dataBits = value[0] - '0';
		if (dataBits < 5 || dataBits > 8) {
			terr.Printf("Invalid data bits: %c (must be 5-8)\n", value[0]);
			return Result::Error;
		}
		config.dataBits = static_cast<uint>(dataBits);
		// Parse parity (second character)
		char parity_char = ::tolower(value[1]);
		switch (parity_char) {
			case 'n':
				config.parity = UART_PARITY_NONE;
				break;
			case 'e':
				config.parity = UART_PARITY_EVEN;
				break;
			case 'o':
				config.parity = UART_PARITY_ODD;
				break;
			default:
				terr.Printf("Invalid parity: %c (must be n, e, or o)\n", value[1]);
				return Result::Error;
		}
		
		// Parse stop bits (third character)
		int stopBits = value[2] - '0';
		if (stopBits != 1 && stopBits != 2) {
			terr.Printf("Invalid stop bits: %c (must be 1 or 2)\n", value[2]);
			return Result::Error;
		}
		config.stopBits = static_cast<uint>(stopBits);
#endif
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
	if (arg.GetBool("flow-control")) {
		config.flow_control = true;
	}
	if (arg.GetString("pin", &value)) {
		if (!value) {
			terr.Printf("Pin specification is required (use 'TX,RX')\n");
			return Result::Error;
		}
		Arg::EachNum each(value);
		if (!each.CheckValidity()) {
			terr.Printf("Invalid pin specification: %s\n", value);
			return Result::Error;
		}
		// Reset UART pin assignments
		config.pinTX = GPIO::InvalidPin;
		config.pinRX = GPIO::InvalidPin;
		// Parse each pin and assign based on function
		int num;
		while (each.Next(&num)) {
			if (num < 0 || num >= count_of(infoTbl)) {
				terr.Printf("Invalid GPIO number: %d\n", num);
				return Result::Error;
			}
			const Info& info = infoTbl[num];
			// Check if pin belongs to the specified UART
			if (info.iUART != iUART) {
				terr.Printf("GPIO%d belongs to UART%d, but UART%d was specified\n", 
					num, info.iUART, iUART);
				return Result::Error;
			}
			// Assign pin based on function
			if (info.func == Func::TX) {
				if (config.pinTX != GPIO::InvalidPin) {
					terr.Printf("TX pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.pinTX, num);
					return Result::Error;
				}
				config.pinTX = static_cast<uint>(num);
			} else if (info.func == Func::RX) {
				if (config.pinRX != GPIO::InvalidPin) {
					terr.Printf("RX pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.pinRX, num);
					return Result::Error;
				}
				config.pinRX = static_cast<uint>(num);
			} else {
				terr.Printf("GPIO%d is a %s pin, only TX and RX pins are supported for basic UART operation\n", 
					num, (info.func == Func::CTS) ? "CTS" : "RTS");
				return Result::Error;
			}
		}
	}
	if (arg.GetBool("dumb")) return Result::Success;
	if (config.pinTX != GPIO::InvalidPin || config.pinRX != GPIO::InvalidPin) {
		// nothing to do
	} else if (argc > 0) {
		terr.Printf("TX or RX pins must be configured for UART %d\n", iUART);
		return Result::Error;
	} else {
		PrintConfig(tout, iUART, "GPIO%d");
		return Result::Success;
	}
	UART& uart = (iUART == 0)? UART0 : UART1;
	uart.raw.init(config.baudrate);
	uart.raw.set_format(config.dataBits, config.stopBits, config.parity);
	uart.raw.set_hw_flow(config.flow_control, config.flow_control);
	uart.raw.set_fifo_enabled(true);
	if (config.pinTX != GPIO::InvalidPin) ::gpio_set_function(config.pinTX, GPIO_FUNC_UART);
	if (config.pinRX != GPIO::InvalidPin) {
		::gpio_set_function(config.pinRX, GPIO_FUNC_UART);
		while (uart.raw.is_readable()) uart.raw.getc();	// Clear any existing data in the RX buffer
	}
	int rtn = 0;
	bool verboseFlag = arg.GetBool("verbose");
	if (argc == 0) {
		PrintConfig(tout, iUART, "GPIO%d");
		return Result::Success;
	} else {
		Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
		if (!each.Initialize()) {
			terr.Printf("%s\n", each.GetErrorMsg());
			return Result::Error;
		}
		while (const char* arg = each.Next()) {
			const char* value;
			if (Arg::GetAssigned(arg, "write", &value)) {
				if (config.pinTX == GPIO::InvalidPin) {
					terr.Printf("TX pin is not configured for UART%d\n", iUART);
					rtn = 1;
					break;
				}
				if (verboseFlag) tout.Printf("write: %s\n", value);
				if (!WriteData(tout, terr, uart, value, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read", &value)) {
				if (config.pinRX == GPIO::InvalidPin) {
					terr.Printf("RX pin is not configured for UART%d\n", iUART);
					rtn = 1;
					break;
				}
				if (verboseFlag) tout.Printf("read: %s\n", value? value : "all");
				if (!ReadData(tout, terr, uart, value, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (::strcmp(arg, "flush") == 0) {
				if (config.pinTX == GPIO::InvalidPin) {
					terr.Printf("TX pin is not configured for UART%d\n", iUART);
					rtn = 1;
					break;
				}
				if (verboseFlag) tout.Printf("flush\n");
				uart.raw.tx_wait_blocking();
				uart.Flush();
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

void AllocDataBuff()
{
	if (!dataBuff) dataBuff = new uint8_t[bytesDataBuff];
	if (!dataBuff) ::panic("Failed to allocate data buffer for UART operations");
}

bool WriteData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout)
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
		terr.Printf("Data size exceeds buffer limit (%d bytes)\n", bytesDataBuff);
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
	int bytesWritten = uart.Write(dataBuff, bytesToWrite);
	if (bytesWritten != bytesToWrite) {
		terr.Printf("Failed to write all data (wrote %d of %d bytes)\n", bytesWritten, bytesToWrite);
		return false;
	}
	return true;
}

bool ReadData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout)
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
	absolute_time_t timeout = ::make_timeout_time_ms(msecTimeout);
	int bytesRead = 0;
	while (bytesRead < bytesToRead && !::time_reached(timeout)) {
		if (uart.raw.is_readable()) {
			dataBuff[bytesRead] = static_cast<uint8_t>(uart.raw.getc());
			bytesRead++;
		} else {
			Tickable::TickSub();
		}
	}
	if (bytesRead == 0) {
		terr.Printf("No data received within timeout\n");
		return false;
	}
	Printable::DumpT dump(tout);
	dump.Addr(bytesRead > dump.GetBytesPerRow())(dataBuff, bytesRead);
	return true;
}

void PrintConfig(Printable& tout, int iUART, const char* formatGPIO)
{
	auto printPin = [&](const char* name, uint pin) {
		tout.Printf(" %s:", name);
		if (pin == GPIO::InvalidPin) {
			tout.Printf("------");
		} else {
			tout.Printf(formatGPIO, pin);
		}
	};
	const Config& config = configTbl[iUART];
	char parityChar = (config.parity == UART_PARITY_NONE)? 'n' :
                    (config.parity == UART_PARITY_EVEN)? 'e' : 'o';
	tout.Printf("UART%d:", iUART);
	printPin("TX", config.pinTX);
	printPin("RX", config.pinRX);
	tout.Printf(" baudrate:%d frame:%d%c%d flow-control:%s timeout:%dmsec\n", 
		config.baudrate, config.dataBits, parityChar, config.stopBits,
		config.flow_control ? "on" : "off", config.msecTimeout);
}

ShellCmdAlias_Named(uart0_, "uart0", uart_)
ShellCmdAlias_Named(uart1_, "uart1", uart_)

}
