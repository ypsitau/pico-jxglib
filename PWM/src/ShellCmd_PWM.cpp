//==============================================================================
// ShellCmd_PWM.cpp
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/PWM.h"
#include "jxglib/GPIO.h"
#include "jxglib/GPIOInfo.h"

using namespace jxglib;

static void PrintPwmStatus(Printable& tout, uint pin);
static bool ProcessPWM(Printable& terr, Printable& tout, uint pin, int argc, char* argv[]);

static const char* strAvailableCommands = "set, freq, wrap, level, duty, clkdiv, enable, disable, counter";

ShellCmd(pwm, "controls PWM pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h', "prints this help"),
		Arg::OptBool("onlypwm",	'n', "only show PWM-capable pins"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	bool genericFlag = (::strcmp(GetName(), "pwm") == 0);
	bool onlyPWMFlag = arg.GetBool("onlypwm");
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Commands:\n");
		tout.Printf("  func=FUNCTION     set pin function (%s)\n", GPIOInfo::GetHelp_AvailableFunc());
		tout.Printf("  freq=FREQUENCY    set PWM frequency in Hz\n");
		tout.Printf("  wrap=VALUE        set PWM wrap value (0-65535)\n");
		tout.Printf("  level=VALUE       set PWM level (0-65535)\n");
		tout.Printf("  duty=RATIO        set PWM duty ratio (0.0-1.0)\n");
		tout.Printf("  clkdiv=DIVIDER    set PWM clock divider (1.0-256.0)\n");
		tout.Printf("  enable            start PWM output\n");
		tout.Printf("  disable           stop PWM output\n");
		tout.Printf("  counter           get PWM counter value\n");
		return 0;
	}
	if (genericFlag) {
		if (argc < 2) {
			for (uint pin = 0; pin < GPIO::NumPins; ++pin) {
				if (!onlyPWMFlag || ::gpio_get_function(pin) == GPIO_FUNC_PWM) {
					PrintPwmStatus(tout, pin);
				}
			}
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
			// Check if pin supports PWM
			if (pwm_gpio_to_slice_num(pin) >= NUM_PWM_SLICES) {
				terr.Printf("GPIO pin %u does not support PWM\n", pin);
				return 1;
			}
			if (!ProcessPWM(terr, tout, pin, argc - 2, argv + 2)) return 1;
		}
		return 0;
	} else if (::strncmp(GetName(), "pwm", 3) == 0) {
		char* endptr;
		auto num = ::strtoul(GetName() + 3, &endptr, 0);
		uint pin = 0;
		if (*endptr == '\0' && num < GPIO::NumPins) pin = static_cast<uint>(num);
		// Check if pin supports PWM
		if (pwm_gpio_to_slice_num(pin) >= NUM_PWM_SLICES) {
			terr.Printf("GPIO pin %u does not support PWM\n", pin);
			return 1;
		}
		return ProcessPWM(terr, tout, pin, argc - 1, argv + 1)? 0 : 1;
	}
	return 1;
}

// Create PWM pin aliases similar to GPIO
ShellCmdAlias(pwm0, pwm)
ShellCmdAlias(pwm1, pwm)
ShellCmdAlias(pwm2, pwm)
ShellCmdAlias(pwm3, pwm)
ShellCmdAlias(pwm4, pwm)
ShellCmdAlias(pwm5, pwm)
ShellCmdAlias(pwm6, pwm)
ShellCmdAlias(pwm7, pwm)
ShellCmdAlias(pwm8, pwm)
ShellCmdAlias(pwm9, pwm)
ShellCmdAlias(pwm10, pwm)
ShellCmdAlias(pwm11, pwm)
ShellCmdAlias(pwm12, pwm)
ShellCmdAlias(pwm13, pwm)
ShellCmdAlias(pwm14, pwm)
ShellCmdAlias(pwm15, pwm)
ShellCmdAlias(pwm16, pwm)
ShellCmdAlias(pwm17, pwm)
ShellCmdAlias(pwm18, pwm)
ShellCmdAlias(pwm19, pwm)
ShellCmdAlias(pwm20, pwm)
ShellCmdAlias(pwm21, pwm)
ShellCmdAlias(pwm22, pwm)
ShellCmdAlias(pwm23, pwm)
ShellCmdAlias(pwm24, pwm)
ShellCmdAlias(pwm25, pwm)
ShellCmdAlias(pwm26, pwm)
ShellCmdAlias(pwm27, pwm)
ShellCmdAlias(pwm28, pwm)
ShellCmdAlias(pwm29, pwm)

