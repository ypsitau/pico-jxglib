#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"

using namespace jxglib;

static void PrintPinFunc(Printable& tout, uint pin);
static const char* GetPinFuncName(int pinFunc, uint pin);
static bool ProcessGPIO(Printable& terr, Printable& tout, uint pin, int argc, char* argv[]);

#if defined(PICO_RP2040)
static const char* strAvailableFunction = "spi, uart, i2c, pwm, sio, pio0, pio1, clock, usb, xip, null";
#elif defined(PICO_RP2350)
static const char* strAvailableFunction = "spi, uart, uart-aux, i2c, pwm, sio, pio0, pio1, pio2, clock, usb, hstx, xip-cs1, coresight-trace, null";
#endif
static const char* strAvailableDrive = "2ma, 4ma, 8ma, 12ma, 16ma";
static const char* strAvailableSlew = "slow, fast";

ShellCmd(gpio, "controls GPIO pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	bool genericFlag = (::strcmp(GetName(), "gpio") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Commands:\n");
		tout.Printf("  func FUNCTION  set pin function (%s)\n", strAvailableFunction);
		tout.Printf("  drive=STRENGTH set pin drive strength (%s)\n", strAvailableDrive);
		tout.Printf("  slew=SLEW      set slew rate (%s)\n", strAvailableSlew);
		tout.Printf("  pull-up        enable pull-up resistor\n");
		tout.Printf("  pull-down      enable pull-down resistor\n");
		tout.Printf("  pull-both      enable pull-up and pull-down resistor\n");
		tout.Printf("  pull-none      disable pull-up and pull-down resistor\n");
		tout.Printf("  dir-in         set pin direction to input\n");
		tout.Printf("  dir-out        set pin direction to output\n");
		tout.Printf("  lo (or 0)      set pin to low (effective for SIO OUT)\n");
		tout.Printf("  hi (or 1)      set pin to high (effective for SIO OUT)\n");
		return 0;
	}
	if (genericFlag) {
		if (argc < 2) {
			for (uint pin = 0; pin < GPIO::NumPins; ++pin) PrintPinFunc(tout, pin);
			return 0;
		}
		Arg::EachNum eachNum(argv[1], GPIO::NumPins - 1);
		if (!eachNum.CheckValidity()) {
			terr.Printf("invalid GPIO pin number: %s\n", argv[1]);
			return 1;
		}
		for (int num = 0; eachNum.Next(&num); ) {
			if (num < 0 || num >= GPIO::NumPins) {
				terr.Printf("invalid GPIO pin number: %d\n", num);
				return 1;
			}
			uint pin = static_cast<uint>(num);
			if (!ProcessGPIO(terr, tout, pin, argc - 2, argv + 2)) return 1;
		}
		return 0;
	} else if (::strncmp(GetName(), "gpio", 4) == 0) {
		char* endptr;
		auto num = ::strtoul(GetName() + 4, &endptr, 0);
		uint pin = 0;
		if (*endptr == '\0' && num < GPIO::NumPins) pin = static_cast<uint>(num);
		return ProcessGPIO(terr, tout, pin, argc - 1, argv + 1)? 0 : 1;
	}
	return 1;
}

ShellCmdAlias(gpio0, gpio)
ShellCmdAlias(gpio1, gpio)
ShellCmdAlias(gpio2, gpio)
ShellCmdAlias(gpio3, gpio)
ShellCmdAlias(gpio4, gpio)
ShellCmdAlias(gpio5, gpio)
ShellCmdAlias(gpio6, gpio)
ShellCmdAlias(gpio7, gpio)
ShellCmdAlias(gpio8, gpio)
ShellCmdAlias(gpio9, gpio)
ShellCmdAlias(gpio10, gpio)
ShellCmdAlias(gpio11, gpio)
ShellCmdAlias(gpio12, gpio)
ShellCmdAlias(gpio13, gpio)
ShellCmdAlias(gpio14, gpio)
ShellCmdAlias(gpio15, gpio)
ShellCmdAlias(gpio16, gpio)
ShellCmdAlias(gpio17, gpio)
ShellCmdAlias(gpio18, gpio)
ShellCmdAlias(gpio19, gpio)
ShellCmdAlias(gpio20, gpio)
ShellCmdAlias(gpio21, gpio)
ShellCmdAlias(gpio22, gpio)
ShellCmdAlias(gpio23, gpio)
ShellCmdAlias(gpio24, gpio)
ShellCmdAlias(gpio25, gpio)
ShellCmdAlias(gpio26, gpio)
ShellCmdAlias(gpio27, gpio)
ShellCmdAlias(gpio28, gpio)
ShellCmdAlias(gpio29, gpio)

