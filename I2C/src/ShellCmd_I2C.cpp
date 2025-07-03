//==============================================================================
// ShellCmd_I2C.cpp
//==============================================================================
#include "jxglib/I2C.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

struct {
	uint SDA = -1;
	uint SCL = -1;
} pinAssign;

uint freq = 100'000;
uint32_t msecTimeout = 300;

void ScanBus(Printable& tout, Printable& terr, I2C& i2c);
bool WriteData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop);
bool ReadData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop);

ShellCmd(i2c, "scans I2C bus and prints connected addresses")
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
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for I2C", "SDA,SCL"),
		Arg::OptString("freq",		'f',	"sets I2C frequency (default: 100000)", "FREQ"),
		Arg::OptString("timeout",	't',	"sets timeout for I2C operations (default: 300)", "MSEC"),
		Arg::OptBool("dumb",		0x0,	"no I2C operations, just remember the pins and frequency"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", argv[0]);
		arg.PrintHelp(tout);
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", argv[0]);
		tout.Printf("Commands:\n");
		tout.Printf("  read[:N]     read N bytes of data\n");
		tout.Printf("  write:data   write data\n");
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("freq", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 1'000'000) {
			tout.Printf("Invalid frequency: %s\n", value);
			return Result::Error;
		}
		freq = static_cast<uint>(num);
	}
	if (arg.GetString("timeout", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 10'000) {
			tout.Printf("Invalid timeout: %s\n", value);
			return Result::Error;
		}
		msecTimeout = static_cast<uint32_t>(num);
	}
	if (arg.GetString("pin", &value)) {
		Arg::EachNum each(value);
		int nNums;
		if (!each.CheckValidity(&nNums) || nNums != 2) {
			tout.Printf("Invalid pin specification: %s\n", value);
			return Result::Error;
		}
		int num;
		while (each.Next(&num)) {
			if (num < 0 || num > 27) {
				tout.Printf("Invalid GPIO number\n");
				return Result::Error;
			}
			if (infoTbl[num].func == Func::SCL) {
				pinAssign.SCL = static_cast<uint>(num);
			} else { // Func::SDA
				pinAssign.SDA = static_cast<uint>(num);
			}
		}
	}
	if (pinAssign.SDA == -1 || pinAssign.SCL == -1) {
		tout.Printf("use -p option to specify a valid pair of GPIOs for I2C\n");
		return Result::Error;
	} else if (infoTbl[pinAssign.SDA].iBus != infoTbl[pinAssign.SCL].iBus) {
		tout.Printf("Invalid pair of GPIOs for I2C\n");
		return Result::Error;
	}
	if (arg.GetBool("dumb")) return Result::Success;
	int iBus = infoTbl[pinAssign.SDA].iBus;
	I2C& i2c = (iBus == 0)? I2C0 : I2C1;
	i2c.init(freq);
	::gpio_set_function(pinAssign.SDA, GPIO_FUNC_I2C);
	::gpio_set_function(pinAssign.SCL, GPIO_FUNC_I2C);
	::gpio_pull_up(pinAssign.SDA);
	::gpio_pull_up(pinAssign.SCL);
	int rtn = 0;
	if (argc < 2) {
		ScanBus(tout, terr, i2c);
	} else {
		int num = ::strtol(argv[1], nullptr, 0);
		if (num < 0 || num > 127) {
			tout.Printf("Invalid I2C address: %s\n", argv[1]);
			return Result::Error;
		}
		uint8_t addr = static_cast<uint8_t>(num);
		Shell::Arg::EachSubcmd each(argv[2], argv[argc]);
		if (!each.Initialize()) {
			terr.Printf("%s\n", each.GetErrorMsg());
			return Result::Error;
		}
		while (const char* arg = each.Next()) {
			const char* value;
			if (Arg::GetAssigned(arg, "write", &value)) {
				if (!WriteData(tout, terr, i2c, addr, value, false)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "write-nostop", &value)) {
				if (!WriteData(tout, terr, i2c, addr, value, true)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read", &value)) {
				if (!ReadData(tout, terr, i2c, addr, value, false)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read-nostop", &value)) {
				if (!ReadData(tout, terr, i2c, addr, value, true)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "sleep", &value)) {
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

void ScanBus(Printable& tout, Printable& terr, I2C& i2c)
{
	tout.Printf("Bus Scan on I2C %d (SDA:GPIO%d SCL:GPIO%d) @ %d Hz\n", i2c.get_index(), pinAssign.SDA, pinAssign.SCL, freq);
	tout.Printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	int iCol = 0;
	for (int addr = 0; addr < (1 << 7); ++addr) {
		if (iCol == 0) tout.Printf("%02x ", addr);
		if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78) {
			tout.Printf("-- ");
		} else {
			uint8_t rxdata;
			int rtn = i2c.read_blocking(addr, &rxdata, sizeof(rxdata), false);
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
}

bool WriteData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop)
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
	int bytesWritten = i2c.write_blocking_until(addr, data, bytesToWrite, nostop, ::make_timeout_time_ms(msecTimeout));
	if (bytesWritten < 0) {
		terr.Printf("Failed to write data to address 0x%02x\n", addr);
		return false;
	}
	return true;
}

bool ReadData(Printable& tout, Printable& terr, I2C& i2c, uint8_t addr, const char* value, bool nostop)
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
	}
	int bytesRead = i2c.read_blocking_until(addr, data, bytesToRead, nostop, ::make_timeout_time_ms(msecTimeout));
	if (bytesRead < 0) {
		terr.Printf("Failed to receive data from address 0x%02x\n", addr);
		return false;
	}
	Printable::DumpT dump(tout);
	dump.Addr(bytesToRead > dump.GetBytesPerRow())(data, bytesRead);
	return true;
}
