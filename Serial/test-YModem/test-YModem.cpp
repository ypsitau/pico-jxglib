#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/UART.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/RTC/DS323x.h"

using namespace jxglib;

class YModem {
private:
	Stream& stream_;
public:
	YModem(Stream& stream) : stream_(stream) {}
public:
	bool SendFile(const char* fileName);
	bool RecvFile(const char* fileName);
};

bool YModem::SendFile(const char* fileName)
{

}

bool YModem::RecvFile(const char* fileName)
{

}

ShellCmd(ysend, "Send file via YModem protocol")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... FILE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	Stream& stream = UART0;
	YModem ymodem(stream);
	for (Arg::Each each(argv[1], argv[argc]); const char* fileName = each.Next(); ) {
		ymodem.SendFile(fileName);
		tout.Printf("Sending file: %s\n", fileName);
	}
	return 0;
}

int main()
{
	::stdio_init_all();
	LFS::Flash driveA("A:",  0x1010'0000, 0x0004'0000); // Flash address and size 256kB
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	::i2c_init(i2c0, 400'000);
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	RTC::DS323x rtc(i2c0);
	for (;;) {
		// :
		// any other jobs
		// :
		Tickable::Tick();
	}
}