bool ProcessPWM(Printable& terr, Printable& tout, uint pin, int argc, char* argv[])
{
	PWM pwm(pin);
	
	for (int iArg = 0; iArg < argc; ++iArg) {
		const char* cmd = argv[iArg];
		const char* value = nullptr;
		
		if (Shell::Arg::GetAssigned(cmd, "func", &value)) {
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
			::gpio_set_function(pin, pinFunc);
		} else if (Shell::Arg::GetAssigned(cmd, "freq", &value) || Shell::Arg::GetAssigned(cmd, "frequency", &value)) {
			if (!value) {
				terr.Printf("specify a frequency value in Hz (8-1000000)\n");
				return false;
			}
			uint32_t freq = ::strtoul(value, nullptr, 0);
			if (freq < 8 || freq > 1000000) {
				terr.Printf("invalid frequency value: %s\n", value);
				return false;
			}
			//float duty = pwm.get_chan_duty();
			pwm.set_freq(freq);
			//pwm.set_chan_duty(duty);
		} else if (Shell::Arg::GetAssigned(cmd, "wrap", &value)) {
			if (!value) {
				terr.Printf("specify a wrap value (0-65535)\n");
				return false;
			}
			uint32_t wrap_val = ::strtoul(value, nullptr, 0);
			if (wrap_val > 65535) {
				terr.Printf("wrap value out of range (0-65535): %s\n", value);
				return false;
			}
			pwm.set_wrap(static_cast<uint16_t>(wrap_val));
		} else if (Shell::Arg::GetAssigned(cmd, "level", &value)) {
			if (!value) {
				terr.Printf("specify a level value (0-65535)\n");
				return false;
			}
			uint32_t level_val = ::strtoul(value, nullptr, 0);
			if (level_val > 65535) {
				terr.Printf("level value out of range (0-65535): %s\n", value);
				return false;
			}
			pwm.set_chan_level(static_cast<uint16_t>(level_val));
		} else if (Shell::Arg::GetAssigned(cmd, "duty", &value)) {
			if (!value) {
				terr.Printf("specify a duty ratio (0.0-1.0)\n");
				return false;
			}
			float duty_val = ::strtof(value, nullptr);
			if (duty_val < 0.0f || duty_val > 1.0f) {
				terr.Printf("duty ratio out of range (0.0-1.0): %s\n", value);
				return false;
			}
			pwm.set_chan_duty(duty_val);
		} else if (Shell::Arg::GetAssigned(cmd, "clkdiv", &value) || Shell::Arg::GetAssigned(cmd, "divider", &value)) {
			if (!value) {
				terr.Printf("specify a clock divider (1.0-256.0)\n");
				return false;
			}
			float div_val = ::strtof(value, nullptr);
			if (div_val < 1.0f || div_val > 256.0f) {
				terr.Printf("clock divider out of range (1.0-256.0): %s\n", value);
				return false;
			}
			pwm.set_clkdiv(div_val);
		} else if (::strcasecmp(cmd, "enable") == 0) {
			pwm.set_enabled(true);
		} else if (::strcasecmp(cmd, "disable") == 0) {
			pwm.set_enabled(false);
		} else if (::strcasecmp(cmd, "counter") == 0) {
			uint16_t counter = pwm.get_counter();
		} else {
			terr.Printf("unknown command: %s\n", cmd);
			terr.Printf("available commands: %s\n", strAvailableCommands);
			return false;
		}
	}
	PrintPwmStatus(tout, pin);
	return true;
}

void PrintPwmStatus(Printable& tout, uint pin)
{
	gpio_function_t pinFunc = ::gpio_get_function(pin);
	if (pinFunc == GPIO_FUNC_PWM) {
		PWM pwm(pin);
		tout.Printf("GPIO%-2u PWM%u %c %-8s freq=%uHz (clkdiv=%.1f wrap=0x%04x) duty=%.3f (level=0x%04x) counter=0x%04x\n",
			pin, pwm.GetSliceNum(), pwm.IsChannelA()? 'A' : 'B',
			pwm.is_enabled() ? "enabled" : "disabled",
			pwm.get_freq(), pwm.get_clkdiv(), pwm.get_wrap(), pwm.get_chan_duty(), pwm.get_chan_level(), pwm.get_counter());
	} else {
		tout.Printf("GPIO%-2u %s\n", pin, GPIOInfo::GetFuncName(::gpio_get_function(pin), pin, "----"));
	}
}
