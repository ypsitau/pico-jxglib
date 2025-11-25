//==============================================================================
// OV7670.cpp
//==============================================================================
#include "jxglib/Device/OV7670.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
OV7670::OV7670(i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq) : i2c_{i2c}, pinAssign_{pinAssign}, freq_{freq}
{
}

OV7670& OV7670::WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t buf[2] = { reg, value };
	::i2c_write_blocking(i2c_, I2CAddr, buf, 2, false);
	return *this;
}

uint8_t OV7670::ReadReg(uint8_t reg)
{
	::i2c_write_blocking(i2c_, I2CAddr, &reg, 1, true);
	uint8_t value = 0;
	::i2c_read_blocking(i2c_, I2CAddr, &value, 1, false);
	return value;
}

void OV7670::Initialize()
{
	uint relAddrStart = 0;
#if 1
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.pull()
		.wait(1, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go low
	.wrap_target()
		.wait(1, "gpio", pinAssign_.HREF)	// wait for HREF to go high
		.mov("x", "osr")
	.L("pixel")
		.wait(0, "gpio", pinAssign_.PLK)	// wait for PLK to go low
		.wait(1, "gpio", pinAssign_.PLK)	// wait for PLK to go high
		.in("pins", 8)
		.jmp("x--", "pixel")				// loop for the number of pixels in the line
		.wait(0, "gpio", pinAssign_.HREF)	// wait for HREF to go low
	.wrap()
	.end();
#else
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.wait(1, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go low
	.wrap_target()
		.wait(1, "gpio", pinAssign_.HREF)	// wait for HREF to go high
	.L("pixel")
		.wait(0, "gpio", pinAssign_.PLK)	// wait for PLK to go low
		.wait(1, "gpio", pinAssign_.PLK)	// wait for PLK to go high
		.in("pins", 8)
		.jmp("pin", "pixel")				// loop while HREF is high
	.wrap()
	.end();
#endif
	//--------------------------------------------------------------------------
	sm_.set_program(program_)
		.reserve_in_pins(pinAssign_.DIN0, 8)
		.reserve_gpio_pin(pinAssign_.PLK, pinAssign_.HREF, pinAssign_.VSYNC)
		.config_set_in_shift_left(true, 32)	// shift left, autopush enabled, push threshold 32
		.config_set_jmp_pin(pinAssign_.HREF)
		.init();
	programToReset_
	.pio_version(0)
	.program("ov7670_reset")
		.jmp(sm_.offset + relAddrStart)		// jump to the main program start
	.end();
	pChannel_ = DMA::claim_unused_channel();
	channelConfig_.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(sm_.get_dreq_rx()) // set DREQ of StateMachine's rx
		.set_chain_to(*pChannel_)    // disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(true)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	PWM(pinAssign_.XLK).set_function().set_freq(freq_).set_chan_duty(.5).set_enabled(true);
}

void OV7670::SetupParam()
{
	WriteReg(Reg12_COM7,
		(0b1 << 7));	// SCCB Register Reset
	::sleep_ms(100);
	//-------------------------------------------------------------------------
	// Table 3-1. Scan Direction Control
	//-------------------------------------------------------------------------
	WriteReg(Reg1E_MVFP,
		(0b1 << 5) |		// Mirror
		(0b0 << 4) |		// VFlip enable
		(0b1 << 2));		// Black sun enable
	//-------------------------------------------------------------------------
	// Table 3-2. Clock Pre-Scaler Control
	//-------------------------------------------------------------------------
	WriteReg(Reg6B_DBLV,
		(0b01 << 6) |		// PLL control
							//  00: Bypass PLL
							//  01: PLL times the input clock by 4
							//  10: PLL times the input clock by 6
							//  11: PLL times the input clock by 8
		(0b0 << 4) |		// Regulator control
		(0b1010 << 0));		// (Reserved)
	//-------------------------------------------------------------------------
	// Table 2-2. Resolution Register Settings
	//-------------------------------------------------------------------------
#if 1
	WriteReg(Reg11_CLKRC,
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0));	// Internal clock prescaler
	WriteReg(Reg12_COM7,
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0));			// Output format - Raw RGB
	WriteReg(Reg0C_COM3,
		(0b0 << 6) |		// Output data MSB and LSB swap
		(0b0 << 5) |		// Tri-state option for output clock at power-down period
		(0b0 << 4) |		// Tri-state option for output data at power-down period
		(0b0 << 3) |		// Scale enable
		(0b1 << 2));			// DCW enable
	WriteReg(Reg3E_COM14,
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0));		// PCLK divider (only when COM14[4] = 1)
	WriteReg(Reg70_SCALING_XSC,
		(0b0 << 7) |		// Test pattern[0]
		(0b0111010 << 0));	// Horizontal scale factor
	WriteReg(Reg71_SCALING_YSC,
		(0b0 << 7) |		// Test pattern[1]
		(0b0110101 << 0));	// Vertical scale factor
	WriteReg(Reg72_SCALING_DCWCTR,
		(0b0 << 7) |		// Vertical average calculation option
		(0b0 << 6) |		// Vertical down sampling option
		(0b01 << 4) |		// Vertical downsampling rate
		(0b0 << 3) |		// Horizontal average calculation option
		(0b0 << 2) |		// Horizontal down sampling option
		(0b01 << 0));		// Horizontal downsampling rate
	WriteReg(Reg73_SCALING_PCLK_DIV,
		0xf0 |
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b001 << 0));		// Clock divider control for DSP scale control	
	WriteReg(RegA2_SCALING_PCLK_DELAY,
		(0b0000010 << 0));	// Scaling output delay
	WriteReg(Reg40_COM15,
		(0b11 < 6) |		// Data format - output full range enable
		(0b01 << 4));		// RGB 555/565 option
