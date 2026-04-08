//==============================================================================
// jxglib/USBDevice/CDC.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_CDC_H
#define PICO_JXGLIB_USBDEVICE_CDC_H
#include "jxglib/USBDevice.h"

#if CFG_TUD_CDC > 0

namespace jxglib::USBDevice {

class CDC : public Interface {
public:
	CDC(Controller& deviceController, const char* str, uint8_t endpNotif, uint8_t bytesNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk);
public:
	virtual void On_cdc_rx() {}
	virtual void On_cdc_rx_wanted(char wanted_char) {}
	virtual void On_cdc_tx_complete() {}
	virtual void On_cdc_line_state(bool dtr, bool rts) {}
	virtual void On_cdc_line_coding(const cdc_line_coding_t* p_line_coding) {}
	virtual void On_cdc_send_break(uint16_t duration_ms) {}
public:
	bool cdc_ready() { return ::tud_cdc_n_ready(iInstance_); }
	bool cdc_connected() { return ::tud_cdc_n_connected(iInstance_); }
	uint8_t cdc_get_line_state() { return ::tud_cdc_n_get_line_state(iInstance_); }
	void cdc_get_line_coding(cdc_line_coding_t* coding) { return ::tud_cdc_n_get_line_coding(iInstance_, coding); }
	void cdc_set_wanted_char(char wanted) { return ::tud_cdc_n_set_wanted_char(iInstance_, wanted); }
	uint32_t cdc_available() { return ::tud_cdc_n_available(iInstance_); }
	uint32_t cdc_read(void* buffer, uint32_t bufsize) { return ::tud_cdc_n_read(iInstance_, buffer, bufsize); }
	int32_t cdc_read_char() { return ::tud_cdc_n_read_char(iInstance_); }
	void cdc_read_flush() { return ::tud_cdc_n_read_flush(iInstance_); }
	bool cdc_peek(uint8_t* ui8) { return ::tud_cdc_n_peek(iInstance_, ui8); }
	uint32_t cdc_write(void const* buffer, uint32_t bufsize) { return ::tud_cdc_n_write(iInstance_, buffer, bufsize); }
	uint32_t cdc_write_char(char ch) { return ::tud_cdc_n_write_char(iInstance_, ch); }
	uint32_t cdc_write_str(char const* str) { return ::tud_cdc_n_write_str(iInstance_, str); }
	uint32_t cdc_write_flush() { return ::tud_cdc_n_write_flush(iInstance_); }
	uint32_t cdc_write_available() { return ::tud_cdc_n_write_available(iInstance_); }
	bool write_clear() { return ::tud_cdc_n_write_clear(iInstance_); }
};

}

#endif

#endif
