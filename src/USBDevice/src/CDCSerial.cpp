//==============================================================================
// CDCSerial.cpp
//==============================================================================
#include "jxglib/USBDevice/CDCSerial.h"
#include "pico/bootrom.h"

#define PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE 1200
#define PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK 0u

namespace jxglib::USBDevice {

CDCSerial::CDCSerial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64), chPrev_{'\0'}, addCrFlag_{true}, keyboard_{*this} {}

#if 1
void CDCSerial::On_cdc_line_coding(const cdc_line_coding_t* p_line_coding)
{
	// Check for the magic baud rate to trigger USB boot reset.
	// This emulates the behavior of USB stdio implemented in pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c:tud_cdc_line_coding_cb()
	if (p_line_coding->bit_rate == PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE) {
#ifdef PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED
		int gpio = PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED;
		bool active_low = PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED_ACTIVE_LOW;
#else
		int gpio = -1;
		bool active_low = false;
#endif
		rom_reset_usb_boot_extra(gpio, PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK, active_low);
	}
}
#endif

int CDCSerial::Read(void* buff, int bytesBuff)
{
	int bytesRead = 0;
	while (bytesRead < bytesBuff) {
		int bytes = cdc_read(static_cast<uint8_t*>(buff) + bytesRead, bytesBuff - bytesRead);
		if (bytes <= 0) break; // no more data available
		bytesRead += bytes;
		Tickable::TickSub();
	}
	return bytesRead;
}

int CDCSerial::Write(const void* buff, int bytesBuff)
{
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += cdc_write(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::TickSub();
	}
	return bytesWritten;
}

bool CDCSerial::Flush()
{
	cdc_write_flush();
	return true;
}

Printable& CDCSerial::PutChar(char ch)
{
	if (addCrFlag_ && chPrev_ != '\r' && ch == '\n') PutCharRaw('\r');
	PutCharRaw(ch);
	chPrev_ = ch;
	return *this;
}

Printable& CDCSerial::PutCharRaw(char ch)
{
	Stream::PutCharRaw(ch);
	if (ch == '\n') cdc_write_flush();
	return *this;
}

}