#else
	WriteReg(Reg11_CLKRC,
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0));	// Internal clock prescaler
	WriteReg(Reg12_COM7,
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0));			// Output format - Raw RGB
	WriteReg(Reg0C_COM3,
		(0b0 << 6) |		// Output data MSB and LSB swap
		(0b0 << 5) |		// Tri-state option for output clock at power-down period
		(0b0 << 4) |		// Tri-state option for output data at power-down period
		(0b0 << 3) |		// Scale enable
		(0b1 << 2));			// DCW enable
	WriteReg(Reg3E_COM14,
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b010 << 0));		// PCLK divider (only when COM14[4] = 1)
	WriteReg(Reg70_SCALING_XSC,
		(0b0 << 7) |		// Test pattern[0]
		(0b0111010 << 0));	// Horizontal scale factor
	WriteReg(Reg71_SCALING_YSC,
		(0b0 << 7) |		// Test pattern[1]
		(0b0110101 << 0));	// Vertical scale factor
	WriteReg(Reg72_SCALING_DCWCTR,
		(0b0 << 7) |		// Vertical average calculation option
		(0b0 << 6) |		// Vertical down sampling option
		(0b10 << 4) |		// Vertical downsampling rate
		(0b0 << 3) |		// Horizontal average calculation option
		(0b0 << 2) |		// Horizontal down sampling option
		(0b10 << 0));		// Horizontal downsampling rate
	WriteReg(Reg73_SCALING_PCLK_DIV,
		0xf0 |
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b010 << 0));		// Clock divider control for DSP scale control	
	WriteReg(RegA2_SCALING_PCLK_DELAY,
		(0b0000010 << 0));	// Scaling output delay
	WriteReg(Reg40_COM15,
		(0b11 < 6) |		// Data format - output full range enable
		(0b01 << 4));		// RGB 555/565 option
