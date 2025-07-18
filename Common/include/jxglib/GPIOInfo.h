//==============================================================================
// jxglib/GPIOFuncName.h
//==============================================================================
#ifndef PICO_JXGLIB_GPIOFUNCNAME_H
#define PICO_JXGLIB_GPIOFUNCNAME_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib::GPIOInfo {

const char* GetHelp_AvailableFunc();
gpio_function_t StringToFunc(const char* str, bool* pValidFlag);
const char* GetFuncName(int pinFunc, const char* funcNameNull = "NULL");
const char* GetFuncName(int pinFunc, uint pin, const char* funcNameNull = "NULL");

}

#endif
