//==============================================================================
// jxglib/UART.h
//==============================================================================
#ifndef PICO_JXGLIB_UART_H
#define PICO_JXGLIB_UART_H
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
namespace jxglib {

//------------------------------------------------------------------------------
// UART
//------------------------------------------------------------------------------
class UART : public Stream {
public:
	class Raw {
	private:
		uart_inst_t* uart_;
	public:
		Raw(uart_inst_t* uart) : uart_{uart} {}
	public:
		uint get_index() const { return ::uart_get_index(uart_); }
		uint init(uint baudrate) const { return ::uart_init(uart_, baudrate); }
		const Raw& deinit() const { ::uart_deinit(uart_); return *this; }
		uint set_baudrate(uint baudrate) const { return ::uart_set_baudrate(uart_, baudrate); }
		const Raw& set_hw_flow(bool cts, bool rts) const { ::uart_set_hw_flow(uart_, cts, rts); return *this; }
		const Raw& set_format(uint data_bits, uint stop_bits, uart_parity_t parity) const { ::uart_set_format(uart_, data_bits, stop_bits, parity); return *this; }
		const Raw& set_irq_enables(bool rx_has_data, bool tx_needs_data) const { ::uart_set_irq_enables(uart_, rx_has_data, tx_needs_data); return *this; }
		bool is_enabled() const { return ::uart_is_enabled(uart_); }
		const Raw& set_fifo_enabled(bool enabled) const { ::uart_set_fifo_enabled(uart_, enabled); return *this; }
		bool is_writable() const { return ::uart_is_writable(uart_); }
		const Raw& tx_wait_blocking() const { ::uart_tx_wait_blocking(uart_); return *this; }
		bool is_readable() const { return ::uart_is_readable(uart_); }
		const Raw& write_blocking(const void* src, size_t len) const { ::uart_write_blocking(uart_, reinterpret_cast<const uint8_t*>(src), len); return *this; }
		const Raw& read_blocking(void* dst, size_t len) const { ::uart_read_blocking(uart_, reinterpret_cast<uint8_t*>(dst), len); return *this; }
		const Raw& putc_raw(char c) const { ::uart_putc_raw(uart_, c); return *this; }
		const Raw& putc(char c) const { ::uart_putc(uart_, c); return *this; }
		const Raw& puts(const char* s) const { ::uart_puts(uart_, s); return *this; }
		char getc() const { return ::uart_getc(uart_); }
		const Raw& set_break(bool en) const { ::uart_set_break(uart_, en); return *this; }
		const Raw& set_translate_crlf(bool translate) const { ::uart_set_translate_crlf(uart_, translate); return *this; }
		bool is_readable_within_us(uint32_t us) const { return ::uart_is_readable_within_us(uart_, us); }
		uint get_dreq(bool is_tx) const { return ::uart_get_dreq(uart_, is_tx); }
		uart_hw_t* get_hw() const { return ::uart_get_hw(uart_); }
	public:
		static void default_tx_wait_blocking() { ::uart_default_tx_wait_blocking(); }
	} raw;
private:
	char chPrev_;
public:
	static UART Default;
public:
	UART(uart_inst_t* uart) : raw(uart), chPrev_('\0') {}
public:
	// virtual functions of Stream
	virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) override;
	virtual bool Write(const void* buff, int bytesBuff) override;
	// virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& FlushScreen() override;
	virtual Printable& Print(const char* str) override;
};

extern UART UART0;
extern UART UART1;

}

#endif