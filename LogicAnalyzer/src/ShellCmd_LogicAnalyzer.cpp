#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/LogicAnalyzer.h"

jxglib::LogicAnalyzer& ShellCmd_LogicAnalyzer_GetLogicAnalyzer();

namespace jxglib::ShellCmd_LogicAnalyzer {

ShellCmd(la, "Logic Analyzer")
{
	LogicAnalyzer& logicAnalyzer = ShellCmd_LogicAnalyzer_GetLogicAnalyzer();
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",			'h', "prints this help"),
		Arg::OptString("pio",			'P', (PIO::Num == 3)? "PIO to use (0-2)" : "PIO to use (0-1)", "PIO"),
		Arg::OptString("pins",			'p', "pins to monitor", "PINS"),
		Arg::OptString("target",		't', "target (internal, external)", "TARGET"),
		Arg::OptString("samplers",		'S', "number of samplers (1-4)", "NUM"),
		Arg::OptString("heap-ratio",	'R', "heap ratio to use as event buffer (0.0-1.0)", "RATIO"),
		Arg::OptString("reso",			'r', "resolution in microseconds (default 1000)", "RESO"),
		Arg::OptString("part",			't', "printed part of the waveform (head, tail, all)", "PART"),
		Arg::OptString("events",		'e', "number of events to print (default 80)", "NUM"),
		Arg::OptString("style",			's', "waveform style (unicode1, unicode2, unicode3, unicode4, ascii1, ascii2, ascii3, ascii4)", "STYLE"),
		Arg::OptString("event-format",	'f', "event format (auto, short, long)", "FORMAT"),
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
	if (arg.GetString("pio", &value)) {
		int iPIO = ::strtol(value, nullptr, 10);
		if (iPIO < 0 || iPIO >= PIO::Num) {
			terr.Printf("Invalid PIO number: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetPIO(iPIO);
	}
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
		if (::strcasecmp(value, "unicode1") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_unicode1;
		} else if (::strcasecmp(value, "unicode2") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_unicode2;
		} else if (::strcasecmp(value, "unicode3") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_unicode3;
		} else if (::strcasecmp(value, "unicode4") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_unicode4;
		} else if (::strcasecmp(value, "ascii1") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_ascii1;
		} else if (::strcasecmp(value, "ascii2") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_ascii2;
		} else if (::strcasecmp(value, "ascii3") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_ascii3;
		} else if (::strcasecmp(value, "ascii4") == 0) {
			pWaveStyle = &LogicAnalyzer::waveStyle_ascii4;
		} else {
			terr.Printf("Invalid waveform style: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetWaveStyle(*pWaveStyle);
	}
	if (arg.GetString("event-format", &value)) {
		LogicAnalyzer::RawEventFormat rawEventFormat = LogicAnalyzer::RawEventFormat::Auto;
		if (::strcasecmp(value, "auto") == 0) {
			rawEventFormat = LogicAnalyzer::RawEventFormat::Auto;
		} else if (::strcasecmp(value, "short") == 0) {
			rawEventFormat = LogicAnalyzer::RawEventFormat::Short;
		} else if (::strcasecmp(value, "long") == 0) {
			rawEventFormat = LogicAnalyzer::RawEventFormat::Long;
		} else {
			terr.Printf("Invalid event format: %s\n", value);
			return Result::Error;
		}
		logicAnalyzer.SetEventFormat(rawEventFormat);
	}
	if (argc < 2) {
		logicAnalyzer.UpdateSamplingInfo();
		logicAnalyzer.PrintSettings(tout);
		return Result::Success;
	}
	Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
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
			logicAnalyzer.PrintWave(tout, terr);
		} else if (::strcmp(subcmd, "plot") == 0) {
			logicAnalyzer.PlotWave();
		} else if (Arg::GetAssigned(subcmd, "decoder", &value)) {
			const char* decoderName = value;
			if (!decoderName) {
				terr.Printf("available decoders:");
				for (const ProtocolDecoder::Factory* pFactory = ProtocolDecoder::Factory::GetHead(); pFactory; pFactory = pFactory->GetNext()) {
					terr.Printf(" %s", pFactory->GetName());
				}
				terr.Println();
				return false;
			}
			ProtocolDecoder* pProtocolDecoder = logicAnalyzer.SetDecoder(decoderName);
			if (!pProtocolDecoder) {
				terr.Printf("unknown decoder: %s\n", decoderName);
				return false;
			}
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				if (!pProtocolDecoder->EvalSubcmd(terr, pSubcmdChild->GetProc())) return false;
			}
		} else {
			tout.Printf("unknown subcommand: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
