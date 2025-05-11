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
ShellCmd_Alias(about_cpu, "about-platform", "prints information about the platform")
{
#if defined(PICO_RP2040)
	out.Printf("RP2040 %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
#elif defined(PICO_RP2350)
	out.Printf("RP2350 (%s) %d MHz\n", CPU_ARCH, ::clock_get_hz(clk_sys) / 1000000);
#else
	out.Printf("unknown %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
#endif
	out.Printf("Flash  0x%08X-0x%08X %7d\n", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES, PICO_FLASH_SIZE_BYTES); 
	out.Printf("SRAM   0x20000000-0x%p %7d\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
	return 0;
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Alias(about_me, "about-me", "prints information about this own program")
{
#if defined(PICO_PROGRAM_NAME)
	out.Printf("%s", PICO_PROGRAM_NAME);
#else
	out.Printf("(no name)");
#endif
#if defined(PICO_PROGRAM_VERSION_STRING)
	out.Printf(" ver.%s\n", PICO_PROGRAM_VERSION_STRING);
#else
	out.Printf("\n");
#endif
#if defined(PICO_PROGRAM_DESCRIPTION)
	out.Printf("Desc   %s\n", PICO_PROGRAM_DESCRIPTION);
#endif
#if defined(PICO_PROGRAM_URL)
	out.Printf("URL    %s\n", PICO_PROGRAM_URL);
#endif
	out.Printf("Flash  0x%p-0x%p %7d\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if defined(__arm__)
	out.Printf("Vector 0x%p-0x%p %7d\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	out.Printf("Data   0x%p-0x%p %7d\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	out.Printf("Bss    0x%p-0x%p %7d\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	out.Printf("Heap   0x%p-0x%p %7d\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	out.Printf("Stack  0x%p-0x%p %7d\n", &__heap_end, &__stack, &__stack - &__heap_end);
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
	virtual int Run(Printable& out, Printable& err, int argc, char* argv[]) override;
};

ShellCmd_d ShellCmd_d::Instance;

int ShellCmd_d::Run(Printable& out, Printable& err, int argc, char* argv[])
{
	int nColsOut, nRowsOut;
	out.GetSize(&nColsOut, &nRowsOut);
	nColsOut -= 8 + 2;
	int nCols = ((nColsOut + 1) / 3) / 8 * 8;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			err.Printf("invalid number\n");
			return 1;
		}
		addr_ = num;
	}
	if (argc >= 3) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[2], &p, 0);
		if (*p != '\0') {
			err.Printf("invalid number\n");
			return 1;
		}
		bytes_ = num;
	}
	if (nCols > 0) {
		out.Dump.Cols(nCols).AddrStart(addr_).DigitsAddr(8)(reinterpret_cast<const void*>(addr_), bytes_);
	}
	addr_ += bytes_;
	return 0;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	Tickable::PrintList(out);
	return 0;
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	Shell::PrintHelp(out);
	return 0;
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	if (argc < 2) {
		out.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
	return 0;
}

}