#endif
	uint32_t hStart = 136, hStop = hStart + 640;
	WriteReg(Reg17_HSTART,
		static_cast<uint8_t>(hStart >> 3));
	WriteReg(Reg18_HSTOP,
		static_cast<uint8_t>(hStop >> 3));
	WriteReg(Reg32_HREF,
		static_cast<uint8_t>((0b10 << 6) | ((hStop & 0b111) << 3) |((hStart & 0b111) << 0)));
	WriteReg(Reg3A_TSLB,
		(0b0 << 5) | 		// Negative image enable
		(0b0 << 4) |		// UV output value
		(0b0 << 3) |		// Output sequence
		(0b0 << 0));		// Auto output window
	//-------------------------------------------------------------------------
	// Table 3-4. Dummy Pixel and Row
	//-------------------------------------------------------------------------
	WriteReg(Reg2A_EXHCH,
		(0b0000 << 4) |		// 4 MSB for dummy pixel insert in horizontal direction
		(0b00 << 2) |		// HSYNC falling edge delay 2 MSB
		(0b00 << 0));		// HSYNC rising edge delay 2 MSB
	WriteReg(Reg2B_EXHCL,
		0x00);				// 8 LSB for dummy pixel insert in horizontal direction
	WriteReg(Reg93_DM_LNH,
		0x00);				// Dummy Line low 8 bits
	WriteReg(Reg92_DM_LNL,
		0x00);				// Dummy Line high 2 bits
	WriteReg(Reg4D_DM_POS,
		(0b0 << 0));		// Dummy row position
	WriteReg(Reg3B_COM11,
		(0b0 << 7) |		// Night mode
		(0b00 << 5) |		// Minimum frame rate of night mode
		(0b0 << 4) |		// D56_Auto
		(0b0 << 3) |		// Banding filter value select
		(0b0 << 1));		// Exposure timing can be less than limit of banding filter when light is too strong
	WriteReg(RegA4_NT_CTRL,
		(0b0 << 3) |		// Auto frame rate adjustment dummy row selection
		(0b00 << 0));		// Auto frame rate adjuestment switch point
	//-------------------------------------------------------------------------
	// Table 3-5. Exposure and Banding Filter
	//-------------------------------------------------------------------------
	WriteReg(Reg07_AECHH,
		0x00);				// 
	WriteReg(Reg10_AECH,
		0x00);				// 
	WriteReg(Reg04_COM1,
		(0b0 << 6) |		// CCIR656 format
		(0b00 << 0));		// AEC[1:0]
	WriteReg(Reg9D_BD50ST,
		0x00);				// 
	WriteReg(Reg9E_BD60ST,
		0x00);				// 
	WriteReg(Reg3B_COM11,
		0x00);				// 
	//-------------------------------------------------------------------------
	// Table 3-6. Exposure Control Mode
	//-------------------------------------------------------------------------
	WriteReg(Reg13_COM8,
		(0b1 << 7) |		// Enable fast AGC/AEC algorithm
		(0b0 << 6) |		// AEC - Step size limit
		(0b0 << 5) |		// Banding filter ON/OFF
		(0b01 << 3) |		// (reserved)
		(0b1 << 2) |		// AGC Enable
		(0b0 << 1) |		// AWB Enable
		(0b1 << 0));		// AEC Enable
	WriteReg(RegAA_NALG,
		0x00);				// 
	//-------------------------------------------------------------------------
	// Table 3-7. Average-based AEC/AGC Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg24_AEW,
		0x75);				// AGC/AEC - Stable Operating Region (Upper Limit)
	WriteReg(Reg25_AEB,
		0x63);				// AGC/AEC - Stable Operating Region (Lower Limit)
	WriteReg(Reg26_VPT,
		(0xa << 4) |		// AGC/AEC Fast Mode Operating Region - High nibble of upper limit of fast mode control zone
		(0x5 << 0));		// AGC/AEC Fast Mode Operating Region - High nibble of lower limit of fast mode control zone
	WriteReg(Reg0D_COM4,
		(0b00 << 4));		// Average option (must be same value as COM17[7:6])
	//-------------------------------------------------------------------------
	// Table 3-8. Histogram-based AEC Related Registers
	//-------------------------------------------------------------------------
	//WriteReg(RegA0_LRL,
	//	0x00);

	//-------------------------------------------------------------------------
	// Table 3-10. Sync Signal Related Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg15_COM10,
		(0b0 << 6) |		// HREF changes to HSYNC
		(0b0 << 5) |		// PCLK output option
		(0b0 << 4) |		// PCLK reverse
		(0b0 << 3) |		// HREF reverse
		(0b0 << 2) |		// VSYNC option
		(0b1 << 1) |		// VSYNC negative
		(0b0 << 0));		// HSYNC negative

	//-------------------------------------------------------------------------
	// Table 3-11. Flashlight Modes
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 3-12. Xenon Flash Pulse Width Control
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 4-2. AGC General Controls
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 4-3. ABLC Control Registers
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 5-1. White Balance Control Registers
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 5-2. AWB Control Registers
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Table 5-3. Gamma Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg7B_GAM1,
		4);				// Gamma curve 1st Segment Input End Point 0x04 Output Value
	WriteReg(Reg7C_GAM2,
		8);				// Gamma curve 2nd Segment Input End Point 0x08 Output Value
	WriteReg(Reg7D_GAM3,
		16);				// Gamma curve 3rd Segment Input End Point 0x10 Output Value
	WriteReg(Reg7E_GAM4,
		32);				// Gamma curve 4th Segment Input End Point 0x20 Output Value
	WriteReg(Reg7F_GAM5,
		40);				// Gamma curve 5th Segment Input End Point 0x28 Output Value
	WriteReg(Reg80_GAM6,
		48);				// Gamma curve 6th Segment Input End Point 0x30 Output Value
	WriteReg(Reg81_GAM7,
		56);				// Gamma curve 7th Segment Input End Point 0x38 Output Value
	WriteReg(Reg82_GAM8,
		64);				// Gamma curve 8th Segment Input End Point 0x40 Output Value
	WriteReg(Reg83_GAM9,
		72);				// Gamma curve 9th Segment Input End Point 0x48 Output Value
	WriteReg(Reg84_GAM10,
		80);				// Gamma curve 10th Segment Input End Point 0x50 Output Value
	WriteReg(Reg85_GAM11,
		96);				// Gamma curve 11th Segment Input End Point 0x60 Output Value
	WriteReg(Reg86_GAM12,
		112);				// Gamma curve 12th Segment Input End Point 0x70 Output Value
	WriteReg(Reg87_GAM13,
		144);				// Gamma curve 13th Segment Input End Point 0x90 Output Value
	WriteReg(Reg88_GAM14,
		176);				// Gamma curve 14th Segment Input End Point 0xB0 Output Value
	WriteReg(Reg89_GAM15,
		208);				// Gamma curve 15th Segment Input End Point 0xD0 Output Value
	WriteReg(Reg7A_SLOP,
		(256 - 208) * 40 / 30);	// Gamma curve highest segment slope
	//-------------------------------------------------------------------------
	// Table 5-4. Color Matrix Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg4F_MTX1,
		0x40);				// Matrix Coefficient 1
	WriteReg(Reg50_MTX2,
		0x34);				// Matrix Coefficient 2
	WriteReg(Reg51_MTX3,
		0x0c);				// Matrix Coefficient 3
	WriteReg(Reg52_MTX4,
		0x17);				// Matrix Coefficient 4
	WriteReg(Reg53_MTX5,
		0x29);				// Matrix Coefficient 5
	WriteReg(Reg54_MTX6,
		0x40);				// Matrix Coefficient 6
	WriteReg(Reg58_MTXS,
		(0b0 << 7) |		// Auto contrast center enable
		(0b011110 << 0));	// Matrix coeffieicnt sign
	WriteReg(Reg41_COM16,
		(0b0 << 5) |		// Enable edge enhancement threshold auto-adjustment (see Table 5-5)
		(0b0 << 4) |		// De-noise threshold auto-adjustment (see Table 5-6)
		(0b0 << 3) |		// AWB gain enable
		(0b0 << 1));		// Color matrix coefficient double option
	//-------------------------------------------------------------------------
	// Table 5-5. Sharpness Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg3F_EDGE,
		(0b00000 << 0));	// Edge enhancement factor
	WriteReg(Reg75,
		(0b00101 << 0));	// Edge enhancement lower limit
	WriteReg(Reg76,
		(0b1 << 7) |		// Black pixel correction enable
		(0b1 << 6) |		// White pixel correction enable
		(0b00001 << 0));	// Edge enhancement upper limit
	//-------------------------------------------------------------------------
	// Table 5-6. De-Noise Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg4C_DNSTH,
		0x00);				// De-noise Strength
	WriteReg(Reg77,
		0x01);				// De-noise offset
	//-------------------------------------------------------------------------
	// Table 5-7. Auto Color Saturation Adjustment Related Registers
	//-------------------------------------------------------------------------
	WriteReg(RegC9_SATCTR,
		0x60);

	//-------------------------------------------------------------------------
