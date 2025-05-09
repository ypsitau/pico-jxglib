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
	terminal.Printf("RP2040 %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
	terminal.Printf("Flash  0x10000000 - 0x10200000 %8d bytes\n", 0x10200000 - 0x10000000); 
	terminal.Printf("SRAM   0x20000000 - 0x%p %8d bytes\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
#elif defined(PICO_RP2350)
	terminal.Printf("RP2350 (%s) %d MHz\n", CPU_ARCH, ::clock_get_hz(clk_sys) / 1000000);
	terminal.Printf("Flash  0x10000000 - 0x10400000 %8d bytes\n", 0x10400000 - 0x10000000); 
	terminal.Printf("SRAM   0x20000000 - 0x%p %8d bytes\n", &__stack, &__stack - reinterpret_cast<char*>(0x20000000));
#else
	terminal.Printf("unknown %d MHz\n", ::clock_get_hz(clk_sys) / 1000000);
#endif
	return 0;
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Alias(about_me, "about-me", "prints information about this own program")
{
#if defined(PICO_PROGRAM_NAME)
	terminal.Printf("%s", PICO_PROGRAM_NAME);
#else
	terminal.Printf("(no name)");
#endif
#if defined(PICO_PROGRAM_VERSION_STRING)
	terminal.Printf(" ver.%s\n", PICO_PROGRAM_VERSION_STRING);
#else
	terminal.Printf("\n");
#endif
#if defined(PICO_PROGRAM_DESCRIPTION)
	terminal.Printf("Desc   %s\n", PICO_PROGRAM_DESCRIPTION);
#endif
#if defined(PICO_PROGRAM_URL)
	terminal.Printf("URL    %s\n", PICO_PROGRAM_URL);
#endif
	terminal.Printf("Flash  0x%p - 0x%p %8d bytes\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if defined(__arm__)
	terminal.Printf("Vector 0x%p - 0x%p %8d bytes\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	terminal.Printf("Data   0x%p - 0x%p %8d bytes\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	terminal.Printf("Bss    0x%p - 0x%p %8d bytes\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	terminal.Printf("Heap   0x%p - 0x%p %8d bytes\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	terminal.Printf("Stack  0x%p - 0x%p %8d bytes\n", &__heap_end, &__stack, &__stack - &__heap_end);
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
	virtual int Run(Terminal& terminal, int argc, char* argv[]) override;
};

ShellCmd_d ShellCmd_d::Instance;

int ShellCmd_d::Run(Terminal& terminal, int argc, char* argv[])
{
	int nColsTerm, nRowsTerm;
	terminal.GetSize(&nColsTerm, &nRowsTerm);
	nColsTerm -= 8 + 2;
	int nCols = ((nColsTerm + 1) / 3) / 8 * 8;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terminal.Printf("invalid number\n");
			return 1;
		}
		addr_ = num;
	}
	if (argc >= 3) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[2], &p, 0);
		if (*p != '\0') {
			terminal.Printf("invalid number\n");
			return 1;
		}
		bytes_ = num;
	}
	if (nCols > 0) {
		terminal.Dump.Cols(nCols).AddrStart(addr_).DigitsAddr(8)(reinterpret_cast<const void*>(addr_), bytes_);
	}
	addr_ += bytes_;
	return 0;
}

//-----------------------------------------------------------------------------
// ticks
//-----------------------------------------------------------------------------
ShellCmd(ticks, "prints names and attributes of running Tickable instances")
{
	Tickable::PrintList(terminal);
	return 0;
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
ShellCmd(help, "prints help strings for available commands")
{
	Shell::PrintHelp(terminal);
	return 0;
}

//-----------------------------------------------------------------------------
// prompt
//-----------------------------------------------------------------------------
ShellCmd(prompt, "changes the command line prompt")
{
	if (argc < 2) {
		terminal.Println(Shell::GetPrompt());
	} else {
		Shell::SetPrompt(argv[1]);
	}
	return 0;
}

}
