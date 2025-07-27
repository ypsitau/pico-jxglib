#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/LogicAnalyzer.h"

namespace jxglib::ShellCmd_LogicAnalyzer {

LogicAnalyzer logicAnalyzer;

ShellCmd(la, "Logic Analyzer")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",			'h', "prints this help"),
		Arg::OptString("pins",			'p', "pins to monitor", "PINS"),
		Arg::OptString("target",		't', "target (internal, external)", "TARGET"),
		Arg::OptString("samplers",		'S', "number of samplers (1-4)", "NUM"),
		Arg::OptString("heap-ratio",	'R', "heap ratio to use as event buffer (0.0-1.0)", "RATIO"),
		Arg::OptString("reso",			'r', "resolution in microseconds (default 1000)", "RESO"),
		Arg::OptString("part",			't', "printed part of the waveform (head, tail, all)", "PART"),
		Arg::OptString("events",		'e', "number of events to print (default 80)", "NUM"),
		Arg::OptString("style",			's', "waveform style (fancy1, fancy2, fancy3, fancy4, simple1, simple2, simple3, simple4)", "STYLE"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "pwm") == 0);
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  enable               enable sampling of the logic analyzer\n");
		tout.Printf("  disable              disable sampling of the logic analyzer\n");
		tout.Printf("  print                print the sampled waveforms\n");
		return Result::Success;
	}
	const char* value;
	if (arg.GetString("pins", &value)) {
		int pinTbl[GPIO::NumPins * 2];
		Arg::EachNum eachNum(value, GPIO::NumPins - 1);
		eachNum.SetBlankValue(-1);
		int nPins = eachNum.GetAll(pinTbl, count_of(pinTbl));
		if (nPins <= 0) {
			terr.Printf("invalid GPIO pin number: %s\n", value);
			return Result::Error;
		}
		for (int i = 0; i < nPins; i++) {
			int pin = pinTbl[i];
			if (pin < -1 || pin >= GPIO::NumPins) {	// -1 is allowed for "blank" value
				terr.Printf("invalid GPIO pin number: %d\n", pin);
				return Result::Error;
			}
		}
		logicAnalyzer.SetPins(pinTbl, nPins);
	}
	if (arg.GetString("target", &value)) {
		if (::strcasecmp(value, "internal") == 0) {
			logicAnalyzer.SetTarget(LogicAnalyzer::Target::Internal);
		} else if (::strcasecmp(value, "external") == 0) {
			logicAnalyzer.SetTarget(LogicAnalyzer::Target::External);
		} else {
			terr.Printf("Invalid target: %s\n", value);
			return Result::Error;
		}
	}
	if (arg.GetString("samplers", &value)) {
		char* endptr = nullptr;
		int nSampler = ::strtol(value, &endptr, 10);
		if (endptr == value || *endptr != '\0' || nSampler < 1 || nSampler > 4) {
			terr.Printf("Invalid sampler count: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetSamplerCount(nSampler);
	}
	if (arg.GetString("heap-ratio", &value)) {
		char* endptr = nullptr;
		float heapRatio = ::strtof(value, &endptr);
		if (endptr == value || *endptr != '\0' || heapRatio <= 0.0f || heapRatio > 1.0f) {
			terr.Printf("Invalid heap ratio: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetHeapRatio(heapRatio);
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
	if (arg.GetString("part", &value)) {
		if (::strcmp(value, "head") == 0) {
			logicAnalyzer.SetPrintPart(LogicAnalyzer::PrintPart::Head);
		} else if (::strcmp(value, "tail") == 0) {
			logicAnalyzer.SetPrintPart(LogicAnalyzer::PrintPart::Tail);
		} else if (::strcmp(value, "all") == 0) {
			logicAnalyzer.SetPrintPart(LogicAnalyzer::PrintPart::All);
		} else {
			terr.Printf("Invalid print position: %s\n", value);
			return Result::Error;
		}
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
	if (arg.GetString("style", &value)) {
		const LogicAnalyzer::WaveStyle* pWaveStyle = nullptr;
		if (::strcasecmp(value, "fancy1") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_fancy1;
		} else if (::strcasecmp(value, "fancy2") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_fancy2;
		} else if (::strcasecmp(value, "fancy3") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_fancy3;
		} else if (::strcasecmp(value, "fancy4") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_fancy4;
		} else if (::strcasecmp(value, "simple1") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_simple1;
		} else if (::strcasecmp(value, "simple2") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_simple2;
		} else if (::strcasecmp(value, "simple3") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_simple3;
		} else if (::strcasecmp(value, "simple4") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_simple4;
		} else {
			terr.Printf("Invalid waveform style: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetWaveStyle(*pWaveStyle);
	}
	if (argc < 2) {
		logicAnalyzer.UpdateSamplingInfo();
		logicAnalyzer.PrintSettings(tout);
		return Result::Success;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const char* subcmd = each.Next()) {
		if (::strcasecmp(subcmd, "enable") == 0) {
			logicAnalyzer.UpdateSamplingInfo();
			if (!logicAnalyzer.HasEnabledPins()) {
				terr.Printf("use -p option to specify pins to sample\n");
			} else if (logicAnalyzer.Enable()) {
				logicAnalyzer.PrintSettings(tout);
			} else {
				terr.Printf("failed to enable logic analyzer. reduce the number of heap-ratio\n");
			}
		} else if (::strcmp(subcmd, "disable") == 0) {
			logicAnalyzer.Disable();
			logicAnalyzer.PrintSettings(tout);
		} else if (::strcmp(subcmd, "print") == 0) {
			logicAnalyzer.PrintWave(tout);
		} else {
			tout.Printf("unknown command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
