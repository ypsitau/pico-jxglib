//==============================================================================
// ShellCmd_Basic.cpp
//==============================================================================
#include <stdlib.h>
#include "pico/binary_info.h"
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
extern uint32_t __binary_info_start;
extern uint32_t __binary_info_end;
extern uint32_t __address_mapping_table;

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

class BinaryInfo {
public:
	static const char* GetString(uint32_t id);
	static void PrintGPIOUsage(Printable& tout);
	static void Print(Printable& tout);
	static const binary_info_t** GetInfoStart() { return reinterpret_cast<const binary_info_t**>(&__binary_info_start); }
	static const binary_info_t** GetInfoEnd() { return reinterpret_cast<const binary_info_t**>(&__binary_info_end); }
	static const char* GetPinFuncName(int func);
};

const char* BinaryInfo::GetString(uint32_t id)
{
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		if (pInfo->type == BINARY_INFO_TYPE_ID_AND_STRING) {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			if (info.id == id) return info.value;
		}
	}
	return "";
}

void BinaryInfo::PrintGPIOUsage(Printable& tout)
{
	bool firstInfoFlag = true;
	for (auto ppInfo = GetInfoEnd(); ppInfo != GetInfoStart(); --ppInfo) {
		const binary_info_t* pInfo = *(ppInfo - 1);
		switch (pInfo->type) {
		case BINARY_INFO_TYPE_PINS_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_func_t*>(pInfo);
			uint32_t pin_encoding = info.pin_encoding;
			uint32_t encodingFormat = pin_encoding & 0b111;
			pin_encoding >>= 3;
			uint32_t pinFunc = pin_encoding & 0b1111;
			pin_encoding >>= 4;
			switch (encodingFormat) {
			case BI_PINS_ENCODING_RANGE: {
				uint32_t pinLow = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				uint32_t pinHigh = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				if (firstInfoFlag) {
					tout.Println();
					firstInfoFlag = false;
				}
				tout.Printf("GPIO %d-%d: %s\n", pinLow, pinHigh, GetPinFuncName(pinFunc));
				break;
			}
			case BI_PINS_ENCODING_MULTI: {
				uint32_t pinPrev = -1;
				if (firstInfoFlag) {
					tout.Println();
					firstInfoFlag = false;
				}
				tout.Printf("GPIO ");
				for (int i = 0; i < 5; ++i) {
					uint32_t pin = pin_encoding & 0b11111;
					pin_encoding >>= 5;
					if (pin == pinPrev) break;
					tout.Printf("%s%d", (i == 0)? "" : ", ", pin);
					pinPrev = pin;
				}
				tout.Printf(": %s\n", GetPinFuncName(pinFunc));
				break;
			}
			default:
				break;
			}
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_name_t*>(pInfo);
			uint32_t pin_mask = info.pin_mask;
			bool firstFlag = true;
			if (firstInfoFlag) {
				tout.Println();
				firstInfoFlag = false;
			}
			tout.Printf("GPIO ");
			for (int pin = 0; pin_mask; pin++, pin_mask >>= 1) {
				if (pin_mask & 1) {
					tout.Printf("%s%d", firstFlag? "" : ", ", pin);
					firstFlag = false;
				}
			}
			tout.Printf(": %s\n", info.label);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_func_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_name_t*>(pInfo);
			break;
		}
		default:
			break;
		}
	}
}

void BinaryInfo::Print(Printable& tout)
{
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		tout.Printf("%04x %04x\n", pInfo->tag, pInfo->type);
		switch (pInfo->type) {
		case BINARY_INFO_TYPE_RAW_DATA: {
			auto& info = *reinterpret_cast<const binary_info_raw_data_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_SIZED_DATA: {
			auto& info = *reinterpret_cast<const binary_info_sized_data_t*>(pInfo);
			tout.Printf("Sized data %d bytes\n", info.length);
			tout.Dump(info.bytes, info.length);
			break;
		}
		case BINARY_INFO_TYPE_BINARY_INFO_LIST_ZERO_TERMINATED: {
			auto& info = *reinterpret_cast<const binary_info_list_zero_terminated_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_BSON: {
			auto& info = *reinterpret_cast<const binary_info_sized_data_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_ID_AND_INT: {
			auto& info = *reinterpret_cast<const binary_info_id_and_int_t*>(pInfo);
			tout.Printf("%08x: %08x\n", info.id, info.value);
			break;
		}
		case BINARY_INFO_TYPE_ID_AND_STRING: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			tout.Printf("%08x: %s\n", info.id, info.value);
			break;
		}
		case BINARY_INFO_TYPE_BLOCK_DEVICE: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_func_t*>(pInfo);
			uint32_t pin_encoding = info.pin_encoding;
			uint32_t encodingFormat = pin_encoding & 0b111;
			pin_encoding >>= 3;
			uint32_t pinFunc = pin_encoding & 0b1111;
			pin_encoding >>= 4;
			switch (encodingFormat) {
			case BI_PINS_ENCODING_RANGE: {
				uint32_t pinLow = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				uint32_t pinHigh = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				tout.Printf("Pins %d-%d: %08x\n", pinLow, pinHigh, pinFunc);
				break;
			}
			case BI_PINS_ENCODING_MULTI: {
				tout.Printf("Pins");
				uint32_t pinPrev = -1;
				for (int i = 0; i < 5; ++i) {
					uint32_t pin = pin_encoding & 0b11111;
					pin_encoding >>= 5;
					if (pin == pinPrev) break;
					tout.Printf(" %d", pin);
					pinPrev = pin;
				}
				tout.Printf(": %08x\n", pinFunc);
				break;
			}
			default:
				break;
			}
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_name_t*>(pInfo);
			uint32_t pin_mask = info.pin_mask;
			tout.Printf("Pins");
			for (int pin = 0; pin_mask; pin++, pin_mask >>= 1) {
				if (pin_mask & 1) tout.Printf(" %d", pin);
			}
			tout.Printf(": %s\n", info.label);
			break;
		}
		case BINARY_INFO_TYPE_NAMED_GROUP: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PTR_INT32_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_ptr_int32_with_name_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PTR_STRING_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_ptr_string_with_name_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_func_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_name_t*>(pInfo);
			break;
		}
		default:
			break;
		}
	}
}

