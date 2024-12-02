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
	raw.write_blocking(buff, bytesBuff);
	return true;
}

Printable& UART::ClearScreen()
{
	chPrev_ = '\0';
	return Stream::ClearScreen();
}

Printable& UART::FlushScreen()
{
	chPrev_ = '\0';
	return Stream::FlushScreen();
}

Printable& UART::PutChar(char ch)
{
	if (addCrFlag_ && chPrev_ != '\r' && ch == '\n') PutCharRaw('\r');
	PutCharRaw(ch);
	chPrev_ = ch;
	return *this;
}

Printable& UART::PutCharRaw(char ch)
{
	while (!raw.is_writable()) ::tight_loop_contents();
	raw.get_hw()->dr = ch;
	return *this;
}

}
