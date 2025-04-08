//==============================================================================
// jxglib/Stdio.h
//==============================================================================
#ifndef PICO_JXGLIB_STDIO_H
#define PICO_JXGLIB_STDIO_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Keyboard.h"
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stdio
//------------------------------------------------------------------------------
class Stdio {
public:
	class Printable : public jxglib::Printable {
	public:
		static Printable Instance;
	public:
		virtual Printable& ClearScreen() { return *this; }
		virtual Printable& RefreshScreen() { ::stdio_flush(); return *this; }
		virtual Printable& Locate(int col, int row) { return *this; }
		virtual Printable& PutChar(char ch) { ::stdio_putchar(ch); return *this; }
		virtual Printable& PutCharRaw(char ch) { ::stdio_putchar_raw(ch); return *this; }
		virtual Printable& Print(const char* str) { ::stdio_put_string(str, ::strlen(str), false, true); return *this; }
		virtual Printable& PrintRaw(const char* str) { ::stdio_put_string(str, ::strlen(str), false, false); return *this; }
		virtual Printable& Println(const char* str = "") { ::stdio_put_string(str, ::strlen(str), true, true); return *this; }
		virtual Printable& PrintlnRaw(const char* str = "") { ::stdio_put_string(str, ::strlen(str), true, false); return *this; }
	};
	class Keyboard : public jxglib::Keyboard {
	private:
		VT100::Decoder decoder_;
	public:
		static Keyboard Instance;
	public:
		virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) override;
		virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override;
		virtual bool GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier = nullptr) override;
		virtual bool GetKeyDataNB(KeyData* pKeyData) override;
	};
public:
	Stdio() {}
public:
	static Printable& GetPrintable() { return Printable::Instance; }
	static Keyboard& GetKeyboard() { return Keyboard::Instance; }
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
