#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_ADC {

static bool ProcessADC(Printable& terr, Printable& tout, const int inputTbl[], int nInputs,
					int argc, char* argv[], bool rawFlag);
static void PrintADC(Printable& tout, const int inputTbl[], int nInputs, bool labelFlag, bool rawFlag);

static const char* strAvailableInputs = "0, 1, 2, 3, 4 (temp sensor)";

ShellCmd(adc, "controls ADC (Analog-to-Digital Converter)")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptBool("raw",		'r',	"prints raw ADC values (12-bit)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcmp(GetName(), "adc") == 0);
	if (arg.GetBool("help")) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [INPUT [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  init                 Prepare a GPIO for use with ADC by disabling all digital functions\n");
		tout.Printf("  read                 read ADC value\n");
		tout.Printf("Available inputs: %s\n", strAvailableInputs);
		return Result::Success;
	}
	bool rawFlag = arg.GetBool("raw");
	const char* strIndex = nullptr;
	int nArgsToSkip = 0;
	if (genericFlag) {
		if (argc < 2) {
			terr.Printf("ADC input number is required\n");
			return Result::Error;
		}
		strIndex = argv[1];
		nArgsToSkip = 2; // Skip "adc" and input index
	} else if (::strncmp(GetName(), "adc", 3) == 0) {
		strIndex = GetName() + 3; // Skip "adc" prefix
		nArgsToSkip = 1; // Skip "adc" command
	}
	if (!strIndex) return Result::Error;
	Arg::EachNum eachNum(strIndex, 4);
	if (!eachNum.CheckValidity()) {
		terr.Printf("invalid ADC input number: %s\n", strIndex);
		return Result::Error;
	}
	int inputTbl[8];
	int nInputs = eachNum.GetAll(inputTbl, count_of(inputTbl));
	for (int i = 0; i < nInputs; ++i) {
		int input = inputTbl[i];
		if (input < 0 || input >= 5) {
			terr.Printf("invalid ADC input number: %d\n", input);
			return Result::Error;
		}
	}
	return ProcessADC(terr, tout, inputTbl, nInputs,
		argc - nArgsToSkip, argv + nArgsToSkip, rawFlag)? Result::Success : Result::Error;
}

ShellCmdAlias(adc0, adc)
ShellCmdAlias(adc1, adc)
ShellCmdAlias(adc2, adc)
ShellCmdAlias(adc3, adc)
ShellCmdAlias(adc4, adc)

bool ProcessADC(Printable& terr, Printable& tout, const int inputTbl[], int nInputs,
				int argc, char* argv[], bool rawFlag)
{
	Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return false;
	}
	bool readFlag = false;
	while (const char* subcmd = each.Next()) {
		const char* value = nullptr;
		if (::strcasecmp(subcmd, "init") == 0) {
			for (int i = 0; i < nInputs; i++) {
				uint input = inputTbl[i];
				::adc_gpio_init(26 + input);
			}
        } else if (::strcasecmp(subcmd, "read") == 0) {
			PrintADC(tout, inputTbl, nInputs, false, rawFlag);
			readFlag = true;
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
	if (!readFlag) PrintADC(tout, inputTbl, nInputs, true, rawFlag);
	return true;
}

void PrintADC(Printable& tout, const int inputTbl[], int nInputs, bool labelFlag, bool rawFlag)
{
	if (labelFlag) {
		tout.Printf("ADC");
		for (int i = 0; i < nInputs; i++) {
			uint input = inputTbl[i];
			if (i > 0) tout.Printf(",");
			tout.Printf("%d", input);
		}
		tout.Printf(" ");
	}
	for (int i = 0; i < nInputs; i++) {
		uint input = inputTbl[i];
		if (i > 0) tout.Printf(" ");
		::adc_select_input(input);
		uint16_t result = ::adc_read();
		if (rawFlag) {
			tout.Printf("0x%03x", result);
		} else {
			float voltage = result * 3.3f / 4095.0f;
			if (input == 4) {
				float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
				tout.Printf("%.1f°C", tempC);
			} else {
				tout.Printf("%.3fV", result * 3.3f / 4095.0f);
			}
		}
	}
	tout.Println();
}

}
