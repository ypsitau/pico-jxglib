#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"
#include "jxglib/Display.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Device/OV7670.h"

using namespace jxglib;

Device::OV7670 ov7670(i2c0, {DIN0: GPIO2, XLK: GPIO10, PLK: GPIO11, HREF: GPIO12, VSYNC: GPIO13}, 25000000);

jxglib::Device::OV7670& ShellCmd_Device_OV7670_GetOV7670()
{
	return ov7670;
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(false);
	Image image;
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	::i2c_init(i2c0, 100000);
	ov7670.Initialize();
	ov7670.WriteReg(Device::OV7670::Reg12_COM7,
		(0b1 << 7));	// SCCB Register Reset
	::sleep_ms(100);
	ov7670
	.WriteReg(Device::OV7670::Reg3A_TSLB,
		(0b0 << 5) | 		// Negative image enable
		(0b0 << 4) |		// UV output value
		(0b0 << 3) |		// Output sequence
		(0b0 << 0))			// Auto output window
	.WriteReg(Device::OV7670::Reg15_COM10,
		(0b0 << 6) |		// HREF changes to HSYNC
		(0b0 << 5) |		// PCLK output option
		(0b0 << 4) |		// PCLK reverse
		(0b0 << 3) |		// HREF reverse
		(0b0 << 2) |		// VSYNC option
		(0b1 << 1) |		// VSYNC negative
		(0b0 << 0))			// HSYNC negative
	.WriteReg(Device::OV7670::Reg7A_SLOP,
		0x20)				// Gamma curve highest segment slope
	.WriteReg(Device::OV7670::Reg7B_GAM1,
		0x1C)				// Gamma curve 1st Segment Input End Point 0x04 Output Value
	.WriteReg(Device::OV7670::Reg7C_GAM2,
		0x28)				// Gamma curve 2nd Segment Input End Point 0x08 Output Value
	.WriteReg(Device::OV7670::Reg7D_GAM3,
		0x3C)				// Gamma curve 3rd Segment Input End Point 0x10 Output Value
	.WriteReg(Device::OV7670::Reg7E_GAM4,
		0x55)				// Gamma curve 4th Segment Input End Point 0x20 Output Value
	.WriteReg(Device::OV7670::Reg7F_GAM5,
		0x68)				// Gamma curve 5th Segment Input End Point 0x28 Output Value
	.WriteReg(Device::OV7670::Reg80_GAM6,
		0x76)				// Gamma curve 6th Segment Input End Point 0x30 Output Value
	.WriteReg(Device::OV7670::Reg81_GAM7,
		0x80)				// Gamma curve 7th Segment Input End Point 0x38 Output Value
	.WriteReg(Device::OV7670::Reg82_GAM8,
		0x88)				// Gamma curve 8th Segment Input End Point 0x40 Output Value
	.WriteReg(Device::OV7670::Reg83_GAM9,
		0x8F)				// Gamma curve 9th Segment Input End Point 0x48 Output Value
	.WriteReg(Device::OV7670::Reg84_GAM10,
		0x96)				// Gamma curve 10th Segment Input End Point 0x50 Output Value
	.WriteReg(Device::OV7670::Reg85_GAM11,
		0xA3)				// Gamma curve 11th Segment Input End Point 0x60 Output Value
	.WriteReg(Device::OV7670::Reg86_GAM12,
		0xAF)				// Gamma curve 12th Segment Input End Point 0x70 Output Value
	.WriteReg(Device::OV7670::Reg87_GAM13,
		0xC4)				// Gamma curve 13th Segment Input End Point 0x90 Output Value
	.WriteReg(Device::OV7670::Reg88_GAM14,
		0xD7)				// Gamma curve 14th Segment Input End Point 0xB0 Output Value
	.WriteReg(Device::OV7670::Reg89_GAM15,
		0xE8)				// Gamma curve 15th Segment Input End Point 0xD0 Output Value
	.WriteReg(Device::OV7670::Reg12_COM7,
		(0b0 << 5) |		// Output format - CIF selection
		(0b1 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0))		// Output format - Raw RGB
	.WriteReg(Device::OV7670::Reg11_CLKRC,
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0))	// Internal clock prescaler
	.WriteReg(Device::OV7670::Reg0C_COM3,
		(0b0 << 6) |		// Output data MSB and LSB swap
		(0b0 << 5) |		// Tri-state option for output clock at power-down period
		(0b0 << 4) |		// Tri-state option for output data at power-down period
		(0b0 << 3) |		// Scale enable
		(0b1 << 2))			// DCW enable
	.WriteReg(Device::OV7670::Reg3E_COM14,
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b010 << 0))		// PCLK divider (only when COM14[4] = 1)
	.WriteReg(Device::OV7670::Reg40_COM15,
		(0b11 < 6) |		// Data format - output full range enable
		(0b01 << 4))		// RGB 555/565 option
	.WriteReg(Device::OV7670::Reg8C_RGB444,
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
		(0b0 << 0))			// RGB444 word format
	.WriteReg(Device::OV7670::Reg04_COM1,
		(0b0 << 6) |		// CCIR656 format
		(0b00 << 0))		// AEC[1:0]
	.WriteReg(Device::OV7670::Reg14_COM9,
		(0b001 << 4) |		// Automatic Gain Ceiling
		(0b0 << 0))			// Freeze AGC/AEC
	.WriteReg(Device::OV7670::Reg4F_MTX1,
		0x80)				// Matrix Coefficient 1
	.WriteReg(Device::OV7670::Reg50_MTX2,
		0x80)				// Matrix Coefficient 2
	.WriteReg(Device::OV7670::Reg51_MTX3,
		0x00)				// Matrix Coefficient 3
	.WriteReg(Device::OV7670::Reg52_MTX4,
		0x22)				// Matrix Coefficient 4
	.WriteReg(Device::OV7670::Reg53_MTX5,
		0x5e)				// Matrix Coefficient 5
	.WriteReg(Device::OV7670::Reg54_MTX6,
		0x80)				// Matrix Coefficient 6
	.WriteReg(Device::OV7670::Reg58_MTXS,
		(0b1 << 7) |		// Auto contrast center enable
		(0b000001 << 0))	// Matrix coeffieicnt sign
	.WriteReg(Device::OV7670::Reg41_COM16,
		(0b0 << 5) |		// Enable edge enhancement threshold auto-adjustment
		(0b0 << 4) |		// De-noise threshold auto-adjustment
		(0b1 << 3) |		// AWB gain enable
		(0b0 << 1))			// Color matrix coefficient double option
	.WriteReg(Device::OV7670::Reg3F_EDGE,
		(0b00000 << 0))		// Edge enhancement factor
	.WriteReg(Device::OV7670::Reg75,
		(0b00101 << 0))		// Edge enhancement lower limit
	.WriteReg(Device::OV7670::Reg76,
		(0b1 << 7) |		// Black pixel correction enable
		(0b1 << 6) |		// White pixel correction enable
		(0b00001 << 0))		// Edge enhancement upper limit
	.WriteReg(Device::OV7670::Reg4C_DNSTH,
		0x00)				// De-noise Strength
	.WriteReg(Device::OV7670::Reg77,
		0x01)				// De-noise offset
	.WriteReg(Device::OV7670::Reg3D_COM13,
		(0b1 << 7) |		// Gamma enable
		(0b1 << 6) |		// UV saturation level - UV auto-adjustment
		(0b1 << 0))			// UV swap
	.WriteReg(Device::OV7670::Reg4B_REG4B,
		(0b1 << 0))			// UV average enable
	.WriteReg(Device::OV7670::RegC9_SATCTR,
		0x60)
	.WriteReg(Device::OV7670::Reg41_COM16,
		(0b1 << 5) |		// Enable edge enhancement threshold auto-adjustment for YUV output
		(0b1 << 4) |		// De-noise threshold auto-adjustment
		(0b1 << 3) |		// AWB gain enable
		(0b0 << 1))			// Color matrix coefficient double option
	.WriteReg(Device::OV7670::Reg56_CONTRAS,
		0x40)				// Contrast control
	.WriteReg(Device::OV7670::Reg70_SCALING_XSC,
		(0b0 << 7) |		// Test pattern[0]
		(0b0111010 << 0))	// Horizontal scale factor
	.WriteReg(Device::OV7670::Reg71_SCALING_YSC,
		(0b0 << 7) |		// Test pattern[1]
		(0b0110101 << 0))	// Vertical scale factor
	.WriteReg(Device::OV7670::Reg72_SCALING_DCWCTR,
		(0b0 << 7) |		// Vertical average calculation option
		(0b0 << 6) |		// Vertical down sampling option
		(0b10 << 4) |		// Vertical downsampling rate
		(0b0 << 3) |		// Horizontal average calculation option
		(0b0 << 2) |		// Horizontal down sampling option
		(0b10 << 0))		// Horizontal downsampling rate
	.WriteReg(Device::OV7670::Reg73_SCALING_PCLK_DIV,
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b010 << 0))		// Clock divider control for DSP scale control	
	.WriteReg(Device::OV7670::RegA2_SCALING_PCLK_DELAY,
		(0b0000010 << 0));	// Scaling output delay
	uint32_t hStart = 136, hStop = hStart + 640;
	ov7670
	.WriteReg(Device::OV7670::Reg17_HSTART,
		static_cast<uint8_t>(hStart >> 3))
	.WriteReg(Device::OV7670::Reg18_HSTOP,
		static_cast<uint8_t>(hStop >> 3))
	.WriteReg(Device::OV7670::Reg32_HREF,
		static_cast<uint8_t>((0b10 << 6) | ((hStop & 0b111) << 3) |((hStart & 0b111) << 0)));
	image.Allocate(Image::Format::RGB565, 160, 120);  // QQVGA size
	Display::Base& display = Display::GetInstance(0);
	while (true) {
		ov7670.Capture(image);
		display.DrawImage(
			(display.GetWidth() - image.GetWidth()) / 2,
			(display.GetHeight() - image.GetHeight()) / 2, image);
		::jxglib_tick();
	}
}
