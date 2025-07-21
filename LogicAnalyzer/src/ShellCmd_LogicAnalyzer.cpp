#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/LogicAnalyzer.h"

namespace jxglib::ShellCmd_LogicAnalyzer {

LogicAnalyzer logicAnalyzer;

ShellCmd(la, "Logic Analyzer")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h', "prints this help"),
		Arg::OptString("pins",		'p', "pins to monitor", "PINS"),
		Arg::OptString("reso",		'r', "resolution in microseconds (default 1000)", "RESO"),
		Arg::OptString("events",	'e', "number of events to print (default 80)", "NUM"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "pwm") == 0);
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		return Result::Success;
	}
	const char* value;
	if (arg.GetString("pins", &value)) {
		Arg::EachNum eachNum(value, GPIO::NumPins - 1);
		if (!eachNum.CheckValidity()) {
			terr.Printf("invalid GPIO pin number: %s\n", value);
			return Result::Error;
		}
		uint pinMin = GPIO::NumPins;
		uint32_t pinBitmap = 0;
		int pin;
		while (eachNum.Next(&pin)) {
			if (pin < 0 || pin >= GPIO::NumPins) {
				terr.Printf("invalid GPIO pin number: %d\n", pin);
				return Result::Error;
			}
			pinBitmap |= (1 << pin);
			if (pinMin > pin) pinMin = pin;
		}
		pinBitmap >>= pinMin;
		logicAnalyzer.SetPins(pinBitmap, pinMin);
	}
	if (arg.GetString("reso", &value)) {
		char* endptr = nullptr;
		float usecReso = ::strtod(value, &endptr);
		if (endptr == value || *endptr != '\0' || usecReso <= 0) {
			terr.Printf("Invalid resolution: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetResolution(usecReso);
	}
	if (arg.GetString("events", &value)) {
		char* endptr = nullptr;
		int nEventsToPrint = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || nEventsToPrint <= 0) {
			terr.Printf("Invalid resolution: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetEventCountToPrint(nEventsToPrint);
	}
	if (argc < 2) {
		logicAnalyzer.PrintSettings(tout);
		return Result::Success;
	}
	const char* subCmd = argv[1];
	if (::strcasecmp(subCmd, "start") == 0) {
		logicAnalyzer.Start();
		tout.Println("Logic Analyzer started.");
	} else if (::strcasecmp(subCmd, "restart") == 0) {
		logicAnalyzer.Restart();
		tout.Println("Logic Analyzer restarted.");
	} else if (::strcmp(subCmd, "stop") == 0) {
		logicAnalyzer.Stop();
		tout.Println("Logic Analyzer stopped.");
	} else if (::strcmp(subCmd, "wave") == 0) {
		logicAnalyzer.PrintWave(tout);
	} else {
		tout.Printf("Unknown command: %s\n", subCmd);
		return Result::Error;
	}
	return Result::Success;
}

}