bool ProcessGPIO(Printable& terr, Printable& tout, uint pin, int argc, char* argv[])
{
	for (int iArg = 0; iArg < argc; ++iArg) {
		const char* cmd = argv[iArg];
		const char* value = nullptr;
		if (Shell::Arg::GetAssigned(cmd, "func", &value) || Shell::Arg::GetAssigned(cmd, "function", &value)) {
			if (!value) {
				terr.Printf("specify a function: %s\n", strAvailableFunction);
				return false;
			}
			if (::strcasecmp(value, "spi") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_SPI);
			} else if (::strcasecmp(value, "uart") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_UART);
			} else if (::strcasecmp(value, "i2c") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_I2C);
			} else if (::strcasecmp(value, "pwm") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_PWM);
			} else if (::strcasecmp(value, "sio") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_SIO);
			} else if (::strcasecmp(value, "pio0") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_PIO0);
			} else if (::strcasecmp(value, "pio1") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_PIO1);
			} else if (::strcasecmp(value, "clock") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_GPCK);
			} else if (::strcasecmp(value, "usb") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_USB);
			} else if (::strcasecmp(value, "null") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_NULL);
#if defined(PICO_RP2040)
			} else if (::strcasecmp(value, "xip") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_XIP);
#elif defined(PICO_RP2350)
			} else if (::strcasecmp(value, "hstx") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_HSTX);
			} else if (::strcasecmp(value, "pio2") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_PIO2);
			} else if (::strcasecmp(value, "xip-cs1") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_XIP_CS1);
			} else if (::strcasecmp(value, "coresight-trace") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_CORESIGHT_TRACE);
			} else if (::strcasecmp(value, "uart-aux") == 0) {
				::gpio_set_function(pin, GPIO_FUNC_UART_AUX);
#endif
			} else {
				terr.Printf("unknown function: %s\n", value);
				return false;
			}
		} else if (Shell::Arg::GetAssigned(cmd, "drive", &value) || Shell::Arg::GetAssigned(cmd, "drive-strength", &value)) {
			if (!value) {
				terr.Printf("specify a drive strength: %s\n", strAvailableDrive);
				return false;
			}
			if (::strcasecmp(value, "2ma") == 0) {
				::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
			} else if (::strcasecmp(value, "4ma") == 0) {
				::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_4MA);
			} else if (::strcasecmp(value, "8ma") == 0) {
				::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_8MA);
			} else if (::strcasecmp(value, "12ma") == 0) {
				::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_12MA);
			} else {
				terr.Printf("unknown drive strength: %s\n", value);
				return false;
			}
		} else if (Shell::Arg::GetAssigned(cmd, "slew", &value) || Shell::Arg::GetAssigned(cmd, "slew-rate", &value)) {
			if (!value) {
				terr.Printf("specifiy a slew rate: %s\n", strAvailableSlew);
				return false;
			}
			if (::strcasecmp(value, "slow") == 0) {
				::gpio_set_slew_rate(pin, GPIO_SLEW_RATE_SLOW);
			} else if (::strcasecmp(value, "fast") == 0) {
				::gpio_set_slew_rate(pin, GPIO_SLEW_RATE_FAST);
			} else {
				terr.Printf("unknown slew rate: %s\n", value);
				return false;
			}
		} else if (::strcasecmp(cmd, "pullup") == 0 || ::strcasecmp(cmd, "pull-up") == 0) {
			::gpio_pull_up(pin);
		} else if (::strcasecmp(cmd, "pulldown") == 0 || ::strcasecmp(cmd, "pull-down") == 0) {
			::gpio_pull_down(pin);
		} else if (::strcasecmp(cmd, "pullboth") == 0 || ::strcasecmp(cmd, "pull-both") == 0) {
			::gpio_set_pulls(pin, true, true);
		} else if (::strcasecmp(cmd, "pullnone") == 0 || ::strcasecmp(cmd, "pull-none") == 0) {
			::gpio_set_pulls(pin, false, false);
		} else if (::strcasecmp(cmd, "dir-in") == 0) {
			::gpio_set_function(pin, GPIO_FUNC_SIO);
			::gpio_set_dir(pin, GPIO_IN);
		} else if (::strcasecmp(cmd, "dir-out") == 0) {
			::gpio_set_function(pin, GPIO_FUNC_SIO);
			::gpio_set_dir(pin, GPIO_OUT);
		} else if (::strcasecmp(cmd, "hi") == 0 || ::strcasecmp(cmd, "high") == 0 ||
					::strcasecmp(cmd, "true") == 0 || ::strcmp(cmd, "1") == 0) {
			::gpio_put(pin, true);
		} else if (::strcasecmp(cmd, "lo") == 0 || ::strcasecmp(cmd, "low") == 0 ||
					::strcasecmp(cmd, "false") == 0 || ::strcmp(cmd, "0") == 0) {
			::gpio_put(pin, false);
		} else {
			terr.Printf("unknown command: %s\n", cmd);
			return false;
		}
	}
	PrintPinFunc(tout, pin);
	return true;
}

