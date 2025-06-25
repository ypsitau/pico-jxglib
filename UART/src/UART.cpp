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

UART::UART(uart_inst_t* uart) : raw(uart), chPrev_{'\0'}, addCrFlag_{true}
{}

int UART::Read(void* buff, int bytesBuff)
{
	uint8_t* pBuff = static_cast<uint8_t*>(buff);
	int i = 0;
	//for ( ; i < bytesBuff && raw.IsReadable(); ++i) {
	for ( ; i < bytesBuff && raw.is_readable(); ++i) {
		*pBuff++ = static_cast<uint8_t>(raw.get_hw()->dr);
	}
	return i;
}

int UART::Write(const void* buff, int bytesBuff)
{
	const uint8_t* pBuff = static_cast<const uint8_t*>(buff);
	int i = 0;
	for ( ; i < bytesBuff; ++i) {
		//while (!raw.IsWritable()) ::tight_loop_contents();
		while (!raw.is_writable()) ::tight_loop_contents();
		raw.get_hw()->dr = *pBuff++;
	}
	return i;
}

Printable& UART::ClearScreen()
{
	chPrev_ = '\0';
	return Stream::ClearScreen();
}

Printable& UART::RefreshScreen()
{
	chPrev_ = '\0';
	return Stream::RefreshScreen();
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
