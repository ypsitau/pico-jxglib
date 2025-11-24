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
	.WriteReg(Device::OV7670::Reg13_COM8,
		(0b1 << 7) |		// Enable fast AGC/AEC algorithm
		(0b1 << 6) |		// AEC - Step size limit
		(0b1 << 5) |		// Banding filter ON/OFF
		(0b1 << 2) |		// AGC Enable
		(0b0 << 1) |		// AWB Enable
		(0b1 << 0))			// AEC Enable
	.WriteReg(Device::OV7670::Reg00_GAIN,
		0x00)				// AGC - Gain control gain setting
	.WriteReg(Device::OV7670::Reg09_COM2,
		(0b0 << 4) |		// Soft sleep mode
		(0b00 << 0))		// Output drive capability
	.WriteReg(Device::OV7670::Reg0D_COM4,
		(0b00 << 4))		// Average option (must be same value as COM17[7:6])
	.WriteReg(Device::OV7670::Reg14_COM9,
		(0b010 << 4) |		// Automatic Gain Ceiling
		(0b0 << 0))			// Freeze AGC/AEC
	.WriteReg(Device::OV7670::RegA5_BD50MAX,
		0x05)				// 50Hz Banding Step Limit
	.WriteReg(Device::OV7670::RegAB_BD60MAX,
		0x07)				// 60Hz Banding Step Limit
	.WriteReg(Device::OV7670::Reg24_AEW,
		0x75)				// AGC/AEC - Stable Operating Region (Upper Limit)
	.WriteReg(Device::OV7670::Reg25_AEB,
		0x63)				// AGC/AEC - Stable Operating Region (Lower Limit)
	.WriteReg(Device::OV7670::Reg26_VPT,
		(0xa << 4) |		// AGC/AEC Fast Mode Operating Region - High nibble of upper limit of fast mode control zone
		(0x5 << 0))			// AGC/AEC Fast Mode Operating Region - High nibble of lower limit of fast mode control zone
	.WriteReg(Device::OV7670::Reg9F_HAECC1,
		0x78)				// Histogram-based AEC/AGC Control 1
	.WriteReg(Device::OV7670::RegA0_HAECC2,
		0x68)				// Histogram-based AEC/AGC Control 2
	.WriteReg(Device::OV7670::RegA6_HAECC3,
		0xdf)				// Histogram-based AEC/AGC Control 3
	.WriteReg(Device::OV7670::RegA7_HAECC4,
		0xdf)				// Histogram-based AEC/AGC Control 4
	.WriteReg(Device::OV7670::RegA8_HAECC5,
		0xf0)				// Histogram-based AEC/AGC Control 5
	.WriteReg(Device::OV7670::RegA9_HAECC6,
		0x90)				// Histogram-based AEC/AGC Control 6
	.WriteReg(Device::OV7670::RegAA_HAECC7,
		0x94)				// Histogram-based AEC/AGC Control 7
	.WriteReg(Device::OV7670::Reg0E_COM5,
		0x61)				// Reserved
	.WriteReg(Device::OV7670::Reg0F_COM6,
		(0b0 << 7) |		// Output of optical black line option
		(0b1 << 1))			// Reset all timing when format changes
	.WriteReg(Device::OV7670::Reg1E_MVFP,
		(0b1 << 5) |		// Mirror
		(0b0 << 4) |		// VFlip enable
		(0b1 << 2))			// Black sun enable
	.WriteReg(Device::OV7670::Reg20_ADCCTR0,
		(0b0 << 3) |		// ADC range adjustment
		(0b100 << 0))		// ADC reference adjustment
	.WriteReg(Device::OV7670::Reg33_CHLF,
		0x08)				// Array Current Contro (Reserved)
	.WriteReg(Device::OV7670::Reg37_ADC,
		0x3f)				// ADC Control (Reserved)
	.WriteReg(Device::OV7670::Reg38_ACOM,
		0x01)				// ADC and Analog Common Mode Control (Reserved)
	.WriteReg(Device::OV7670::Reg39_OFON,
		0x00)				// ADC Offset Control (Reserved)
	.WriteReg(Device::OV7670::Reg12_COM7,
		(0b1 << 5) |		// Output format - CIF selection
		(0b1 << 4) |		// Output format - QVGA selection
		(0b1 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0))			// Output format - Raw RGB
	.WriteReg(Device::OV7670::Reg69_GFIX,
		(0b01 << 6) |		// Fix gain for Gr Channel
		(0b01 << 4) |		// Fix gain for Gb Channel
		(0b11 << 2) |		// Fix gain for R Channel
		(0b01 << 0))		// Fix gain for B Channel
	.WriteReg(Device::OV7670::Reg74,
		(0b1 << 4) |		// DG_Manu
		(0b01 << 0))		// Digital gain manual control
	.WriteReg(Device::OV7670::Reg92_DM_LNL,
		0x00)				// Dummy Line low 8 bits
	.WriteReg(Device::OV7670::Reg93_DM_LNH,
		0x00)				// Dummy Line high 8 bits
	.WriteReg(Device::OV7670::RegB1_ABLC1,
		(0b1 << 2))			// ABLC enable
	.WriteReg(Device::OV7670::RegB3_THL_ST,
		0x82)				// ABLC Target
	.WriteReg(Device::OV7670::Reg43_AWBC1,
		0x14)				// Reserved
	.WriteReg(Device::OV7670::Reg44_AWBC2,
		0xf0)				// Reserved
	.WriteReg(Device::OV7670::Reg45_AWBC3,
		0x34)				// Reserved
	.WriteReg(Device::OV7670::Reg46_AWBC4,
		0x58)				// Reserved
	.WriteReg(Device::OV7670::Reg47_AWBC5,
		0x28)				// Reserved
	.WriteReg(Device::OV7670::Reg48_AWBC6,
		0x3a)				// Reserved
	.WriteReg(Device::OV7670::Reg62_LCC1,
		0x00)				// Lens Correction Option 1
	.WriteReg(Device::OV7670::Reg63_LCC2,
		0x00)				// Lens Correction Option 2
	.WriteReg(Device::OV7670::Reg64_LCC3,
		0x04)				// Lens Correction Option 3
	.WriteReg(Device::OV7670::Reg65_LCC4,
		0x20)				// Lens Correction Option 4
	.WriteReg(Device::OV7670::Reg66_LCC5,
		(0b1 << 2) |		// Lens Correction control select
		(0b1 << 0))			// Lens correction enable
	.WriteReg(Device::OV7670::Reg94_LCC6,
		0x04)				// Lens Correction Option 6
	.WriteReg(Device::OV7670::Reg95_LCC7,
		0x08)				// Lens Correction Option 7
	.WriteReg(Device::OV7670::Reg6C_AWBCTR3,
		0x0a)				// AWB Control 3
	.WriteReg(Device::OV7670::Reg6D_AWBCTR2,
		0x55)				// AWB Control 2
	.WriteReg(Device::OV7670::Reg6E_AWBCTR1,
		0x11)				// AWB Control 1
	.WriteReg(Device::OV7670::Reg6F_AWBCTR0,
		0x9f)				// AWB Control 0
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
	.WriteReg(Device::OV7670::Reg55_BRIGHT,
		0x00)				// Brightness control
	.WriteReg(Device::OV7670::Reg56_CONTRAS,
		0x40)				// Contrast control
	.WriteReg(Device::OV7670::Reg57_CONTRAS_CENTER,
		0x80)				// Contrast center control

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
