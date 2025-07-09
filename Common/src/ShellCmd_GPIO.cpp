#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/GPIOInfo.h"

using namespace jxglib;

static bool ProcessGPIO(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[]);
static void PrintPinFunc(Printable& tout, uint pin);

static const char* strAvailableDrive = "2ma, 4ma, 8ma, 12ma";
static const char* strAvailableSlew = "slow, fast";

ShellCmd(gpio, "controls GPIO pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "gpio") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Commands:\n");
		tout.Printf("  func=FUNCTION  set pin function (%s)\n", GPIOInfo::GetHelp_AvailableFunc());
		tout.Printf("  drive=STRENGTH set pin drive strength (%s)\n", strAvailableDrive);
		tout.Printf("  slew=SLEW      set slew rate (%s)\n", strAvailableSlew);
		tout.Printf("  pull-up        enable pull-up resistor\n");
		tout.Printf("  pull-down      enable pull-down resistor\n");
		tout.Printf("  pull-both      enable pull-up and pull-down resistor\n");
		tout.Printf("  pull-none      disable pull-up and pull-down resistor\n");
		tout.Printf("  dir=DIR        set pin direction (in or out)\n");
		tout.Printf("  lo (or 0)      set pin to low (effective for SIO OUT)\n");
		tout.Printf("  hi (or 1)      set pin to high (effective for SIO OUT)\n");
		tout.Printf("  toggle         toggle pin state (effective for SIO OUT)\n");
		return Result::Success;
	}
	int nArgsSkip = 0;
	int pinTbl[GPIO::NumPins];
	int nPins = 0;
	if (genericFlag) {
		if (argc < 2) {
			for (uint pin = 0; pin < GPIO::NumPins; ++pin) PrintPinFunc(tout, pin);
			return Result::Success;
		}
		Arg::EachNum eachNum(argv[1], GPIO::NumPins - 1);
		if (!eachNum.CheckValidity()) {
			terr.Printf("invalid GPIO pin number: %s\n", argv[1]);
			return Result::Error;
		}
		nPins = eachNum.GetAll(pinTbl, count_of(pinTbl));
		for (int i = 0; i < nPins; ++i) {
			int pin = pinTbl[i];
			if (pin < 0 || pin >= GPIO::NumPins) {
				terr.Printf("invalid GPIO pin number: %d\n", pin);
				return Result::Error;
			}
		}
		nArgsSkip = 2;
	} else if (::strncmp(GetName(), "gpio", 4) == 0) {
		pinTbl[0] = ::strtoul(GetName() + 4, nullptr, 0);
		nPins = 1;
		nArgsSkip = 1;
	}
	return ProcessGPIO(terr, tout, pinTbl, nPins, argc - nArgsSkip, argv + nArgsSkip)? Result::Success : Result::Error;
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