const char* BinaryInfo::GetPinFuncName(int pinFunc)
{
#if defined(PICO_RP2040)
	switch (pinFunc) {
	case GPIO_FUNC_XIP:		return "XIP"; 
	case GPIO_FUNC_SPI:		return "SPI"; 
	case GPIO_FUNC_UART:	return "UART"; 
	case GPIO_FUNC_I2C:		return "I2C"; 
	case GPIO_FUNC_PWM:		return "PWM"; 
	case GPIO_FUNC_SIO:		return "SIO"; 
	case GPIO_FUNC_PIO0:	return "PIO0"; 
	case GPIO_FUNC_PIO1:	return "PIO1"; 
	case GPIO_FUNC_GPCK:	return "GPCK"; 
	case GPIO_FUNC_USB:		return "USB"; 
	default:				return "unknown";
	}
#elif defined(PICO_RP2350)
	switch (pinFunc) {
	case GPIO_FUNC_HSTX:	return "HSTX";
	case GPIO_FUNC_SPI:		return "SPI";
	case GPIO_FUNC_UART:	return "UART";
	case GPIO_FUNC_I2C:		return "I2C";
	case GPIO_FUNC_PWM:		return "PWM";
	case GPIO_FUNC_SIO:		return "SIO";
	case GPIO_FUNC_PIO0:	return "PIO0";
	case GPIO_FUNC_PIO1:	return "PIO1";
	case GPIO_FUNC_PIO2:	return "PIO2";
	case GPIO_FUNC_GPCK:	return "GPCK";
	//case GPIO_FUNC_XIP_CS1:	return "XIP_CS1";
	//case GPIO_FUNC_CORESIGHT_TRACE:	return "CORESIGHT_TRACE";
	case GPIO_FUNC_USB:		return "USB";
	case GPIO_FUNC_UART_AUX:return "UART_AUX";
	default:				return "unknown";
	}
#else
	return "unknown"
#endif
}

//-----------------------------------------------------------------------------
// about-me
//-----------------------------------------------------------------------------
ShellCmd_Named(about_me, "about-me", "prints information about this own program")
{
	tout.Printf("Name        %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_NAME));
	tout.Printf("Version     %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_VERSION_STRING));
	tout.Printf("Date        %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_BUILD_DATE_STRING));
	//tout.Printf("binary end      %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_BINARY_END));
	tout.Printf("URL         %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_URL));
	tout.Printf("Description %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_DESCRIPTION));
	tout.Printf("Feature     %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_FEATURE));
	tout.Printf("Attribute   %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PROGRAM_BUILD_ATTRIBUTE));
	tout.Printf("SDK version %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_SDK_VERSION));
	tout.Printf("Pico board  %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_PICO_BOARD));
	tout.Printf("Boot2       %s\n", BinaryInfo::GetString(BINARY_INFO_ID_RP_BOOT2_NAME));
	BinaryInfo::PrintGPIOUsage(tout);
	tout.Println();
	tout.Printf("Flash  0x%p-0x%p %7d\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if defined(__arm__)
	tout.Printf("Vector 0x%p-0x%p %7d\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	tout.Printf("Data   0x%p-0x%p %7d\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	tout.Printf("Bss    0x%p-0x%p %7d\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	tout.Printf("Heap   0x%p-0x%p %7d\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	tout.Printf("Stack  0x%p-0x%p %7d\n", &__heap_end, &__stack, &__stack - &__heap_end);
	//BinaryInfo::Print(tout);
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
