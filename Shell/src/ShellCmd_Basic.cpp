//==============================================================================
// ShellCmd_Basic.cpp
//==============================================================================
#include <stdlib.h>
#include "hardware/clocks.h"
#include "jxglib/Shell.h"

#if defined(__riscv)
    #define CPU_ARCH "RISC-V"
#elif defined(__arm__)
    #define CPU_ARCH "ARM"
#else
    #define CPU_ARCH "Unknown"
#endif

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __heap_start;
extern char __heap_end;
extern char __stack;
#if defined(__arm__)
extern uint32_t ram_vector_table;
#endif
namespace jxglib::ShellCmd_Basic {

//-----------------------------------------------------------------------------
// about-platform
//-----------------------------------------------------------------------------
ShellCmd_Named(about_cpu, "about-platform", "prints information about the platform")
{
#if defined(PICO_RP2040)
	tout.Printf("RP2040 %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
#elif defined(PICO_RP2350)
	tout.Printf("RP2350 (%s) %d MHz\n", CPU_ARCH, ::clock_get_hz(clk_sys) / 1000000);
#else
	tout.Printf("unknown %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
#endif
	tout.Printf("Flash  0x%08X-0x%08X %7d\n", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES, PICO_FLASH_SIZE_BYTES); 
	tout.Printf("SRAM   0x20000000-0x%p %7d\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
	return 0;
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Named(about_me, "about-me", "prints information about this own program")
{
#if defined(PICO_PROGRAM_NAME)
	tout.Printf("%s", PICO_PROGRAM_NAME);
#else
	tout.Printf("(no name)");
#endif
#if defined(PICO_PROGRAM_VERSION_STRING)
	tout.Printf(" ver.%s\n", PICO_PROGRAM_VERSION_STRING);
#else
	tout.Printf("\n");
#endif
#if defined(PICO_PROGRAM_DESCRIPTION)
	tout.Printf("Desc   %s\n", PICO_PROGRAM_DESCRIPTION);
#endif
#if defined(PICO_PROGRAM_URL)
	tout.Printf("URL    %s\n", PICO_PROGRAM_URL);
#endif
	tout.Printf("Flash  0x%p-0x%p %7d\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if defined(__arm__)
	tout.Printf("Vector 0x%p-0x%p %7d\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	tout.Printf("Data   0x%p-0x%p %7d\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	tout.Printf("Bss    0x%p-0x%p %7d\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	tout.Printf("Heap   0x%p-0x%p %7d\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	tout.Printf("Stack  0x%p-0x%p %7d\n", &__heap_end, &__stack, &__stack - &__heap_end);
	return 0;
}

//-----------------------------------------------------------------------------
// d addr bytes
//-----------------------------------------------------------------------------
class ShellCmd_d : public Shell::Cmd {
private:
	uint32_t addr_;
	uint32_t bytes_;
public:
	static ShellCmd_d Instance;
public:
	ShellCmd_d() : Shell::Cmd("d", "prints memory content at the specified address"), addr_{0x00000000}, bytes_{64} {}
public:
	virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) override;
};

ShellCmd_d ShellCmd_d::Instance;

int ShellCmd_d::Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[])
{
	int nColsOut, nRowsOut;
	tout.GetSize(&nColsOut, &nRowsOut);
	nColsOut -= 8 + 2;
	int nCols = ((nColsOut + 1) / 3) / 8 * 8;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		addr_ = num;
	}
	if (argc >= 3) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[2], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		bytes_ = num;
	}
	if (nCols > 0) {
		tout.Dump.Cols(nCols).AddrStart(addr_).DigitsAddr(8)(reinterpret_cast<const void*>(addr_), bytes_);
	}
	addr_ += bytes_;
	return 0;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	Tickable::PrintList(tout);
	return 0;
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	Shell::PrintHelp(tout);
	return 0;
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	if (argc < 2) {
		tout.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
	return 0;
}

}
