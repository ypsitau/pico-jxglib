//==============================================================================
// UART.cpp
//==============================================================================
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// UART
//------------------------------------------------------------------------------
UART UART0(uart0);
UART UART1(uart1);
UART UART::Default(uart_default);

bool UART::Read(void* buff, int bytesBuff, int* pBytesRead)
{
	return true;
}

bool UART::Write(const void* buff, int bytesBuff)
{
	return true;
}


}