#if 1
	WriteReg(Reg00_GAIN,
		0x00);				// AGC - Gain control gain setting
	WriteReg(Reg09_COM2,
		(0b0 << 4) |		// Soft sleep mode
		(0b00 << 0));		// Output drive capability
	WriteReg(Reg14_COM9,
		(0b010 << 4) |		// Automatic Gain Ceiling
		(0b0 << 0));			// Freeze AGC/AEC
	WriteReg(RegA5_BD50MAX,
		0x05);				// 50Hz Banding Step Limit
	WriteReg(RegAB_BD60MAX,
		0x07);				// 60Hz Banding Step Limit
	WriteReg(Reg9F_HAECC1,
		0x78);				// Histogram-based AEC/AGC Control 1
	WriteReg(RegA0_HAECC2,
		0x68);				// Histogram-based AEC/AGC Control 2
	WriteReg(RegA6_HAECC3,
		0xdf);				// Histogram-based AEC/AGC Control 3
	WriteReg(RegA7_HAECC4,
		0xdf);				// Histogram-based AEC/AGC Control 4
	WriteReg(RegA8_HAECC5,
		0xf0);				// Histogram-based AEC/AGC Control 5
	WriteReg(RegA9_HAECC6,
		0x90);				// Histogram-based AEC/AGC Control 6
	//WriteReg(RegAA_HAECC7,
	//	0x94);				// Histogram-based AEC/AGC Control 7
	WriteReg(Reg0E_COM5,
		0x61);				// Reserved
	WriteReg(Reg0F_COM6,
		(0b0 << 7) |		// Output of optical black line option
		(0b1 << 1));			// Reset all timing when format changes
	WriteReg(Reg20_ADCCTR0,
		(0b0 << 3) |		// ADC range adjustment
		(0b100 << 0));		// ADC reference adjustment
	WriteReg(Reg33_CHLF,
		0x08);				// Array Current Contro (Reserved)
	WriteReg(Reg37_ADC,
		0x3f);				// ADC Control (Reserved)
	WriteReg(Reg38_ACOM,
		0x01);				// ADC and Analog Common Mode Control (Reserved)
	WriteReg(Reg39_OFON,
		0x00);				// ADC Offset Control (Reserved)
	WriteReg(Reg69_GFIX,
		(0b01 << 6) |		// Fix gain for Gr Channel
		(0b01 << 4) |		// Fix gain for Gb Channel
		(0b11 << 2) |		// Fix gain for R Channel
		(0b01 << 0));		// Fix gain for B Channel
	WriteReg(Reg74,
		(0b1 << 4) |		// DG_Manu
		(0b01 << 0));		// Digital gain manual control
	WriteReg(Reg92_DM_LNL,
		0x00);				// Dummy Line low 8 bits
	WriteReg(Reg93_DM_LNH,
		0x00);				// Dummy Line high 8 bits
	WriteReg(RegB1_ABLC1,
		(0b1 << 2));			// ABLC enable
	WriteReg(RegB3_THL_ST,
		0x82);				// ABLC Target
	WriteReg(Reg43_AWBC1,
		0x14);				// Reserved
	WriteReg(Reg44_AWBC2,
		0xf0);				// Reserved
	WriteReg(Reg45_AWBC3,
		0x34);				// Reserved
	WriteReg(Reg46_AWBC4,
		0x58);				// Reserved
	WriteReg(Reg47_AWBC5,
		0x28);				// Reserved
	WriteReg(Reg48_AWBC6,
		0x3a);				// Reserved
	WriteReg(Reg62_LCC1,
		0x00);				// Lens Correction Option 1
	WriteReg(Reg63_LCC2,
		0x00);				// Lens Correction Option 2
	WriteReg(Reg64_LCC3,
		0x04);				// Lens Correction Option 3
	WriteReg(Reg65_LCC4,
		0x20);				// Lens Correction Option 4
	WriteReg(Reg66_LCC5,
		(0b1 << 2) |		// Lens Correction control select
		(0b1 << 0));			// Lens correction enable
	WriteReg(Reg94_LCC6,
		0x04);				// Lens Correction Option 6
	WriteReg(Reg95_LCC7,
		0x08);				// Lens Correction Option 7
	WriteReg(Reg6C_AWBCTR3,
		0x0a);				// AWB Control 3
	WriteReg(Reg6D_AWBCTR2,
		0x55);				// AWB Control 2
	WriteReg(Reg6E_AWBCTR1,
		0x11);				// AWB Control 1
	WriteReg(Reg6F_AWBCTR0,
		0x9f);				// AWB Control 0
	WriteReg(Reg55_BRIGHT,
		0x00);				// Brightness control
	WriteReg(Reg56_CONTRAS,
		0x40);				// Contrast control
	WriteReg(Reg57_CONTRAS_CENTER,
		0x80);				// Contrast center control
	WriteReg(Reg8C_RGB444,
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
		(0b0 << 0));			// RGB444 word format
	WriteReg(Reg3D_COM13,
		(0b1 << 7) |		// Gamma enable
		(0b1 << 6) |		// UV saturation level - UV auto-adjustment
		(0b1 << 0));			// UV swap
	WriteReg(Reg4B_REG4B,
		(0b1 << 0));			// UV average enable
	WriteReg(Reg41_COM16,
		(0b1 << 5) |		// Enable edge enhancement threshold auto-adjustment for YUV output
		(0b1 << 4) |		// De-noise threshold auto-adjustment
		(0b1 << 3) |		// AWB gain enable
		(0b0 << 1));			// Color matrix coefficient double option
#endif
}

OV7670& OV7670::Capture(Image& image)
{
	sm_.set_enabled(false);
	sm_.clear_fifos().exec(programToReset_);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(image.GetPointer())
		.set_trans_count_trig(image.GetBytesBuff() / sizeof(uint32_t));
	sm_.set_enabled();
	sm_.put(image.GetWidth() * 2 - 1);
	while (pChannel_->is_busy()) Tickable::Tick();
	return *this;
}

}
