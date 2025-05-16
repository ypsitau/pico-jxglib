//==============================================================================
// jxglib/BinaryInfo.h
//==============================================================================
#ifndef PICO_JXGLIB_BINARYINFO_H
#define PICO_JXGLIB_BINARYINFO_H
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/Common.h"

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

namespace jxglib {

//-----------------------------------------------------------------------------
// BinaryInfo
//-----------------------------------------------------------------------------
class BinaryInfo {
public:
    static void PrintProgramInformation(Printable& tout);
    static void PrintBuildInformation(Printable& tout);
	static void PrintFixedPinInformation(Printable& tout);
	static void PrintMemoryMap(Printable& tout);
	static void Print(Printable& tout);
private:
	static bool GetInt(uint32_t id, uint32_t *pValue);
	static bool GetString(uint32_t id, const char** pValue);
	static const binary_info_t** GetInfoStart() { return reinterpret_cast<const binary_info_t**>(&__binary_info_start); }
	static const binary_info_t** GetInfoEnd() { return reinterpret_cast<const binary_info_t**>(&__binary_info_end); }
	static const char* GetPinFuncName(int func);
	static const char* GetPinFuncName(int func, uint pin);
};

}

#endif
