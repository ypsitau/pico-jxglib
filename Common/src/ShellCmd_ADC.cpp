#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "jxglib/Shell.h"

using namespace jxglib;

static bool ProcessADC(Printable& terr, Printable& tout, Shell::Arg::EachNum& eachNum, int argc, char* argv[]);
static void PrintADC(Printable& tout, Shell::Arg::EachNum& eachNum);

static const char* strAvailableInputs = "0, 1, 2, 3, 4 (temp sensor)";

ShellCmd(adc, "controls ADC (Analog-to-Digital Converter)")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
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
		tout.Printf("Commands:\n");
		tout.Printf("  read           read ADC value (raw 12-bit value)\n");
		tout.Printf("  voltage        read ADC voltage (converted to volts)\n");
		tout.Printf("  temp           read temperature (for input 4 only)\n");
		tout.Printf("  continuous     continuously read ADC values (Ctrl+C to stop)\n");
		tout.Printf("  sleep=MSEC     sleep for specified milliseconds\n");
		tout.Printf("Available inputs: %s\n", strAvailableInputs);
		return Result::Success;
	}
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
	if (!ProcessADC(terr, tout, eachNum, argc - nArgsToSkip, argv + nArgsToSkip)) return Result::Error;
	return Result::Success;
}

ShellCmdAlias(adc0, adc)
ShellCmdAlias(adc1, adc)
ShellCmdAlias(adc2, adc)
ShellCmdAlias(adc3, adc)
ShellCmdAlias(adc4, adc)

bool ProcessADC(Printable& terr, Printable& tout, Shell::Arg::EachNum& eachNum, int argc, char* argv[])
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
			eachNum.Rewind();
			for (int num = 0; eachNum.Next(&num); ) {
				uint input = static_cast<uint>(num);
				::adc_gpio_init(26 + input);
			}
        } else if (::strcasecmp(subcmd, "read") == 0) {
			PrintADC(tout, eachNum);
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
	if (!readFlag) PrintADC(tout, eachNum);
	return true;
}

void PrintADC(Printable& tout, Shell::Arg::EachNum& eachNum)
{
	tout.Printf("ADC ");
	eachNum.Rewind();
	bool firstFlag = true;
	for (int num = 0; eachNum.Next(&num); ) {
		uint input = static_cast<uint>(num);
		if (!firstFlag) tout.Printf(",");
		firstFlag = false;
		tout.Printf("%d", input);
	}
	tout.Printf(": ");
	eachNum.Rewind();
	firstFlag = true;
	for (int num = 0; eachNum.Next(&num); ) {
		uint input = static_cast<uint>(num);
		if (!firstFlag) tout.Printf(" ");
		firstFlag = false;
		::adc_select_input(input);
		uint16_t result = ::adc_read();
		float voltage = result * 3.3f / 4095.0f;
		if (input == 4) {
			float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
			tout.Printf("%.1f°C", tempC);
		} else {
			tout.Printf("%.3fV", result * 3.3f / 4095.0f);
		}
	}
	tout.Printf("\n");
}
