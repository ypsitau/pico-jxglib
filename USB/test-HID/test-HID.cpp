#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "usb_descriptors.h"
#include "jxglib/GPIO.h"
#include "jxglib/USB.h"

void hid_task(void);

using namespace jxglib;

#if 0
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xcafe,
    .idProduct          = 0x4000,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
#endif

int main(void)
{
	::stdio_init_all(); 
	USB::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_HID, //0x00,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON, //0x00,
		bDeviceProtocol:	MISC_PROTOCOL_IAD, //0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			0x4000,
		bcdDevice:			0x0100,
		iManufacturer:		0x01,
		iProduct:			0x02,
		iSerialNumber:		0x03,
	});
	//device.AddInterface();
	GPIO16.init().set_dir_IN().pull_up();
	GPIO17.init().set_dir_IN().pull_up();
	GPIO18.init().set_dir_IN().pull_up();
	GPIO19.init().set_dir_IN().pull_up();
	GPIO20.init().set_dir_IN().pull_up();
	GPIO21.init().set_dir_IN().pull_up();
	//::tud_init(BOARD_TUD_RHPORT);
	device.Initialize();
	for (;;) {
		//USB::Device::Task();
		::tud_task();
		::hid_task();
	}
	return 0;
}


// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus

uint32_t board_millis()
{
	return ::to_ms_since_boot(::get_absolute_time());
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
	// Poll every 10ms
	const uint32_t interval_ms = 10;
	static uint32_t start_ms = 0;

	if (board_millis() - start_ms < interval_ms) return; // not enough time
	start_ms += interval_ms;

	//uint32_t const btn = board_button_read();
	bool btnLeft = !GPIO16.get();
	bool btnUp = !GPIO17.get();
	bool btnDown = !GPIO18.get();
	bool btnRight = !GPIO19.get();
	bool btnA = !GPIO20.get();
	bool btnB = !GPIO21.get();

	if (::tud_suspended() && (btnLeft || btnUp || btnDown || btnRight)) {
		// Wake up host if we are in suspend mode
		// and REMOTE_WAKEUP feature is enabled by host
		::tud_remote_wakeup();
		return;
	}
	if (::tud_hid_n_ready(ITF_NUM_KEYBOARD)) {
		// keyboard interface
		// used to avoid send multiple consecutive zero report for keyboard
		static bool has_keyboard_key = false;

		uint8_t report_id = 0;
		uint8_t modifier  = 0;

		if (btnLeft) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_ARROW_LEFT;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else if (btnUp) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_ARROW_UP;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else if (btnDown) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_ARROW_DOWN;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else if (btnRight) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_ARROW_RIGHT;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else if (btnA) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_PAGE_UP;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else if (btnB) {
			uint8_t keycode[6] = { 0 };
			keycode[0] = HID_KEY_PAGE_DOWN;
			::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
			has_keyboard_key = true;
		} else {
			// send empty key report if previously has key pressed
			if (has_keyboard_key) ::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, NULL);
			has_keyboard_key = false;
		}
	}
#if 0
	// mouse interface
	if (::tud_hid_n_ready(ITF_NUM_MOUSE)) {
		if (btn ) {
			uint8_t const report_id   = 0;
			uint8_t const button_mask = 0;
			int8_t  const vertical    = 0;
			int8_t  const horizontal  = 0;
			int8_t  const delta       = 5;
			::tud_hid_n_mouse_report(ITF_NUM_MOUSE, report_id, button_mask, delta, delta, vertical, horizontal);
		}
	}
#endif
}

