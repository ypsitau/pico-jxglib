//==============================================================================
// ShellCmd_UART.cpp
//==============================================================================
#include "jxglib/UART.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_UART {

struct Config {
	uint TX = -1;
	uint RX = -1;
	uint baudrate = 115200;
	uint data_bits = 8;
	uint stop_bits = 1;
	uart_parity_t parity = UART_PARITY_NONE;
	bool flow_control = false;
	uint32_t msecTimeout = 1000;
};

static Config configTbl[2];  // UART0 and UART1 configurations

const int bytesDataBuff = 8192;
static uint8_t* dataBuff = nullptr;

static bool WriteData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout);
static bool ReadData(Printable& tout, Printable& terr, UART& uart, const char* value, uint32_t msecTimeout);
static void PrintConfig(Printable& tout, int iUART, const char* formatGPIO);

ShellCmd_Named(uart_, "uart", "controls UART communication")
{
	enum class Func { TX, RX };
	struct Info { int iUART; Func func; };
	static const Info infoTbl[] = {
		{ 0, Func::TX }, // GPIO0  UART0 TX
		{ 0, Func::RX }, // GPIO1  UART0 RX
		{ 1, Func::TX }, // GPIO2  UART1 TX
		{ 1, Func::RX }, // GPIO3  UART1 RX
		{ 0, Func::TX }, // GPIO4  UART0 TX
		{ 0, Func::RX }, // GPIO5  UART0 RX
		{ 1, Func::TX }, // GPIO6  UART1 TX
		{ 1, Func::RX }, // GPIO7  UART1 RX
		{ 0, Func::TX }, // GPIO8  UART0 TX
		{ 0, Func::RX }, // GPIO9  UART0 RX
		{ 1, Func::TX }, // GPIO10 UART1 TX
		{ 1, Func::RX }, // GPIO11 UART1 RX
		{ 0, Func::TX }, // GPIO12 UART0 TX
		{ 0, Func::RX }, // GPIO13 UART0 RX
		{ 1, Func::TX }, // GPIO14 UART1 TX
		{ 1, Func::RX }, // GPIO15 UART1 RX
		{ 0, Func::TX }, // GPIO16 UART0 TX
		{ 0, Func::RX }, // GPIO17 UART0 RX
		{ 1, Func::TX }, // GPIO18 UART1 TX
		{ 1, Func::RX }, // GPIO19 UART1 RX
		{ 0, Func::TX }, // GPIO20 UART0 TX
		{ 0, Func::RX }, // GPIO21 UART0 RX
		{ 1, Func::TX }, // GPIO22 UART1 TX
		{ 1, Func::RX }, // GPIO23 UART1 RX
		{ 0, Func::TX }, // GPIO24 UART0 TX
		{ 0, Func::RX }, // GPIO25 UART0 RX
		{ 1, Func::TX }, // GPIO26 UART1 TX
		{ 1, Func::RX }, // GPIO27 UART1 RX
		{ 0, Func::TX }, // GPIO28 UART0 TX
		{ 0, Func::RX }, // GPIO29 UART0 RX
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("help-pin",	0x0,	"prints help for pin assignment"),
		Arg::OptBool("dumb",		0x0,	"no UART operations, just remember the pins and settings"),
		Arg::OptBool("verbose",		'v',	"verbose output for debugging"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for UART", "TX,RX"),
		Arg::OptString("baudrate",	'b',	"sets UART baudrate (default: 115200)", "RATE"),
		Arg::OptString("data-bits",	'd',	"sets data bits (default: 8)", "BITS"),
		Arg::OptString("stop-bits",	's',	"sets stop bits (default: 1)", "BITS"),
		Arg::OptString("parity",	0x0,	"sets parity (default: none)", "none|even|odd"),
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
				tout.Printf("GPIO%-2d UART%d %s\n", pin, info.iUART, (info.func == Func::TX)? "TX" : "RX");
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
	if (arg.GetString("data-bits", &value)) {
		if (!value) {
			terr.Printf("Data bits value is required\n");
			return Result::Error;
		}
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num < 5 || num > 8) {
			terr.Printf("Invalid data bits: %s (must be 5-8)\n", value);
			return Result::Error;
		}
		config.data_bits = static_cast<uint>(num);
	}
	if (arg.GetString("stop-bits", &value)) {
		if (!value) {
			terr.Printf("Stop bits value is required\n");
			return Result::Error;
		}
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || (num != 1 && num != 2)) {
			terr.Printf("Invalid stop bits: %s (must be 1 or 2)\n", value);
			return Result::Error;
		}
		config.stop_bits = static_cast<uint>(num);
	}
	if (arg.GetString("parity", &value)) {
		if (!value) {
			terr.Printf("Parity value is required\n");
			return Result::Error;
		} else if (::strcasecmp(value, "none") == 0) {
			config.parity = UART_PARITY_NONE;
		} else if (::strcasecmp(value, "even") == 0) {
			config.parity = UART_PARITY_EVEN;
		} else if (::strcasecmp(value, "odd") == 0) {
			config.parity = UART_PARITY_ODD;
		} else {
			terr.Printf("Invalid parity: %s (expected 'none', 'even', or 'odd')\n", value);
			return Result::Error;
		}
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
		int nNums;
		if (!each.CheckValidity(&nNums) || nNums != 2) {
			terr.Printf("Invalid pin specification: %s (expected 2 UART pins: TX,RX)\n", value);
			return Result::Error;
		}
		
		// Reset UART pin assignments
		config.TX = -1;
		config.RX = -1;
		
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
				if (config.TX != -1) {
					terr.Printf("TX pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.TX, num);
					return Result::Error;
				}
				config.TX = static_cast<uint>(num);
			} else { // Func::RX
				if (config.RX != -1) {
					terr.Printf("RX pin already assigned (GPIO%d), cannot assign GPIO%d\n", config.RX, num);
					return Result::Error;
				}
				config.RX = static_cast<uint>(num);
			}
		}
	}
	if (arg.GetBool("dumb")) return Result::Success;
	if (config.TX != -1 && config.RX != -1) {
		// nothing to do
	} else if (argc > 0) {
		terr.Printf("TX and RX pins must be configured for UART %d\n", iUART);
		return Result::Error;
	} else {
		PrintConfig(tout, iUART, "GPIO%d");
		return Result::Success;
	}
	
	UART& uart = (iUART == 0)? UART0 : UART1;
	uart.raw.init(config.baudrate);
	uart.raw.set_format(config.data_bits, config.stop_bits, config.parity);
	uart.raw.set_hw_flow(config.flow_control, config.flow_control);
	uart.raw.set_fifo_enabled(true);
	::gpio_set_function(config.TX, GPIO_FUNC_UART);
	::gpio_set_function(config.RX, GPIO_FUNC_UART);
	
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
				if (verboseFlag) tout.Printf("write: %s\n", value);
				if (!WriteData(tout, terr, uart, value, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read", &value)) {
				if (verboseFlag) tout.Printf("read: %s\n", value? value : "all");
				if (!ReadData(tout, terr, uart, value, config.msecTimeout)) {
					rtn = 1;
					break;
				}
			} else if (::strcmp(arg, "flush") == 0) {
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
			::tight_loop_contents();
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
		if (pin != -1) {
			tout.Printf(formatGPIO, pin);
		} else {
			tout.Printf("------");
		}
	};
	const Config& config = configTbl[iUART];
	const char* parityStr = (config.parity == UART_PARITY_NONE)? "none" :
							(config.parity == UART_PARITY_EVEN)? "even" : "odd";
	tout.Printf("UART%d:", iUART);
	printPin("TX", config.TX);
	printPin("RX", config.RX);
	tout.Printf(" baud:%d %d%c%d flow:%s timeout:%dmsec\n", 
		config.baudrate, config.data_bits, parityStr[0], config.stop_bits,
		config.flow_control ? "on" : "off", config.msecTimeout);
}

}

using namespace jxglib::ShellCmd_UART;

ShellCmdAlias_Named(uart0_, "uart0", uart_)
ShellCmdAlias_Named(uart1_, "uart1", uart_)
