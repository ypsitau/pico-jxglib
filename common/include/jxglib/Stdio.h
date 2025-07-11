//==============================================================================
// jxglib/Stdio.h
//==============================================================================
#ifndef PICO_JXGLIB_STDIO_H
#define PICO_JXGLIB_STDIO_H
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "jxglib/Keyboard.h"
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------
class Stdio : public Stream {
private:
	int bytesRead_; // Number of bytes read from the input
	int bytesAvailable_; // Number of bytes available for reading
	uint8_t buffRead_[256]; // Buffer for reading input
	VT100::Keyboard keyboard_;
public:
	static Stdio Instance;
public:
	Stdio() : bytesRead_(0), bytesAvailable_(0), keyboard_(*this) {}
public:
	// virtual functions of Readable
	virtual int ReadChar();
	virtual int Read(void* buff, int bytesBuff) override;
	// virtual functions of Printable
	virtual int Write(const void* buff, int bytesBuff) override;
public:
	// Buffer management methods
	void ClearReadBuffer();
	int GetAvailableBytes() const { return bytesAvailable_; }
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { return *this; }
	virtual bool Flush() override { ::stdio_flush(); return true; }
	virtual Printable& Locate(int col, int row) override { return *this; }
	virtual Printable& PutChar(char ch) override { ::stdio_putchar(ch); return *this; }
	virtual Printable& PutCharRaw(char ch) override { ::stdio_putchar_raw(ch); return *this; }
	virtual Printable& Print(const char* str) override { ::stdio_put_string(str, ::strlen(str), false, true); return *this; }
	virtual Printable& PrintRaw(const char* str) override { ::stdio_put_string(str, ::strlen(str), false, false); return *this; }
	virtual Printable& Println(const char* str = "") override { ::stdio_put_string(str, ::strlen(str), true, true); return *this; }
	virtual Printable& PrintlnRaw(const char* str = "") override { ::stdio_put_string(str, ::strlen(str), true, false); return *this; }
public:
	static Printable& GetPrintable() { return Instance; }
	static Keyboard& GetKeyboard() { return Instance.keyboard_; }
public:
	static bool init_all() { return ::stdio_init_all(); }
	static bool deinit_all() { return ::stdio_deinit_all(); }
	static void flush() { return ::stdio_flush(); }
	static int getchar_timeout_us(uint32_t timeout_us) { return ::stdio_getchar_timeout_us(timeout_us); }
	static void set_driver_enabled(stdio_driver_t *driver, bool enabled) { ::stdio_set_driver_enabled(driver, enabled); }
	static void filter_driver(stdio_driver_t *driver) { ::stdio_filter_driver(driver); }
	static void set_translate_crlf(stdio_driver_t *driver, bool translate) { ::stdio_set_translate_crlf(driver, translate); }
	static int putchar_raw(int c) { return ::stdio_putchar_raw(c); }
	static int puts_raw(const char *s) { return ::stdio_puts_raw(s); }
	static void set_chars_available_callback(void(*fn)(void *), void *param) { ::stdio_set_chars_available_callback(fn, param); }
	static int get_until(char *buf, int len, absolute_time_t until) { return ::stdio_get_until(buf, len, until); }
	static int put_string(const char *s, int len, bool newline, bool cr_translation) { return ::stdio_put_string(s, len, newline, cr_translation); }
	static int getchar() { return ::stdio_getchar(); }
	static int putchar(int c) { return ::stdio_putchar(c); }
	static int puts(const char *s) { return ::stdio_puts(s); }
	static int vprintf(const char *format, va_list va) { return ::stdio_vprintf(format, va); }
};

}

#endif
