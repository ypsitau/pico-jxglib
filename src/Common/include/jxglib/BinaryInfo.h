//==============================================================================
// jxglib/BinaryInfo.h
//==============================================================================
#ifndef PICO_JXGLIB_BINARYINFO_H
#define PICO_JXGLIB_BINARYINFO_H
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/GPIOInfo.h"

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

namespace jxglib::BinaryInfo {

void PrintProgramInformation(Printable& tout);
void PrintBuildInformation(Printable& tout);
void PrintFixedPinInformation(Printable& tout);
void PrintMemoryMap(Printable& tout);
void Print(Printable& tout);

}

#endif
