//==============================================================================
// ShellCmd_SPI.cpp
//==============================================================================
#include "jxglib/SPI.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_SPI {

struct {
	uint SCK = -1;
	uint MOSI = -1;
	uint MISO = -1;
	uint CS = -1;
} pinAssign;

uint freq = 1'000'000;
uint32_t msecTimeout = 300;
spi_cpol_t cpol = SPI_CPOL_0;
spi_cpha_t cpha = SPI_CPHA_0;
spi_order_t order = SPI_MSB_FIRST;

bool WriteData(Printable& tout, Printable& terr, SPI& spi, const char* value);
bool ReadData(Printable& tout, Printable& terr, SPI& spi, const char* value);
bool TransferData(Printable& tout, Printable& terr, SPI& spi, const char* value);

ShellCmd(spi, "controls SPI bus communication")
{
	enum class Func { None, SCK, MOSI, MISO };
	struct Info { int iBus; Func func; };
	static const Info infoTbl[] = {
		{ 0, Func::MISO}, // GPIO0  SPI0 RX
		{ -1, Func::None }, // GPIO1  (not SPI function)
		{ 0, Func::SCK  }, // GPIO2  SPI0 SCK
		{ 0, Func::MOSI }, // GPIO3  SPI0 TX
		{ 0, Func::MISO }, // GPIO4  SPI0 RX
		{ -1, Func::None }, // GPIO5  (not SPI function)
		{ 0, Func::SCK  }, // GPIO6  SPI0 SCK
		{ 0, Func::MOSI }, // GPIO7  SPI0 TX
		{ 1, Func::MISO }, // GPIO8  SPI1 RX
		{ -1, Func::None }, // GPIO9  (not SPI function)
		{ 1, Func::SCK  }, // GPIO10 SPI1 SCK
		{ 1, Func::MOSI }, // GPIO11 SPI1 TX
		{ 1, Func::MISO }, // GPIO12 SPI1 RX
		{ -1, Func::None }, // GPIO13 (not SPI function)
		{ 1, Func::SCK  }, // GPIO14 SPI1 SCK
		{ 1, Func::MOSI }, // GPIO15 SPI1 TX
		{ 0, Func::MISO }, // GPIO16 SPI0 RX
		{ -1, Func::None }, // GPIO17 (not SPI function)
		{ 0, Func::SCK  }, // GPIO18 SPI0 SCK
		{ 0, Func::MOSI }, // GPIO19 SPI0 TX
		{ 0, Func::MISO }, // GPIO20 SPI0 RX
		{ -1, Func::None }, // GPIO21 (not SPI function)
		{ 0, Func::SCK  }, // GPIO22 SPI0 SCK
		{ 0, Func::MOSI }, // GPIO23 SPI0 TX
		{ 1, Func::MISO }, // GPIO24 SPI1 RX
		{ -1, Func::None }, // GPIO25 (not SPI function)
		{ 1, Func::SCK  }, // GPIO26 SPI1 SCK
		{ 1, Func::MOSI }, // GPIO27 SPI1 TX
	};
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("pin",		'p',	"sets GPIO pins for SPI (function auto-detected)", "SCK,MOSI[,MISO]"),
		Arg::OptString("pin-cs",	'c',	"sets CS pin (any GPIO)", "CS"),
		Arg::OptString("freq",		'f',	"sets SPI frequency (default: 1000000)", "FREQ"),
		Arg::OptString("timeout",	't',	"sets timeout for SPI operations (default: 300)", "MSEC"),
		Arg::OptString("mode",		'm',	"sets SPI mode (0-3, default: 0)", "MODE"),
		Arg::OptString("cpol",		0x0,	"sets clock polarity (0 or 1, default: 0)", "CPOL"),
		Arg::OptString("cpha",		0x0,	"sets clock phase (0 or 1, default: 0)", "CPHA"),
		Arg::OptString("order",		0x0,	"sets bit order (msb or lsb, default: msb)", "ORDER"),
		Arg::OptBool("dumb",		0x0,	"no SPI operations, just remember the pins and settings"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", argv[0]);
		arg.PrintHelp(tout);
		tout.Printf("Commands:\n");
		tout.Printf("  write:data   write data to SPI\n");
		tout.Printf("  read:N       read N bytes from SPI (requires MISO)\n");
		tout.Printf("  transfer:data transfer data (write and read simultaneously, requires MISO)\n");
		tout.Printf("  cs-low       set CS pin low\n");
		tout.Printf("  cs-high      set CS pin high\n");
		tout.Printf("  sleep:MSEC   sleep for specified milliseconds\n");
		tout.Printf("\nSPI Pin Functions:\n");
		tout.Printf("SPI0: SCK(2,6,18), MOSI(3,7,19), MISO(0,4,16)\n");
		tout.Printf("SPI1: SCK(10,14,26), MOSI(11,15,27), MISO(8,12,24)\n");
		tout.Printf("CS can be any GPIO pin (specified with --pin-cs)\n");
		tout.Printf("\nSPI Modes:\n");
		tout.Printf("Mode 0: CPOL=0, CPHA=0\n");
		tout.Printf("Mode 1: CPOL=0, CPHA=1\n");
		tout.Printf("Mode 2: CPOL=1, CPHA=0\n");
		tout.Printf("Mode 3: CPOL=1, CPHA=1\n");
		return Result::Success;
	}
	const char* value = nullptr;
	bool showInitMsg = false;
	if (arg.GetString("freq", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 100'000'000) {
			terr.Printf("Invalid frequency: %s\n", value);
			return Result::Error;
		}
		freq = static_cast<uint>(num);
		showInitMsg = true;
	}
	if (arg.GetString("timeout", &value)) {
		char* endptr;
		long num = ::strtol(value, &endptr, 0);
		if (*endptr != '\0' || num <= 0 || num > 10'000) {
			terr.Printf("Invalid timeout: %s\n", value);
			return Result::Error;
		}
		msecTimeout = static_cast<uint32_t>(num);
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
		cpol = (mode & 2) ? SPI_CPOL_1 : SPI_CPOL_0;
		cpha = (mode & 1) ? SPI_CPHA_1 : SPI_CPHA_0;
		showInitMsg = true;
	}
	if (arg.GetString("cpol", &value)) {
		int pol = ::strtol(value, nullptr, 0);
		if (pol == 0) {
			cpol = SPI_CPOL_0;
		} else if (pol == 1) {
			cpol = SPI_CPOL_1;
		} else {
			terr.Printf("Invalid clock polarity: %s (must be 0 or 1)\n", value);
			return Result::Error;
		}
		showInitMsg = true;
	}
	if (arg.GetString("cpha", &value)) {
		int pha = ::strtol(value, nullptr, 0);
		if (pha == 0) {
			cpha = SPI_CPHA_0;
		} else if (pha == 1) {
			cpha = SPI_CPHA_1;
		} else {
			terr.Printf("Invalid clock phase: %s (must be 0 or 1)\n", value);
			return Result::Error;
		}
		showInitMsg = true;
	}
	if (arg.GetString("order", &value)) {
		if (::strcasecmp(value, "msb") == 0) {
			order = SPI_MSB_FIRST;
		} else if (::strcasecmp(value, "lsb") == 0) {
			order = SPI_LSB_FIRST;
		} else {
			terr.Printf("Invalid bit order: %s (must be msb or lsb)\n", value);
			return Result::Error;
		}
		showInitMsg = true;
	}
	if (arg.GetString("pin", &value)) {
		Arg::EachNum each(value);
		int nNums;
		if (!each.CheckValidity(&nNums) || nNums < 2 || nNums > 3) {
			terr.Printf("Invalid pin specification: %s (expected 2-3 SPI pins: SCK,MOSI[,MISO])\n", value);
			return Result::Error;
		}
		
		// Reset SPI pin assignments (not CS)
		pinAssign.SCK = -1;
		pinAssign.MOSI = -1;
		pinAssign.MISO = -1;
		
		// Parse each pin and assign based on function
		int num;
		int iBus = -1;
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
			
			// Check if all pins belong to the same SPI bus
			if (iBus == -1) {
				iBus = info.iBus;
			} else if (iBus != info.iBus) {
				terr.Printf("All pins must belong to the same SPI bus (GPIO%d belongs to SPI%d, but previous pins belong to SPI%d)\n", 
					num, info.iBus, iBus);
				return Result::Error;
			}
			
			// Assign pin based on function
			switch (info.func) {
			case Func::SCK:
				if (pinAssign.SCK != -1) {
					terr.Printf("SCK pin already assigned (GPIO%d), cannot assign GPIO%d\n", pinAssign.SCK, num);
					return Result::Error;
				}
				pinAssign.SCK = num;
				break;
			case Func::MOSI:
				if (pinAssign.MOSI != -1) {
					terr.Printf("MOSI pin already assigned (GPIO%d), cannot assign GPIO%d\n", pinAssign.MOSI, num);
					return Result::Error;
				}
				pinAssign.MOSI = num;
				break;
			case Func::MISO:
				if (pinAssign.MISO != -1) {
					terr.Printf("MISO pin already assigned (GPIO%d), cannot assign GPIO%d\n", pinAssign.MISO, num);
					return Result::Error;
				}
				pinAssign.MISO = num;
				break;
			}
		}
		showInitMsg = true;
	}
	if (arg.GetString("pin-cs", &value)) {
		int csPin = ::strtol(value, nullptr, 0);
		if (csPin < 0 || csPin > 27) {
			terr.Printf("Invalid CS pin: %s (must be 0-27)\n", value);
			return Result::Error;
		}
		pinAssign.CS = csPin;
		showInitMsg = true;
	}
	if (pinAssign.SCK == -1 || pinAssign.MOSI == -1) {
		if (pinAssign.SCK == -1 && pinAssign.MOSI == -1) {
			terr.Printf("SCK and MOSI pins are required but not specified\n");
		} else if (pinAssign.SCK == -1) {
			terr.Printf("SCK pin is required but not specified\n");
		} else {
			terr.Printf("MOSI pin is required but not specified\n");
		}
		return Result::Error;
	}
	if (arg.GetBool("dumb")) return Result::Success;
	
	int iBus = infoTbl[pinAssign.SCK].iBus;
	SPI& spi = (iBus == 0) ? SPI0 : SPI1;
	
	// Initialize SPI
	spi.init(freq);
	spi.set_format(8, cpol, cpha, order);
	
	// Configure GPIO pins
	::gpio_set_function(pinAssign.SCK, GPIO_FUNC_SPI);
	::gpio_set_function(pinAssign.MOSI, GPIO_FUNC_SPI);
	if (pinAssign.MISO != -1) {
		::gpio_set_function(pinAssign.MISO, GPIO_FUNC_SPI);
	}
	
	if (pinAssign.CS != -1) {
		::gpio_init(pinAssign.CS);
		::gpio_set_dir(pinAssign.CS, GPIO_OUT);
		::gpio_put(pinAssign.CS, true); // CS high (inactive)
	}
	
	// Show initialization message only if options were specified
	if (showInitMsg || argc < 2) {
		int mode = (cpol << 1) | cpha;
		tout.Printf("SPI%d: SCK:GPIO%d MOSI:GPIO%d", 
			iBus, pinAssign.SCK, pinAssign.MOSI);
		if (pinAssign.MISO != -1) {
			tout.Printf(" MISO:GPIO%d", pinAssign.MISO);
		}
		if (pinAssign.CS != -1) {
			tout.Printf(" CS:GPIO%d", pinAssign.CS);
		}
		tout.Printf(" @ %d Hz, Mode%d (CPOL:%d CPHA:%d) %s\n", 
			freq, mode, cpol, cpha, (order == SPI_MSB_FIRST) ? "MSB" : "LSB");
	}
	
	int rtn = 0;
	if (argc >= 2) {
		Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
		if (!each.Initialize()) {
			terr.Printf("%s\n", each.GetErrorMsg());
			return Result::Error;
		}
		while (const char* arg = each.Next()) {
			const char* value;
			if (Arg::GetAssigned(arg, "write", &value)) {
				if (!WriteData(tout, terr, spi, value)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "read", &value)) {
				if (pinAssign.MISO == -1) {
					terr.Printf("MISO pin not configured, cannot read\n");
					rtn = 1;
					break;
				}
				if (!ReadData(tout, terr, spi, value)) {
					rtn = 1;
					break;
				}
			} else if (Arg::GetAssigned(arg, "transfer", &value)) {
				if (pinAssign.MISO == -1) {
					terr.Printf("MISO pin not configured, cannot transfer\n");
					rtn = 1;
					break;
				}
				if (!TransferData(tout, terr, spi, value)) {
					rtn = 1;
					break;
				}
			} else if (::strcasecmp(arg, "cs-low") == 0) {
				if (pinAssign.CS != -1) {
					::gpio_put(pinAssign.CS, false);
					tout.Printf("CS set low\n");
				} else {
					terr.Printf("CS pin not configured\n");
					rtn = 1;
					break;
				}
			} else if (::strcasecmp(arg, "cs-high") == 0) {
				if (pinAssign.CS != -1) {
					::gpio_put(pinAssign.CS, true);
					tout.Printf("CS set high\n");
				} else {
					terr.Printf("CS pin not configured\n");
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
	//tout.Printf("Wrote %d bytes\n", bytesWritten);
	return true;
}

bool ReadData(Printable& tout, Printable& terr, SPI& spi, const char* value)
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
	uint8_t dummy = 0xFF;
	int bytesRead = spi.read_blocking(dummy, data, bytesToRead);
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
	//tout.Printf("Transferred %d bytes, received:\n", bytesTransferred);
	Printable::DumpT dump(tout);
	dump.Addr(bytesToTransfer > dump.GetBytesPerRow())(rxData, bytesTransferred);
	return true;
}

}
