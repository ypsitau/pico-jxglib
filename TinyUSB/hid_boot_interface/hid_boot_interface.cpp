#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "jxglib/GPIO.h"

void hid_task(void);

using namespace jxglib;

int main(void)
{
	::stdio_init_all();    
	GPIO16.init().set_dir_IN().pull_up();
	GPIO17.init().set_dir_IN().pull_up();
	GPIO18.init().set_dir_IN().pull_up();
	GPIO19.init().set_dir_IN().pull_up();
	GPIO20.init().set_dir_IN().pull_up();
	GPIO21.init().set_dir_IN().pull_up();
	::tud_init(BOARD_TUD_RHPORT);
	for (;;) {
		::tud_task();
		::hid_task();
	}
	return 0;
}

void tud_mount_cb(void)
{
}

void tud_umount_cb(void)
{
}

// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
}

void tud_resume_cb(void)
{
	//tud_mounted();
}

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

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol)
{
	// TODO set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
	return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
	// keyboard interface
	if (instance == ITF_NUM_KEYBOARD) {
		// Set keyboard LED e.g Capslock, Numlock etc...
		if (report_type == HID_REPORT_TYPE_OUTPUT) {
			// bufsize should be (at least) 1
			if (bufsize < 1) return;
			uint8_t const kbd_leds = buffer[0];
			if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
				// Capslock On: disable blink, turn led on
				//blink_interval_ms = 0;
				//board_led_write(true);
			} else {
				// Caplocks Off: back to normal blink
				//board_led_write(false);
			}
		}
	}
}