void PrintPinFunc(Printable& tout, uint pin)
{
	gpio_function_t pinFunc = ::gpio_get_function(pin);
	gpio_drive_strength driveStrength = ::gpio_get_drive_strength(pin);
	gpio_slew_rate slewRate = ::gpio_get_slew_rate(pin);
	tout.Printf("GPIO%-2u %-10s %-2s %-9s %4s %-4s\n",
		pin,
		GetPinFuncName(pinFunc, pin),
		::gpio_get(pin)? "hi" : "lo",
		(!::gpio_is_pulled_down(pin) && !::gpio_is_pulled_up(pin))? "pull-none" :
		(::gpio_is_pulled_down(pin) && ::gpio_is_pulled_up(pin))? "pull-both" :
		::gpio_is_pulled_down(pin)? "pull-down" : "pull-up",
		(driveStrength == GPIO_DRIVE_STRENGTH_2MA)? "2mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_4MA)? "4mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_8MA)? "8mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_12MA)? "12mA" : "",
		(slewRate == GPIO_SLEW_RATE_SLOW)? "slow" :
		(slewRate == GPIO_SLEW_RATE_FAST)? "fast" : "");
}

const char* GetPinFuncName(int pinFunc, uint pin)
{
	if (pinFunc == GPIO_FUNC_SIO) return (::gpio_get_dir(pin) == GPIO_OUT)? "SIO OUT" : "SIO IN";
	if (pinFunc == GPIO_FUNC_NULL) return "NULL";
#if defined(PICO_RP2040)
	// RP2040 Datasheet (rp2040-datasheet.pdf) 1.4.3 GPIO Functions
	static const char* funcNameTbl[][10] = {
		{ "XIP",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 0
		{ "XIP",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 1
		{ "XIP",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 2
		{ "XIP",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 3
		{ "XIP",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 4
		{ "XIP",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 5
		{ "XIP",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 6
		{ "XIP",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 7
		{ "XIP",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 8
		{ "XIP",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 9
		{ "XIP",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 10
		{ "XIP",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 11
		{ "XIP",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 12
		{ "XIP",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 13
		{ "XIP",	"SPI1 SCK",	"UART0 CTS","I2C1 SDA",	"PWM7 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 14
		{ "XIP",	"SPI1 TX",	"UART0 RTS","I2C1 SCL",	"PWM7 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 15
		{ "XIP",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 16
		{ "XIP",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 17
		{ "XIP",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 18
		{ "XIP",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 19
		{ "XIP",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPIN0",	"USB VBUS EN",		}, // 20
		{ "XIP",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT0",	"USB OVCUR DET",	}, // 21
		{ "XIP",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPIN1",	"USB VBUS DET",		}, // 22
		{ "XIP",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT1",	"USB VBUS EN",		}, // 23
		{ "XIP",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT2",	"USB OVCUR DET",	}, // 24
		{ "XIP",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT3",	"USB VBUS DET",		}, // 25
		{ "XIP",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 26
		{ "XIP",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 27
		{ "XIP",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 28
		{ "XIP",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#elif defined(PICO_RP2350)
	// RP2350 Datasheet (rp2350-datasheet.pdf) 1.2.3 GPIO Functions (Bank 0)
	static const char* funcNameTbl[][12] = {
		{ "",		"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB OVCUR DET",	"",			}, // 0
		{ "",		"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACECLK",		"USB VBUS DET",		"",			}, // 1
		{ "",		"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA0",	"USB VBUS EN",		"UART0 TX",	}, // 2
		{ "",		"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA1",	"USB OVCUR DET",	"UART0 RX",	}, // 3
		{ "",		"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA2",	"USB VBUS DET",		"",			}, // 4
		{ "",		"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA3",	"USB VBUS EN",		"",			}, // 5
		{ "",		"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 TX",	}, // 6
		{ "",		"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 RX",	}, // 7
		{ "",		"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS EN",		"",			}, // 8
		{ "",		"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"",			}, // 9
		{ "",		"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 TX",	}, // 10
		{ "",		"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 RX",	}, // 11
		{ "HSTX",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB OVCUR DET",	"",			}, // 12
		{ "HSTX",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB VBUS DET",		"",			}, // 13
		{ "HSTX",	"SPI1 SCK",	"UART0 CTS","I2C1 SDA",	"PWM7 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS EN",		"UART0 TX",	}, // 14
		{ "HSTX",	"SPI1 TX",	"UART0 RTS","I2C1 SCL",	"PWM7 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB OVCUR DET",	"UART0 RX",	}, // 15
		{ "HSTX",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 16
		{ "HSTX",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 17
		{ "HSTX",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART0 TX",	}, // 18
		{ "HSTX",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS DET",		"UART0 RX",	}, // 19
		{ "",		"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB VBUS EN",		"",			}, // 20
		{ "",		"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB OVCUR DET",	"",			}, // 21
		{ "",		"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS DET",		"UART1 TX",	}, // 22
		{ "",		"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB VBUS EN",		"UART1 RX",	}, // 23
		{ "",		"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT2",	"USB OVCUR DET",	"",			}, // 24
		{ "",		"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT3",	"USB VBUS DET",		"",			}, // 25
		{ "",		"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 TX",	}, // 26
		{ "",		"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 RX",	}, // 27
		{ "",		"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 28
		{ "",		"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#else
	return "unknown";
#endif
}
