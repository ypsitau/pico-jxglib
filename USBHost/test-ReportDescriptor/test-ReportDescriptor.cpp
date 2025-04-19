#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/USBHost.h"

using namespace jxglib;
/*
C 言語で、キーボードの USB report descriptor と、それをテストする report データのサンプルを 10 個つくってください。
- reort descritor の変数名は descReort, report データサンプルの変数名は reportTbl とすること
- コメントを英語で表記すること
*/

void test_Keyboard()
{
	// USB HID Report Descriptor for a keyboard
	const uint8_t descReport[] = {
		0x05, 0x01, // Usage Page (Generic Desktop)
		0x09, 0x06, // Usage (Keyboard)
		0xA1, 0x01, // Collection (Application)
		0x05, 0x07, // Usage Page (Key Codes)
		0x19, 0xE0, // Usage Minimum (224)
		0x29, 0xE7, // Usage Maximum (231)
		0x15, 0x00, // Logical Minimum (0)
		0x25, 0x01, // Logical Maximum (1)
		0x75, 0x01, // Report Size (1)
		0x95, 0x08, // Report Count (8)
		0x81, 0x02, // Input (Data, Variable, Absolute) - Modifier keys
		0x95, 0x01, // Report Count (1)
		0x75, 0x08, // Report Size (8)
		0x81, 0x03, // Input (Constant) - Reserved byte
		0x95, 0x06, // Report Count (6)
		0x75, 0x08, // Report Size (8)
		0x15, 0x00, // Logical Minimum (0)
		0x25, 0x65, // Logical Maximum (101)
		0x05, 0x07, // Usage Page (Key Codes)
		0x19, 0x00, // Usage Minimum (0)
		0x29, 0x65, // Usage Maximum (101)
		0x81, 0x00, // Input (Data, Array) - Key array
		0xC0        // End Collection
	};
	// Sample report data for testing (10 samples)
	const uint8_t reportTbl[10][8] = {
		{0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'a'
		{0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'b'
		{0x02, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'c'
		{0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'd'
		{0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'e'
		{0x02, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'f'
		{0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'g'
		{0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'h'
		{0x02, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'i'
		{0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00}  // 'j'
	};
	RefPtr<USBHost::ReportDescriptor::Application> pApplication(::USBHost::Instance.reportDescriptor.Parse(descReport, sizeof(descReport)));
	USBHost::GenericHID hid(pApplication.Reference(), true);
	pApplication->Print(Stdio::Instance);
	for (int i = 0; i < count_of(reportTbl); i++) {
		hid.OnReport(0, 0, reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Modifier:%d%d%d%d%d%d%d%d %02x %02x %02x %02x %02x %02x\n",
			hid.GetVariable(0x0007'00e0),
			hid.GetVariable(0x0007'00e1),
			hid.GetVariable(0x0007'00e2),
			hid.GetVariable(0x0007'00e3),
			hid.GetVariable(0x0007'00e4),
			hid.GetVariable(0x0007'00e5),
			hid.GetVariable(0x0007'00e6),
			hid.GetVariable(0x0007'00e7),
			hid.GetArrayItem(0), hid.GetArrayItem(1), hid.GetArrayItem(2),
			hid.GetArrayItem(3), hid.GetArrayItem(4), hid.GetArrayItem(5));
	}
}

void test_Mouse()
{
	// USB Mouse Report Descriptor
	uint8_t descReport[] = {
		0x05, 0x01, // Usage Page (Generic Desktop)
		0x09, 0x02, // Usage (Mouse)
		0xA1, 0x01, // Collection (Application)
		0x09, 0x01, //   Usage (Pointer)
		0xA1, 0x00, //   Collection (Physical)
		0x05, 0x09, //     Usage Page (Button)
		0x19, 0x01, //     Usage Minimum (Button 1)
		0x29, 0x03, //     Usage Maximum (Button 3)
		0x15, 0x00, //     Logical Minimum (0)
		0x25, 0x01, //     Logical Maximum (1)
		0x95, 0x03, //     Report Count (3)
		0x75, 0x01, //     Report Size (1)
		0x81, 0x02, //     Input (Data, Variable, Absolute)
		0x95, 0x01, //     Report Count (1)
		0x75, 0x05, //     Report Size (5)
		0x81, 0x03, //     Input (Constant, Variable, Absolute)
		0x05, 0x01, //     Usage Page (Generic Desktop)
		0x09, 0x30, //     Usage (X)
		0x09, 0x31, //     Usage (Y)
		0x15, 0x81, //     Logical Minimum (-127)
		0x25, 0x7F, //     Logical Maximum (127)
		0x75, 0x08, //     Report Size (8)
		0x95, 0x02, //     Report Count (2)
		0x81, 0x06, //     Input (Data, Variable, Relative)
		0xC0,       //   End Collection
		0xC0        // End Collection
	};
	// Sample report data for testing
	uint8_t reportTbl[10][3] = {
		{0x00, 0x00, 0x00}, // No buttons pressed, no movement
		{0x01, 0x10, 0x10}, // Button 1 pressed, move diagonally
		{0x02, 0xFF, 0x00}, // Button 2 pressed, move left
		{0x04, 0x00, 0xFF}, // Button 3 pressed, move down
		{0x03, 0x7F, 0x7F}, // Button 1 and 2 pressed, move to bottom-right
		{0x00, 0x81, 0x81}, // No buttons pressed, move to top-left
		{0x01, 0x00, 0x7F}, // Button 1 pressed, move up
		{0x02, 0x7F, 0x00}, // Button 2 pressed, move right
		{0x04, 0x7F, 0x81}, // Button 3 pressed, move diagonally
		{0x00, 0x00, 0x00}  // No buttons pressed, no movement
	};
	RefPtr<USBHost::ReportDescriptor::Application> pApplication(::USBHost::Instance.reportDescriptor.Parse(descReport, sizeof(descReport)));
	USBHost::GenericHID hid(pApplication.Reference(), true);
	pApplication->Print(Stdio::Instance);
	for (int i = 0; i < count_of(reportTbl); i++) {
		hid.OnReport(0, 0, reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Button1:%d Button2:%d Button3:%d X:%d Y:%d\n",
			hid.GetVariable(0x0001'0001, 0x0009'0001),
			hid.GetVariable(0x0001'0001, 0x0009'0002),
			hid.GetVariable(0x0001'0001, 0x0009'0003),
			hid.GetVariable(0x0001'0001, 0x0001'0030),
			hid.GetVariable(0x0001'0001, 0x0001'0031));
	}
}

int main()
{
	::stdio_init_all();
	Printable::SetStandardOutput(Stdio::Instance);
	test_Keyboard();
	test_Mouse();
	for (;;) ::tight_loop_contents();
}
