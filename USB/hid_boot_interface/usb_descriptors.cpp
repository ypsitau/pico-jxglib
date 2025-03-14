//#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
	.bLength            = sizeof(tusb_desc_device_t),
	.bDescriptorType    = TUSB_DESC_DEVICE,
	.bcdUSB             = 0x0200,
	.bDeviceClass       = 0x00,
	.bDeviceSubClass    = 0x00,
	.bDeviceProtocol    = 0x00,
	.bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
	.idVendor           = 0xCafe,
	.idProduct          = 0x4000 | (CFG_TUD_CDC << 0) | (CFG_TUD_MSC << 1) | (CFG_TUD_HID << 2) | (CFG_TUD_MIDI << 3) | (CFG_TUD_VENDOR << 4),
	.bcdDevice          = 0x0100,
	.iManufacturer      = 0x01,
	.iProduct           = 0x02,
	.iSerialNumber      = 0x03,
	.bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
	return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_keyboard_report[] =
{
	TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const desc_hid_mouse_report[] =
{
	TUD_HID_REPORT_DESC_MOUSE()
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
	return (instance == 0) ? desc_hid_keyboard_report : desc_hid_mouse_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
	// LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
	// 1 Interrupt, 2 Bulk, 3 Iso, 4 Interrupt, 5 Bulk etc ...
	#define EPNUM_KEYBOARD   0x81
	#define EPNUM_MOUSE      0x84
#else
	#define EPNUM_KEYBOARD   0x81
	#define EPNUM_MOUSE      0x82
#endif

uint8_t const desc_configuration[] =
{
	// Config number, interface count, string index, total length, attribute, power in mA
	TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, (TUD_CONFIG_DESC_LEN + 2 * TUD_HID_DESC_LEN), TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

	// Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
	TUD_HID_DESCRIPTOR(ITF_NUM_KEYBOARD, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_keyboard_report), EPNUM_KEYBOARD, CFG_TUD_HID_EP_BUFSIZE, 10),

	// Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
	TUD_HID_DESCRIPTOR(ITF_NUM_MOUSE, 0, HID_ITF_PROTOCOL_MOUSE, sizeof(desc_hid_mouse_report), EPNUM_MOUSE, CFG_TUD_HID_EP_BUFSIZE, 10)
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
	(void) index; // for multiple configurations
	return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String Descriptor Index
enum {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
};

// array of pointer to string descriptors
char const *string_desc_arr[] =
{
	(const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
	"TinyUSB",                     // 1: Manufacturer
	"TinyUSB Device",              // 2: Product
	NULL,                          // 3: Serials will use unique ID if possible
};

static inline size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars) {
	uint8_t uid[16] TU_ATTR_ALIGNED(4);
	size_t uid_len;

	// TODO work with make, but not working with esp32s3 cmake
	//if ( board_get_unique_id ) {
	//  uid_len = board_get_unique_id(uid, sizeof(uid));
	{
		// fixed serial string is 01234567889ABCDEF
		uint32_t* uid32 = (uint32_t*) (uintptr_t) uid;
		uid32[0] = 0x67452301;
		uid32[1] = 0xEFCDAB89;
		uid_len = 8;
	}

	if ( uid_len > max_chars / 2 ) uid_len = max_chars / 2;

	for ( size_t i = 0; i < uid_len; i++ ) {
		for ( size_t j = 0; j < 2; j++ ) {
			const char nibble_to_hex[16] = {
					'0', '1', '2', '3', '4', '5', '6', '7',
					'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
			};
			uint8_t const nibble = (uid[i] >> (j * 4)) & 0xf;
			desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
		}
	}

	return 2 * uid_len;
}


static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
	(void) langid;
	size_t chr_count;

	switch ( index ) {
		case STRID_LANGID:
			memcpy(&_desc_str[1], string_desc_arr[0], 2);
			chr_count = 1;
			break;

		case STRID_SERIAL:
			chr_count = board_usb_get_serial(_desc_str + 1, 32);
			break;

		default:
			// Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

			if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

			const char *str = string_desc_arr[index];

			// Cap at max char
			chr_count = strlen(str);
			size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
			if ( chr_count > max_count ) chr_count = max_count;

			// Convert ASCII string into UTF-16
			for ( size_t i = 0; i < chr_count; i++ ) {
				_desc_str[1 + i] = str[i];
			}
			break;
	}

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

	return _desc_str;
}