bool ProcessGPIO(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[])
{
	//uint pin = pinTbl[0];
	Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return false;
	}
	while (const char* subcmd = each.Next()) {
		const char* value = nullptr;
		if (Shell::Arg::GetAssigned(subcmd, "func", &value)) {
			if (!value) {
				terr.Printf("specify a function: %s\n", GPIOInfo::GetHelp_AvailableFunc());
				return false;
			}
			bool validFlag = false;
			gpio_function_t pinFunc = GPIOInfo::StringToFunc(value, &validFlag);
			if (!validFlag) {
				terr.Printf("unknown function: %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_set_function(pin, pinFunc);
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "drive", &value)) {
			if (!value) {
				terr.Printf("specify a drive strength: %s\n", strAvailableDrive);
				return false;
			}
			if (::strcasecmp(value, "2ma") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
				}
			} else if (::strcasecmp(value, "4ma") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_4MA);
				}
			} else if (::strcasecmp(value, "8ma") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_8MA);
				}
			} else if (::strcasecmp(value, "12ma") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_12MA);
				}
			} else {
				terr.Printf("unknown drive strength: %s\n", value);
				return false;
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "slew", &value)) {
			if (!value) {
				terr.Printf("specifiy a slew rate: %s\n", strAvailableSlew);
				return false;
			}
			if (::strcasecmp(value, "slow") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_slew_rate(pin, GPIO_SLEW_RATE_SLOW);
				}
			} else if (::strcasecmp(value, "fast") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_slew_rate(pin, GPIO_SLEW_RATE_FAST);
				}
			} else {
				terr.Printf("unknown slew rate: %s\n", value);
				return false;
			}
		} else if (::strcasecmp(subcmd, "pullup") == 0 || ::strcasecmp(subcmd, "pull-up") == 0) {
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_pull_up(pin);
			}
		} else if (::strcasecmp(subcmd, "pulldown") == 0 || ::strcasecmp(subcmd, "pull-down") == 0) {
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_pull_down(pin);
			}
		} else if (::strcasecmp(subcmd, "pullboth") == 0 || ::strcasecmp(subcmd, "pull-both") == 0) {
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_set_pulls(pin, true, true);
			}
		} else if (::strcasecmp(subcmd, "pullnone") == 0 || ::strcasecmp(subcmd, "pull-none") == 0) {
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_set_pulls(pin, false, false);
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "dir", &value)) {
			if (!value) {
				terr.Printf("specify a direction: in or out\n");
				return false;
			} else if (::strcasecmp(value, "in") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_dir(pin, GPIO_IN);
				}
			} else if (::strcasecmp(value, "out") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_dir(pin, GPIO_OUT);
				}
			} else {
				terr.Printf("unknown direction: %s\n", value);
				return false;
			}
		} else if (::strcasecmp(subcmd, "hi") == 0 || ::strcasecmp(subcmd, "high") == 0 ||
					::strcasecmp(subcmd, "true") == 0 || ::strcmp(subcmd, "1") == 0) {
			uint32_t mask = 0;
			for (int i = 0; i < nPins; ++i) mask |= (1 << pinTbl[i]);
			::gpio_set_mask(mask);
		} else if (::strcasecmp(subcmd, "lo") == 0 || ::strcasecmp(subcmd, "low") == 0 ||
					::strcasecmp(subcmd, "false") == 0 || ::strcmp(subcmd, "0") == 0) {
			uint32_t mask = 0;
			for (int i = 0; i < nPins; ++i) mask |= (1 << pinTbl[i]);
			::gpio_clr_mask(mask);
		} else if (::strcasecmp(subcmd, "toggle") == 0) {
			uint32_t mask = 0;
			for (int i = 0; i < nPins; ++i) mask |= (1 << pinTbl[i]);
			::gpio_xor_mask(mask);
		} else if (Shell::Arg::GetAssigned(subcmd, "sleep", &value)) {
			if (!value) {
				terr.Printf("specify a sleep duration in milliseconds\n");
				return false;
			}
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				return false;
			}
			Tickable::Sleep(msec);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return false;
		}
		if (Tickable::TickSub()) return true;
	}
	for (int i = 0; i < nPins; ++i) {
		uint pin = pinTbl[i];
		PrintPinFunc(tout, pin);
	}
	return true;
}

void PrintPinFunc(Printable& tout, uint pin)
{
	gpio_function_t pinFunc = ::gpio_get_function(pin);
	gpio_drive_strength driveStrength = ::gpio_get_drive_strength(pin);
	gpio_slew_rate slewRate = ::gpio_get_slew_rate(pin);
	tout.Printf("GPIO%-2u %-10s %-2s %-9s %4s %-4s\n",
		pin,
		(pinFunc == GPIO_FUNC_SIO)? ((::gpio_get_dir(pin) == GPIO_OUT)? "SIO OUT" : "SIO IN") : GPIOInfo::GetFuncName(pinFunc, pin, "------"),
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
