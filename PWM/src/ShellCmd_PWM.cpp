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

namespace jxglib::ShellCmd_PWM {

static bool ProcessPWM(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[], bool onlyPWMFlag, bool dumbFlag, bool builtinLEDFlag);
static void PrintPWMStatus(Printable& tout, uint pin, bool onlyPWMFlag, bool builtinLEDFlag);

static const char* strAvailableCommands = "func, enable, disable, freq, wrap, level, duty, clkdiv, phase-correct, invert, counter";

inline bool IsAccessiblePin(uint pin, bool builtinLEDFlag) { return pin != 23 && pin != 24 && pin != 29 && (pin != 25 || builtinLEDFlag); }

ShellCmd(pwm, "controls PWM pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h', "prints this help"),
		Arg::OptBool("only-pwm",	'n', "only show PWM-capable pins"),
		Arg::OptBool("quiet",		'Q', "do not print any status information"),
		Arg::OptBool("builtin-led",	'B', "use the built-in LED (GPIO 25)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "pwm") == 0);
	bool onlyPWMFlag = arg.GetBool("only-pwm");
	bool quietFlag = arg.GetBool("quiet");
	bool builtinLEDFlag = arg.GetBool("builtin-led");
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" func:FUNCTION      set pin function (%s)\n", GPIOInfo::GetHelp_AvailableFunc());
		tout.Printf(" enable             start PWM output\n");
		tout.Printf(" disable            stop PWM output\n");
		tout.Printf(" freq:FREQUENCY     set PWM frequency in Hz\n");
		tout.Printf(" duty:RATIO         set PWM duty ratio (0.0-1.0)\n");
		tout.Printf(" clkdiv:DIVIDER     set PWM clock divider (1.0-255.9)\n");
		tout.Printf(" wrap:VALUE         set PWM wrap value (0-65535)\n");
		tout.Printf(" level:VALUE        set PWM level (0-65535)\n");
		tout.Printf(" phase-correct:BOOL enable/disable phase-correct (0, 1)\n");
		tout.Printf(" invert:BOOL        enable/disable inverted output (0, 1)\n");
		tout.Printf(" counter:VALUE      set PWM counter value (0-65535)\n");
		return Result::Success;
	}
	int pinTbl[GPIO::NumPins];
	int nPins = 0;
	int nArgsSkip = 0;
	if (genericFlag) {
		if (argc < 2) {
			for (uint pin = 0; pin < GPIO::NumPins; ++pin) {
				PrintPWMStatus(tout, pin, onlyPWMFlag, builtinLEDFlag);
			}
			return Result::Success;
		}
		Arg::EachNum eachNum(argv[1], GPIO::NumPins - 1);
		if (!eachNum.CheckValidity(&nPins)) {
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
	} else if (::strncmp(GetName(), "pwm", 3) == 0) {
		pinTbl[0] = ::strtoul(GetName() + 3, nullptr, 0);
		nPins = 1;
		nArgsSkip = 1;
	}
	return ProcessPWM(terr, tout, pinTbl, nPins, argc - nArgsSkip, argv + nArgsSkip, onlyPWMFlag, quietFlag, builtinLEDFlag)? Result::Success : Result::Error;
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

bool ProcessPWM(Printable& terr, Printable& tout, const int pinTbl[], int nPins, int argc, char* argv[], bool onlyPWMFlag, bool quietFlag, bool builtinLEDFlag)
{
	Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return false;
	}
	while (const char* cmd = each.Next()) {
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
			for (int i = 0; i < nPins; ++i) {
				uint pin = pinTbl[i];
				if (IsAccessiblePin(pin, builtinLEDFlag)) ::gpio_set_function(pin, pinFunc);
			}
		} else if (::strcasecmp(cmd, "enable") == 0) {
			uint32_t mask = PWM::get_mask_enabled();
			for (int i = 0; i < nPins; ++i) mask |= PWM(pinTbl[i]).GetSliceMask();
			PWM::set_mask_enabled(mask);
		} else if (::strcasecmp(cmd, "disable") == 0) {
			uint32_t mask = PWM::get_mask_enabled();
			for (int i = 0; i < nPins; ++i) mask &= ~PWM(pinTbl[i]).GetSliceMask();
			PWM::set_mask_enabled(mask);
		} else if (Shell::Arg::GetAssigned(cmd, "freq", &value) || Shell::Arg::GetAssigned(cmd, "frequency", &value)) {
			if (!value) {
				terr.Printf("specify a frequency value in Hz\n");
				return false;
			}
			char* endptr;
			uint32_t freq = ::strtoul(value, &endptr, 0);
			if (*endptr != '\0' || freq == 0) {
				terr.Printf("invalid frequency value: %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_freq(freq);
		} else if (Shell::Arg::GetAssigned(cmd, "wrap", &value)) {
			if (!value) {
				terr.Printf("specify a wrap value: 0-65535\n");
				return false;
			}
			uint32_t wrap_val = ::strtoul(value, nullptr, 0);
			if (wrap_val > 65535) {
				terr.Printf("wrap value out of range (0-65535): %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_wrap(static_cast<uint16_t>(wrap_val));
		} else if (Shell::Arg::GetAssigned(cmd, "level", &value)) {
			if (!value) {
				terr.Printf("specify a level value: 0-65535\n");
				return false;
			}
			uint32_t level_val = ::strtoul(value, nullptr, 0);
			if (level_val > 65535) {
				terr.Printf("level value out of range (0-65535): %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_chan_level(static_cast<uint16_t>(level_val));
		} else if (Shell::Arg::GetAssigned(cmd, "duty", &value)) {
			if (!value) {
				terr.Printf("specify a duty ratio: 0.0-1.0\n");
				return false;
			}
			float duty_val = ::strtof(value, nullptr);
			if (duty_val < 0.0f || duty_val > 1.0f) {
				terr.Printf("duty ratio out of range (0.0-1.0): %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_chan_duty(duty_val);
		} else if (Shell::Arg::GetAssigned(cmd, "clkdiv", &value)) {
			if (!value) {
				terr.Printf("specify a clock divider: 1.0-255.9\n");
				return false;
			}
			float div_val = ::strtof(value, nullptr);
			if (div_val < 1.0f || div_val > 255.9f) {
				terr.Printf("clock divider out of range (1.0-255.9): %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_clkdiv(div_val);
		} else if (Shell::Arg::GetAssigned(cmd, "phase-correct", &value)) {
			if (!value) {
				terr.Printf("specify a boolean: false, true, 0, 1\n");
				return false;
			}
			bool phaseCorrect;
			if (::strcasecmp(value, "1") == 0 || ::strcasecmp(value, "true") == 0) {
				phaseCorrect = true;
			} else if (::strcasecmp(value, "0") == 0 || ::strcasecmp(value, "false") == 0) {
				phaseCorrect = false;
			} else {
				terr.Printf("invalid value for phase-correct (false, true, 0, 1)\n");
				return false;
			}
			for (int i = 0; i < nPins; ++i) {
				PWM pwm(pinTbl[i]);
				uint32_t freq = pwm.get_freq();
				pwm.set_phase_correct(phaseCorrect);
				pwm.set_freq(freq); // Recalculate frequency after changing phase-correct
			}
		} else if (Shell::Arg::GetAssigned(cmd, "invert", &value)) {
			if (!value) {
				terr.Printf("specify a boolean: false, true, 0, 1\n");
				return false;
			}
			bool inv;
			if (::strcasecmp(value, "1") == 0 || ::strcasecmp(value, "true") == 0) {
				inv = true;
			} else if (::strcasecmp(value, "0") == 0 || ::strcasecmp(value, "false") == 0) {
				inv = false;
			} else {
				terr.Printf("invalid value for phase-correct (false, true, 0, 1)\n");
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_chan_output_polarity(inv);
		} else if (Shell::Arg::GetAssigned(cmd, "counter", &value)) {
			if (!value) {
				terr.Printf("specify a counter value: 0-65535\n");
				return false;
			}
			uint32_t counter_val = ::strtoul(value, nullptr, 0);
			if (counter_val > 65535) {
				terr.Printf("counter value out of range (0-65535): %s\n", value);
				return false;
			}
			for (int i = 0; i < nPins; ++i) PWM(pinTbl[i]).set_counter(static_cast<uint16_t>(counter_val));
		} else if (Shell::Arg::GetAssigned(cmd, "sleep", &value)) {
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				return false;
			}
			Tickable::Sleep(msec);
		} else {
			terr.Printf("unknown command: %s\n", cmd);
			terr.Printf("available commands: %s\n", strAvailableCommands);
			return false;
		}
		if (Tickable::TickSub()) return true;
	}
	if (!quietFlag) {
		for (int i = 0; i < nPins; ++i) PrintPWMStatus(tout, pinTbl[i], onlyPWMFlag, builtinLEDFlag);
	}
	return true;
}

void PrintPWMStatus(Printable& tout, uint pin, bool onlyPWMFlag, bool builtinLEDFlag)
{
	gpio_function_t pinFunc = ::gpio_get_function(pin);
	const char* funcName = GPIOInfo::GetFuncName(::gpio_get_function(pin), pin, "------");
	if (pinFunc == GPIO_FUNC_PWM) {
		PWM pwm(pin);
		tout.Printf("GPIO%-2u%sfunc:%-10s %-8s freq:%uHz (clkdiv:%.1f wrap:0x%04x) duty:%.3f (level:0x%04x)%s%s counter:0x%04x\n",
			pin, IsAccessiblePin(pin, builtinLEDFlag) ? " " : "*", funcName, pwm.is_enabled()? "enabled" : "disabled",
			pwm.get_freq(), pwm.get_clkdiv(), pwm.get_wrap(), pwm.get_chan_duty(), pwm.get_chan_level(),
			pwm.get_phase_correct()? " phase-correct" : "",
			pwm.get_chan_output_polarity() ? " inverted" : "",
			pwm.get_counter());
	} else if (!onlyPWMFlag) {
		tout.Printf("GPIO%-2u%sfunc:%s\n", pin, IsAccessiblePin(pin, builtinLEDFlag) ? " " : "*", funcName);
	}
}

}
