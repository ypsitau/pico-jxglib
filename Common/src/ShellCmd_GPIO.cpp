#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/GPIOInfo.h"

namespace jxglib::ShellCmd_GPIO {

static bool ProcessGPIO(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[], bool quietFlag);
static void PrintPinFunc(Printable& tout, uint pin);

static const char* strAvailableDrive = "2mA, 4mA, 8mA, 12mA";
static const char* strAvailableSlew = "slow, fast";

ShellCmd(gpio, "controls GPIO pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("quiet",		'Q',	"quiet mode, suppresses output of current pin status"),
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
		tout.Printf("Sub Commands:\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  put:VALUE            drive the pin (0, 1, lo, hi)\n");;
		tout.Printf("  toggle               toggle pin value\n");
		tout.Printf("  func:FUNCTION        set pin function (%s)\n", GPIOInfo::GetHelp_AvailableFunc());
		tout.Printf("  dir:DIR              set pin direction (in, out)\n");
		tout.Printf("  pull:DIR             set pull direction (up, down, both, none)\n");
		tout.Printf("  drive:STRENGTH       set pin drive strength (%s)\n", strAvailableDrive);
		tout.Printf("  slew:SLEW            set slew rate (%s)\n", strAvailableSlew);
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
	bool quietFlag = arg.GetBool("quiet");
	return ProcessGPIO(terr, tout, pinTbl, nPins, argc - nArgsSkip, argv + nArgsSkip, quietFlag)? Result::Success : Result::Error;
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

bool ProcessGPIO(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[], bool quietFlag)
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
		} else if (Shell::Arg::GetAssigned(subcmd, "pull", &value)) {
			bool up = false, down = false;
			if (!value) {
				terr.Printf("specify a pull direction: up, down, both, none\n");
				return false;
			} else if (::strcasecmp(value, "up") == 0) {
				up = true;
			} else if (::strcasecmp(value, "down") == 0) {
				down = true;
			} else if (::strcasecmp(value, "both") == 0) {
				up = down = true;
			} else if (::strcasecmp(value, "none") == 0) {
				up = down = false;
			} else {
				terr.Printf("unknown pull direction: %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				::gpio_set_pulls(pin, up, down);
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "dir", &value)) {
			if (!value) {
				terr.Printf("specify a direction: in, out\n");
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
			} else if (::strcmp(value, "-") == 0 || ::strcmp(value, "--") == 0 || ::strcmp(value, "---") == 0) {
				for (int i = 0; i < nPins; ++i) {
					uint pin = pinTbl[i];
					::gpio_set_dir(pin, GPIO_IN);
					::gpio_set_input_enabled(pin, false);
				}
			} else {
				terr.Printf("unknown direction: %s\n", value);
				return false;
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "put", &value)) {
			if (!value) {
				terr.Printf("specify a value: lo, hi, 0, 1\n");
				return false;
			}
			uint32_t mask = 0;
			for (int i = 0; i < nPins; ++i) mask |= (1 << pinTbl[i]);
			if (::strcasecmp(value, "hi") == 0 || ::strcasecmp(value, "high") == 0 ||
				::strcasecmp(value, "true") == 0 || ::strcmp(value, "1") == 0) {
				::gpio_set_mask(mask);
			} else if (::strcasecmp(value, "lo") == 0 || ::strcasecmp(value, "low") == 0 ||
						::strcasecmp(value, "false") == 0 || ::strcmp(value, "0") == 0) {
				::gpio_clr_mask(mask);
			} else {
				terr.Printf("unknown value: %s\n", value);
				return false;
			}
		} else if (::strcasecmp(subcmd, "toggle") == 0) {
			uint32_t mask = 0;
			for (int i = 0; i < nPins; ++i) mask |= (1 << pinTbl[i]);
			::gpio_xor_mask(mask);
		} else if (::strcasecmp(subcmd, "get") == 0) {
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				if (i > 0) tout.Printf(" ");
				tout.Printf("%s", ::gpio_get(pin)? "hi" : "lo");
			}
			tout.Println();
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
	if (!quietFlag) {
		for (int i = 0; i < nPins; ++i) {
			uint pin = pinTbl[i];
			PrintPinFunc(tout, pin);
		}
	}
	return true;
}

void PrintPinFunc(Printable& tout, uint pin)
{
	gpio_function_t pinFunc = ::gpio_get_function(pin);
	gpio_drive_strength driveStrength = ::gpio_get_drive_strength(pin);
	gpio_slew_rate slewRate = ::gpio_get_slew_rate(pin);
	tout.Printf("GPIO%-2u %s%s func:%-10s dir:%-3s pull:%-4s drive:%s slew:%s\n",
		pin,
		::gpio_get(pin)? "hi" : "lo",
		(::gpio_get(pin) == ::gpio_get_out_level(pin))? " " : "~",
		GPIOInfo::GetFuncName(pinFunc, pin, "------"),
		(::gpio_get_dir(pin) == GPIO_OUT)? "out" : "in",
		(!::gpio_is_pulled_down(pin) && !::gpio_is_pulled_up(pin))? "none" :
		(::gpio_is_pulled_down(pin) && ::gpio_is_pulled_up(pin))? "both" :
		::gpio_is_pulled_down(pin)? "down" : "up",
		(driveStrength == GPIO_DRIVE_STRENGTH_2MA)? "2mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_4MA)? "4mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_8MA)? "8mA" :
		(driveStrength == GPIO_DRIVE_STRENGTH_12MA)? "12mA" : "",
		(slewRate == GPIO_SLEW_RATE_SLOW)? "slow" :
		(slewRate == GPIO_SLEW_RATE_FAST)? "fast" : "");
}

}
