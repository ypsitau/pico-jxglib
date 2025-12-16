//==============================================================================
// OV7670.cpp
//==============================================================================
#include "jxglib/Camera/OV7670.h"

namespace jxglib::Camera {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
// Table 2-2. Resolution Register Settings
const OV7670::ResolutionSetting OV7670::resolutionSetting_VGA {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b0 << 3) |		// Scale enable
		(0b0 << 2),			// DCW enable
	Reg3E_COM14:
		(0b0 << 4) |		// DCW and scaling PCLK enable
		(0b0 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b000 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b01 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b000 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(2 << 0),			// Scaling output delay (0-127)
	hStart: 162,
	vStart: 2,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_QVGA {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b0 << 3) |		// Scale enable
		(0b1 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b01 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b001 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(2 << 0),			// Scaling output delay (0-127)
	hStart: 174,
	vStart: 10,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_QQVGA {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b0 << 3) |		// Scale enable
		(0b1 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b010 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b10 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b10 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b010 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(2 << 0),			// Scaling output delay (0-127)
	hStart: 186,
	vStart: 11,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_QQQVGA {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b0 << 3) |		// Scale enable
		(0b1 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b011 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b11 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b11 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b011 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(2 << 0),			// Scaling output delay (0-127)
	hStart: 210,
	vStart: 12,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_CIF {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b1 << 3) |		// Scale enable
		(0b0 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b01 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b001 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(2 << 0),			// Scaling output delay (0-127)
	hStart: 136,
	vStart: 12,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_QCIF {
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b1 << 3) |		// Scale enable
		(0b1 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b01 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b001 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(82 << 0),			// Scaling output delay (0-127)
	hStart: 136,
	vStart: 12,
};

const OV7670::ResolutionSetting OV7670::resolutionSetting_QQCIF{
	Reg11_CLKRC:
		(0b0 << 6) |		// Use external clock directly (no clock pre-scale available)
		(0b000001 << 0),	// Internal clock prescaler
	Reg12_COM7:
		(0b0 << 5) |		// Output format - CIF selection
		(0b0 << 4) |		// Output format - QVGA selection
		(0b0 << 3) |		// Output format - QCIF selection
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 1) |		// Color bar
		(0b0 << 0),			// Output format - Raw RGB
	Reg0C_COM3:
		(0b1 << 3) |		// Scale enable
		(0b1 << 2),			// DCW enable
	Reg3E_COM14:
		(0b1 << 4) |		// DCW and scaling PCLK enable
		(0b1 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b010 << 0),		// PCLK divider (only when COM14[4] = 1)
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	Reg70_SCALING_XSC:
		(0x3a << 0),		// Horizontal scale factor (0-127)
	Reg71_SCALING_YSC:
		(0x35 << 0),		// Vertical scale factor (0-127)
	Reg72_SCALING_DCWCTR:
		(0b10 << 4) |		// Vertical downsampling rate
							//  00: No vertical down samling
							//  01: Vertical down sample by 2
							//  10: Vertical down sample by 4
							//  11: Vertical down sample by 8
		(0b10 << 0),		// Horizontal downsampling rate
							//  00: No horizontal down samling
							//  01: Horizontal down sample by 2
							//  10: Horizontal down sample by 4
							//  11: Horizontal down sample by 8
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
							//  0: Enable clock divider
							//  1: Bypass clock divider
		(0b010 << 0),		// Clock divider control for DSP scale control	
							//  000: Divided by 1
							//  001: Divided by 2
							//  010: Divided by 4
							//  011: Divided by 8
							//  100: Divided by 16
	RegA2_SCALING_PCLK_DELAY:
		(42 << 0),			// Scaling output delay (0-127)
	hStart: 136,
	vStart: 12,
};

// Table 2-1. OV7670/OV7171 Output Formats
const OV7670::FormatSetting OV7670::formatSetting_RawBayerRGB {
	Reg12_COM7:
		(0b0 << 2) |		// Output format - RGB selection
		(0b1 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b00 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_ProcessedBayerRGB {
	Reg12_COM7:
		(0b1 << 2) |		// Output format - RGB selection
		(0b1 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b00 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_YUV422 {
	Reg12_COM7:
		(0b0 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b00 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b1 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b1 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_GRB422 {
	Reg12_COM7:
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b00 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_RGB565 {
	Reg12_COM7:
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b01 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_RGB555 {
	Reg12_COM7:
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b11 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b0 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

const OV7670::FormatSetting OV7670::formatSetting_RGB444 {
	Reg12_COM7:
		(0b1 << 2) |		// Output format - RGB selection
		(0b0 << 0),			// Output format - Raw RGB
	Reg40_COM15:
		(0b01 << 4),		// RGB 555/565 option
							//  0x: Normal RGB output
							//  01: RGB 565
							//  11: RGB 555
	Reg8C_RGB444:
		(0b1 << 1) |		// RGB444 enable, effective only when COM15[4] is high
							//  0: Disable
							//  1: Enable
		(0b0 << 0),			// RGB444 word format
							//  0: xR GB
							//  1: RG Bx
	Reg3A_TSLB:
		(0b0 << 3),			// Output sequence
							//  0: Y first
							//  1: UV first
	Reg3D_COM13:
		(0b0 << 0),			// UV swap
							// when TSLB[3] = 0
							//  0: Y U Y V
							//  1: Y V Y U
							// when TSLB[3] = 1
							//  0: U Y V Y
							//  1: V Y U Y
};

OV7670::OV7670(i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq) :
	i2c_{i2c}, pinAssign_{pinAssign}, freq_{freq}, updateResolutionAndFormatFlag_{true}
{
	format_ = Format::RGB565;
	resolution_ = Resolution::QVGA;
	UpdateSize();
}

OV7670& OV7670::WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t buf[2] = { reg, value };
	::i2c_write_blocking(i2c_, I2CAddr, buf, 2, false);
	return *this;
}

OV7670& OV7670::WriteReg(uint8_t reg, uint8_t mask, uint8_t value)
{
	uint8_t valueCur = ReadReg(reg);
	valueCur = (valueCur & ~mask) | (value & mask);
	return WriteReg(reg, valueCur);
}

uint8_t OV7670::ReadReg(uint8_t reg)
{
	::i2c_write_blocking(i2c_, I2CAddr, &reg, 1, false);
	uint8_t value = 0;
	::i2c_read_blocking(i2c_, I2CAddr, &value, 1, false);
	return value;
}

void OV7670::ReadRegs(uint8_t reg, uint8_t values[], int count)
{
	::i2c_write_blocking(i2c_, I2CAddr, &reg, 1, false);
	::i2c_read_blocking(i2c_, I2CAddr, values, count, false);
}

Base& OV7670::SetResolution(Resolution resolution)
{
	image_.Free();
	Base::SetResolution(resolution);
	updateResolutionAndFormatFlag_ = true;
	return *this;
}

Base& OV7670::SetFormat(Format format)
{
	Base::SetFormat(format);
	updateResolutionAndFormatFlag_ = true;
	return *this;
}

OV7670& OV7670::SetTestPattern(TestPattern testPattern)
{
	uint8_t value =
		(testPattern == TestPattern::Off) ? 0b00 :
		(testPattern == TestPattern::Shifting1s) ? 0b01 :
		(testPattern == TestPattern::EightBars) ? 0b10 :
		(testPattern == TestPattern::FadeToGray) ? 0b11 : 0b00;
	WriteRegBit(Reg71_SCALING_YSC, 7, value & 0b10);
	WriteRegBit(Reg70_SCALING_XSC, 7, value & 0b01);
	return *this;
}

OV7670::TestPattern OV7670::GetTestPattern()
{
	uint8_t value = (ReadRegBit(Reg71_SCALING_YSC, 7)? 0b10 : 0b00) | (ReadRegBit(Reg70_SCALING_XSC, 7)? 0b01 : 0b00);
	return
		(value == 0b00) ? TestPattern::Off :
		(value == 0b01) ? TestPattern::Shifting1s :
		(value == 0b10) ? TestPattern::EightBars :
		(value == 0b11) ? TestPattern::FadeToGray : TestPattern::Off;
}

bool OV7670::Initialize()
{
	if (format_ == Format::RawBayerRGB) {
		if (resolution_ != Resolution::VGA) {
			// Raw Bayer RGB only supports VGA resolution
			return false;
		}
	} else if (format_ == Format::ProcessedBayerRGB) {
		if (resolution_ != Resolution::VGA && resolution_ != Resolution::QVGA) {
			// Processed Bayer RGB only supports VGA resolution
			return false;
		}
	}
	bool hrefFlag = false;
	uint relAddrStart = 0;
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.pull()
		.wait(1, "gpio", pinAssign_.VSYNC)			// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)			// wait for VSYNC to go low
	.wrap_target()
		.wait(0, "gpio", pinAssign_.HREF)			// wait for HREF to go low
		.wait(1, "gpio", pinAssign_.HREF)			// wait for HREF to go high
		.mov("x", "osr")
	.L("loop_pixel")
		.wait(0, "gpio", pinAssign_.PCLK)			// wait for PCLK to go low
		.wait(1, "gpio", pinAssign_.PCLK)			// wait for PCLK to go high
		.in("pins", 8)								// read 8 bits from data pins
		.jmp(hrefFlag? "pin" : "x--", "loop_pixel")	// loop while HREF or for the specified number of pixels
	.wrap()
	.end();
	//--------------------------------------------------------------------------
	sm_.set_program(program_)
		.reserve_in_pins(pinAssign_.D0, 8)
		.reserve_gpio_pin(pinAssign_.PCLK, pinAssign_.HREF, pinAssign_.VSYNC)
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
		.set_dreq(sm_.get_dreq_rx())		// set DREQ of StateMachine's rx
		.set_chain_to(*pChannel_)			// disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(true)					// byte swap: b0 b1 b2 b3 -> b3 b2 b1 b0
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	PWM(pinAssign_.XCLK).set_function().set_freq(freq_).set_chan_duty(.5).set_enabled(true);
	return true;
}

OV7670& OV7670::SetupReg()
{
	uint32_t hStart = 136, hStop = hStart + 640;
	uint32_t vStart = 12, vStop = vStart + 480;
	ResetAllReg();
	//-------------------------------------------------------------------------
	// Table 3-1. Scan Direction Control
	//-------------------------------------------------------------------------
	WriteReg(Reg1E_MVFP,
		(0b0 << 5) |		// Mirror
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
	// Table 3-4. Dummy Pixel and Row
	//-------------------------------------------------------------------------
	WriteReg(Reg2A_EXHCH,
		(0b0000 << 4) |		// 4 MSB for dummy pixel insert in horizontal direction
		(0b00 << 2) |		// HSYNC falling edge delay 2 MSB
		(0b00 << 0));		// HSYNC rising edge delay 2 MSB
	WriteReg(Reg2B_EXHCL,
		0x00);				// 8 LSB for dummy pixel insert in horizontal direction
	WriteReg(Reg92_DM_LNL,
		0x00);				// Dummy Line low 8 bits
	WriteReg(Reg93_DM_LNH,
		0x00);				// Dummy Line high 8 bits
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
	//WriteReg(OV7670_REG_COM11, (1 << 3)); // 50Hz
	//	//{0x9D, 99); // Banding filter for 50 Hz at 15.625 MHz
	//WriteReg(0x9D, 89); // Banding filter for 50 Hz at 13.888 MHz
	//WriteReg(OV7670_REG_BD50MAX, 0x05);
	//WriteReg(OV7670_REG_BD60MAX, 0x07);
	WriteReg(Reg07_AECHH,
		0x00);				// Exposure time [15:10]
	WriteReg(Reg10_AECH,
		0x00);				// Exposure time [9:2]
	WriteReg(Reg04_COM1,
		(0b0 << 6) |		// CCIR656 format
		(0b00 << 0));		// Exposure time [1:0]
	WriteReg(Reg9D_BD50ST,
		0x99);				// 
	WriteReg(Reg9E_BD60ST,
		0x7f);				// 
	WriteReg(RegA5_BD50MAX,
		5);					// 50Hz Banding Step Limit (0-255)
	WriteReg(RegAB_BD60MAX,
		7);					// 60Hz Banding Step Limit (0-255)
	WriteReg(Reg3B_COM11,
		(1 << 3));			// 
	//-------------------------------------------------------------------------
	// Table 3-6. Exposure Control Mode
	//-------------------------------------------------------------------------
	WriteReg(Reg13_COM8,
		(0b1 << 7) |		// Enable fast AGC/AEC algorithm
							//  0: Normal speed
							//  1: Fast speed
		(0b1 << 6) |		// AEC - Step size limit
		(0b1 << 5) |		// Banding filter ON/OFF (see Table 3-5)
		(0b01 << 3) |		// (reserved)
		(0b1 << 2) |		// AGC Enable (see Table 4-2)
							//  0: Disable AGC function, gain control function is still active
							//  1: Enable AGC function
		(0b1 << 1) |		// AWB Enable (see Table 5-1)
							//  0: Disable AWB, White Balance is in manual mode
							//  1: Enable AWB, White Balance is auto mode
		(0b1 << 0));		// AEC Enable (see Table 3-6)
							//  0: Disable AEC, Exposure is in manual mode
							//  1: Enable AEC, Exposure is auto mode
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
							//  00: Full window
							//  01: 1/2 window
							//  10: 1/4 window
							//  11: 1/4 window
	WriteReg(Reg42_COM17,
		(0b00 << 6) |		// AEC window must be the same value as COM4[5:4]
							//  00: Normal
							//  01: 1/2
							//  10: 1/4
							//  11: 1/4
		(0b0 << 3));		// DSP color bar enable
							//  0: Disable
							//  1: Enable
	//-------------------------------------------------------------------------
	// Table 3-8. Histogram-based AEC Related Registers
	//-------------------------------------------------------------------------
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
	WriteReg(RegAA_HAECC7,
		0x94);				// Histogram-based AEC/AGC Control 7
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
	WriteReg(Reg1B_PSHFT,
		0x00);				// Data Format - Pixel Delay Select
	WriteReg(Reg2A_EXHCH,
		(0b0000 << 4) |		// 4 MSB for dummy pixel insert in horizontal direction
		(0b00 << 2) |		// HSYNC falling edge delay 2 MSB
		(0b00 << 0));		// HSYNC rising edge delay 2 MSB
	//-------------------------------------------------------------------------
	// Table 3-11. Flashlight Modes
	// Table 3-12. Xenon Flash Pulse Width Control
	//-------------------------------------------------------------------------
	WriteReg(RegAC_STR_OPT,
		(0b0 << 7) |		// Strobe enable
		(0b0 << 6) |		// R/G/B gain controlled by STR_R/STR_G/STR_B for LED output frame
		(0b00 << 4) |		// Xenon mode option
							//  00: 1 row
							//  01: 2 rows
							//  10: 3 rows
							//  11: 4 rows
		(0b00 << 0));		// Mode select
							//  00: Xenon
							//  01: LED 1
							//  1x: LED 2
	WriteReg(RegAD_STR_R,
		0x00);				// R Gain for LED Output Frame
	WriteReg(RegAE_STR_G,
		0x00);				// G Gain for LED Output Frame
	WriteReg(RegAF_STR_B,
		0x00);				// B Gain for LED Output Frame
	//-------------------------------------------------------------------------
	// Table 4-1. Total Gain to Control Bit Correlation
	//-------------------------------------------------------------------------
	WriteReg(Reg00_GAIN,	// AGC - Gain control gain setting
		0x00);				// AGC[7:0]
	WriteReg(Reg03_VREF,
		(0b00 << 6) |		// AGC[9:8]
		((vStop & 0b11) << 2) |	// VREF end low 2 bits (high 8 bits at VSTOP[7:0])
		((vStart & 0b11) << 0));// VREF start low 2 bits (high 8 bits at VSTART[7:0])
	//-------------------------------------------------------------------------
	// Table 4-2. AGC General Controls
	//-------------------------------------------------------------------------
	WriteReg(Reg14_COM9,
		(0b010 << 4) |		// Automatic Gain Ceiling
							//  000: 2x
							//  001: 4x
							//  010: 8x
							//  011: 16x
							//  100: 32x
							//  101: 64x
							//  110: 128x
							//  111: Not allowedReserved
		(0b0 << 0));		// Freeze AGC/AEC
	//-------------------------------------------------------------------------
	// Table 4-3. ABLC Control Registers
	//-------------------------------------------------------------------------
	WriteReg(RegB1_ABLC1,
		(0b1 << 2));		// ABLC enable
	WriteReg(RegB3_THL_ST,
		0x82);				// ABLC Target
	WriteReg(RegB5_THL_DLT,
		0x20);				// ABLC Stable Range
	WriteReg(RegBE_AD_CHB,
		(0b0 << 6) |		// Sign bit
		(0 << 0));			// Blue channel black level compensation
	WriteReg(RegBF_AD_CHR,
		(0b0 << 6) |		// Sign bit
		(0 << 0));			// Red channel black level compensation
	WriteReg(RegC0_AD_CHGB,
		(0b0 << 6) |		// Sign bit
		(0 << 0));			// Gb channel black level compensation
	WriteReg(RegC1_AD_CHGR,
		(0b0 << 6) |		// Sign bit
		(0 << 0));			// Gr channel black level compensation
	WriteReg(Reg0F_COM6,
		(0b0 << 7) |		// Output of optical black line option
		(0b1 << 1));		// Reset all timing when format changes
	//-------------------------------------------------------------------------
	// Table 5-1. White Balance Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg6C_AWBCTR3,
		0x0a);				// AWB Control 3 (magic number)
	WriteReg(Reg6D_AWBCTR2,
		0x55);				// AWB Control 2 (magic number)
	WriteReg(Reg6E_AWBCTR1,
		0x11);				// AWB Control 1 (magic number)
	WriteReg(Reg6F_AWBCTR0,
		0x90 |
		(0b1 << 3) |		// 0: AWB Adjusts R and B gain only
							// 1: AWB Adjusts R, G, and B gains
		(0b0 << 2) |		// 0: Maximum color gain is 2x
							// 1: Maximum color gain is 4x
		(0b1 << 1) |		// 
		(0b0 << 0));		// 0: Advanced AWB mode
							// 1: Normal AWB mode
	WriteReg(Reg01_BLUE,
		0x80);				// AWB - Blue channel gain setting
	WriteReg(Reg02_RED,
		0x80);				// AWB - Red channel gain setting
	WriteReg(Reg6A_GGAIN,
		0x00);				// AWB - Green channel gain setting
	WriteReg(Reg5F_B_LMT,	// AWB Blue Gain Range
		(15 << 4) |			// Max Blue gain (0-15)
		(0 << 0));			// Min Blue gain (0-15)
	WriteReg(Reg60_R_LMT,	// AWB Red Gain Range
		(15 << 4) |			// Max Red gain (0-15)
		(0 << 0));			// Min Red gain (0-15)
	WriteReg(Reg61_G_LMT,	// AWB Green Gain Range
		(15 << 4) |			// Max Green gain (0-15)
		(0 << 0));			// Min Green gain (0-15)
	WriteReg(Reg69_GFIX,
		(0b01 < 6) |		// Fix gain for Gr channel
							//  00: 1x
							//  01: 1.25x
							//  10: 1.5x
							//  11: 1.75x
		(0b01 < 6) |		// Fix gain for Gb channel
							//  00: 1x
							//  01: 1.25x
							//  10: 1.5x
							//  11: 1.75x
		(0b11 < 6) |		// Fix gain for Red channel
							//  00: 1x
							//  01: 1.25x
							//  10: 1.5x
							//  11: 1.75x
		(0b01 < 6));		// Fix gain for Blue channel
							//  00: 1x
							//  01: 1.25x
							//  10: 1.5x
							//  11: 1.75x
	//-------------------------------------------------------------------------
	// Table 5-2. AWB Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg43_AWBC1,
		0x14);				// Reserved
	WriteReg(Reg44_AWBC2,
		0xf0);				// Reserved
	WriteReg(Reg45_AWBC3,
		0x45);				// Reserved
	WriteReg(Reg46_AWBC4,
		0x61);				// Reserved
	WriteReg(Reg47_AWBC5,
		0x51);				// Reserved
	WriteReg(Reg48_AWBC6,
		0x79);				// Reserved
	WriteReg(Reg59_AWBC7,
		0x00);
	WriteReg(Reg5A_AWBC8,
		0x00);
	WriteReg(Reg5B_AWBC9,
		0x01);
	WriteReg(Reg5C_AWBC10,
		0x00);
	WriteReg(Reg5D_AWBC11,
		0x00);
	WriteReg(Reg5E_AWBC12,
		0x00);
	//-------------------------------------------------------------------------
	// Table 5-3. Gamma Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg7A_SLOP,
		0x20);				// Gamma curve highest segment slope
	WriteReg(Reg7B_GAM1,
		0x1c);				// Gamma curve 1st Segment Input End Point 0x04 Output Value
	WriteReg(Reg7C_GAM2,
		0x28);				// Gamma curve 2nd Segment Input End Point 0x08 Output Value
	WriteReg(Reg7D_GAM3,
		0x3c);				// Gamma curve 3rd Segment Input End Point 0x10 Output Value
	WriteReg(Reg7E_GAM4,
		0x55);				// Gamma curve 4th Segment Input End Point 0x20 Output Value
	WriteReg(Reg7F_GAM5,
		0x68);				// Gamma curve 5th Segment Input End Point 0x28 Output Value
	WriteReg(Reg80_GAM6,
		0x76);				// Gamma curve 6th Segment Input End Point 0x30 Output Value
	WriteReg(Reg81_GAM7,
		0x80);				// Gamma curve 7th Segment Input End Point 0x38 Output Value
	WriteReg(Reg82_GAM8,
		0x88);				// Gamma curve 8th Segment Input End Point 0x40 Output Value
	WriteReg(Reg83_GAM9,
		0x8f);				// Gamma curve 9th Segment Input End Point 0x48 Output Value
	WriteReg(Reg84_GAM10,
		0x96);				// Gamma curve 10th Segment Input End Point 0x50 Output Value
	WriteReg(Reg85_GAM11,
		0xa3);				// Gamma curve 11th Segment Input End Point 0x60 Output Value
	WriteReg(Reg86_GAM12,
		0xaf);				// Gamma curve 12th Segment Input End Point 0x70 Output Value
	WriteReg(Reg87_GAM13,
		0xc4);				// Gamma curve 13th Segment Input End Point 0x90 Output Value
	WriteReg(Reg88_GAM14,
		0xd7);				// Gamma curve 14th Segment Input End Point 0xB0 Output Value
	WriteReg(Reg89_GAM15,
		0xe8);				// Gamma curve 15th Segment Input End Point 0xD0 Output Value
	WriteReg(Reg4B,
		(0b1 << 0));		// UV average enable
	//-------------------------------------------------------------------------
	// Table 5-4. Color Matrix Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg4F_MTX1,
		0x80);				// Matrix Coefficient 1
	WriteReg(Reg50_MTX2,
		0x80);				// Matrix Coefficient 2
	WriteReg(Reg51_MTX3,
		0x00);				// Matrix Coefficient 3
	WriteReg(Reg52_MTX4,
		0x22);				// Matrix Coefficient 4
	WriteReg(Reg53_MTX5,
		0x5e);				// Matrix Coefficient 5
	WriteReg(Reg54_MTX6,
		0x80);				// Matrix Coefficient 6
	WriteReg(Reg58_MTXS,
		(0b0 << 7) |		// Auto contrast center enable (see Table 5-9)
							//  0: Center luminance leve is set manually using register CONTRAS_CENTER (0x57)
							//  1: Center luminance leve is adjusted automatically and saved in register CONTRAS_CENTER (0x57)
		(0b011110 << 0));	// Matrix coeffieicnt sign
	WriteReg(Reg41_COM16,
		(0b0 << 5) |		// Enable edge enhancement threshold auto-adjustment (see Table 5-5)
							//  0: Manual mode, sharpness is set by register EDGE (0x3F)
							//  1: Automatic mode, sharpness is adjusted automatically and saved in register EDGE (0x3F)
		(0b0 << 4) |		// De-noise threshold auto-adjustment (see Table 5-6)
							//  0: Manual mode, de-noise strength is set by register DNSTH (0x4C)
							//  1: Automatic mode, de-noise strength is adjusted automatically and saved in register DNSTH (0x4C)
		(0b1 << 3) |		// AWB gain enable (see Table 5-1)
							//  0: Bypass AWB gain
							//  1: Enable AWB gain
		(0b0 << 1));		// Color matrix coefficient double option
	//-------------------------------------------------------------------------
	// Table 5-5. Sharpness Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg3F_EDGE,
		(0 << 0));			// Edge enhancement factor (0-31)
	WriteReg(Reg75,
		(15 << 0));			// Edge enhancement higher limit (0-31, effective in automatic mode)
							// (the description in the register set is wrong)
	WriteReg(Reg76,
		(0b1 << 7) |		// Black pixel correction enable
							//  0: Disable
							//  1: Enable
		(0b1 << 6) |		// White pixel correction enable
							//  0: Disable
							//  1: Enable
		(1 << 0));			// Edge enhancement lower limit (0-31, effective in automatic mode)
							// (the description in the register set is wrong)
	//-------------------------------------------------------------------------
	// Table 5-6. De-Noise Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg4C_DNSTH,
		0);					// De-noise Strength (0-255)
	WriteReg(Reg77,
		16);				// De-noise offset (0-255, effective in automatic mode)
	//-------------------------------------------------------------------------
	// Table 5-7. Auto Color Saturation Adjustment Related Registers
	//-------------------------------------------------------------------------
	WriteReg(RegC9_SATCTR,
		(12 << 4) |			// UV Saturation control min (0-15)
		(0 << 0));			// UV saturation control result (0-15)
	//-------------------------------------------------------------------------
	// Table 5-8. Lens Shading Correction Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg62_LCC1,	// X Coordinate of Lens Correction Center Relative to Array Center
		0x00);				// Lens Correction Option 1
	WriteReg(Reg63_LCC2,	// Y Coordinate of Lens Correction Center Relative to Array Center
		0x00);				// Lens Correction Option 2
	WriteReg(Reg64_LCC3,	// G Channel Compensation Coefficient when LCC5 (0x66) is 1
		0x04);				// Lens Correction Option 3
	WriteReg(Reg65_LCC4,	// Radius of the circular section where no compensation is applied
		0x20);				// Lens Correction Option 4
	WriteReg(Reg66_LCC5,	// Lens Correction Control
		(0b1 << 2) |		// Lens Correction control select
							//  0: Apply same coefficient to R, G, and B channels
							//  1: Apply different coefficients to R, G, and B channels
		(0b1 << 0));		// Lens correction enable
							//  0: Disable lens correction function
							//  1: Enable lens correction function
	WriteReg(Reg94_LCC6,
		0x04);				// B Channel Compensation Coefficient when LCC5 (0x66) is 1
	WriteReg(Reg95_LCC7,
		0x08);				// R Channel Compensation Coefficient when LCC5 (0x66) is 1
	//-------------------------------------------------------------------------
	// Table 5-9. Brightness and Contrast Related Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg55_BRIGHT,
		(0b0 << 7) |		// Brightness control sign bit
							//  0: Positive
							//  1: Negative
		(0 << 0));			// Brightness control (0-127)
	WriteReg(Reg56_CONTRAS,
		0x40);				// Contrast control (0-255)
	WriteReg(Reg57_CONTRAS_CENTER,
		0x80);				// Contrast center control
							// Reg58_MTXS[7] decides manual/auto mode
	//-------------------------------------------------------------------------
	// Table 6-1. Image Scaling Control Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg74,
		(0b1 << 4) |		// DG_Manu
							//  0: Digital gain control by VREF[7:6]
							//  1: Digital gain control by REG74[1:0]
		(0b01 << 0));		// Digital gain manual control
							//  00: Bypass
							//  01: 1x
							//  10: 2x
							//  11: 4x
	//-------------------------------------------------------------------------
	// Table 6-4. Windowing Control Registers
	//-------------------------------------------------------------------------
	//WriteReg(Reg17_HSTART,
	//	static_cast<uint8_t>(hStart >> 3));
	//WriteReg(Reg18_HSTOP,
	//	static_cast<uint8_t>(hStop >> 3));
	//WriteReg(Reg32_HREF,
	//	static_cast<uint8_t>((0b10 << 6) | ((hStop & 0b111) << 3) |((hStart & 0b111) << 0)));
	//WriteReg(Reg19_VSTRT,
	//	static_cast<uint8_t>(vStart >> 2));
	//WriteReg(Reg1A_VSTOP,
	//	static_cast<uint8_t>(vStop >> 2));
	// (Reg03_VREF is set in Table 4-1)
	//-------------------------------------------------------------------------
	// Table 7-1. Output Drive Current
	//-------------------------------------------------------------------------
	WriteReg(Reg09_COM2,
		(0b0 << 4) |		// Soft sleep mode (see Table 8-1)
							//  0: Disable standby mode
							//  1: Enable standby mode
		(0b00 << 0));		// Output drive capability
							//  00: 1x IOL/IOH Enable
							//  01: 2x IOL/IOH Enable
							//  10: 3x IOL/IOH Enable
							//  11: 4x IOL/IOH Enable
	WriteReg(Reg33_CHLF,
		0x08);				// Array Current Control (Reserved)
	//-------------------------------------------------------------------------
	// ADC (?)
	//-------------------------------------------------------------------------
	WriteReg(Reg37_ADC,
		0x3f);				// ADC Control (Reserved)
	WriteReg(Reg20_ADCCTR0,
		(0b0 << 3) |		// ADC range adjustment
		(0b100 << 0));		// ADC reference adjustment
	WriteReg(Reg38_ACOM,
		0x01);				// ADC and Analog Common Mode Control (Reserved)
	WriteReg(Reg39_OFON,
		0x00);				// ADC Offset Control (Reserved)
	updateResolutionAndFormatFlag_ = true;
	return *this;
}

OV7670& OV7670::SetupReg_ResolutionAndFormat()
{
	//-------------------------------------------------------------------------
	// Table 2-1. OV7670/OV7171 Output Formats
	// Table 2-2. Resolution Register Settings
	//-------------------------------------------------------------------------
	const ResolutionSetting& resolutionSetting =
		(resolution_ == Resolution::VGA)?		resolutionSetting_VGA :
		(resolution_ == Resolution::QVGA)?		resolutionSetting_QVGA :
		(resolution_ == Resolution::QQVGA)?		resolutionSetting_QQVGA :
		(resolution_ == Resolution::QQQVGA)?	resolutionSetting_QQQVGA :
		(resolution_ == Resolution::CIF)?		resolutionSetting_CIF :
		(resolution_ == Resolution::QCIF)?		resolutionSetting_QCIF :
		(resolution_ == Resolution::QQCIF)?		resolutionSetting_QQCIF : resolutionSetting_VGA;
	const FormatSetting& formatSetting =
		(format_ == Format::RawBayerRGB)?		formatSetting_RawBayerRGB :
		(format_ == Format::ProcessedBayerRGB)?	formatSetting_ProcessedBayerRGB :
		(format_ == Format::YUV422)?			formatSetting_YUV422 :
		(format_ == Format::GRB422)?			formatSetting_GRB422 :
		(format_ == Format::RGB565)?			formatSetting_RGB565 :
		(format_ == Format::RGB555)?			formatSetting_RGB555 :
		(format_ == Format::RGB444)?			formatSetting_RGB444 : formatSetting_RGB565;
	WriteReg(Reg11_CLKRC,	resolutionSetting.Reg11_CLKRC);
	WriteReg(Reg12_COM7,	resolutionSetting.Reg12_COM7 | formatSetting.Reg12_COM7 |
		(0b0 << 7) |		// SCCB Register Reset
							//  0: No change
							//  1: Resets all registers to default values
		(0b0 << 1));		// Color bar
							//  0: Disable
							//  1: Enable
	WriteReg(Reg40_COM15,	formatSetting.Reg40_COM15 |
		(0b11 < 6));		// Data format - output full range enable
							//  0x: Output range: [10] to [F0]
							//  10: Output range: [01] to [FE]
							//  11: Output range: [00] to [FF]
	WriteReg(Reg8C_RGB444,	formatSetting.Reg8C_RGB444);
	WriteReg(Reg0C_COM3,	resolutionSetting.Reg0C_COM3 |
		(0b0 << 6) |		// Output data MSB and LSB swap
		(0b0 << 5) |		// Tri-state PCLK, HREF/HSYNC, VSYNC, and STROBE in power-down mode, active low (see Table 7-2)
		(0b0 << 4));		// Tri-state data bud D[7:0] in power-down mode, active low (see Table 7-2)
	WriteReg(Reg3E_COM14,	resolutionSetting.Reg3E_COM14);
	WriteReg(Reg70_SCALING_XSC, resolutionSetting.Reg70_SCALING_XSC |
		(0b0 << 7));		// Test pattern[0]
	WriteReg(Reg71_SCALING_YSC, resolutionSetting.Reg71_SCALING_YSC |
		(0b0 << 7));		// Test pattern[1]
	WriteReg(Reg72_SCALING_DCWCTR, resolutionSetting.Reg72_SCALING_DCWCTR |
		(0b0 << 7) |		// Vertical average calculation option (see Table 6-1)
							//  0: Vertical truncation
							//  1: Vertical rounding
		(0b0 << 6) |		// Vertical down sampling option (see Table 6-1)
							//  0: Vertical truncation
							//  1: Vertical rounding
		(0b0 << 3) |		// Horizontal average calculation option (see Table 6-1)
							//  0: Horizontal truncation
							//  1: Horizontal rounding
		(0b0 << 2));		// Horizontal down sampling option (see Table 6-1)
							//  0: Horizontal truncation
							//  1: Horizontal rounding
	WriteReg(Reg73_SCALING_PCLK_DIV,	resolutionSetting.Reg73_SCALING_PCLK_DIV |
		(0b1111 << 4));		// Reserved bits
	WriteReg(RegA2_SCALING_PCLK_DELAY,	resolutionSetting.RegA2_SCALING_PCLK_DELAY);
	WriteReg(Reg3A_TSLB, formatSetting.Reg3A_TSLB |
		(0b0 << 5) | 		// Negative image enable
							//  0: Normal image
							//  1: Negative image
		(0b0 << 4) |		// UV output value
							//  0: Use normal UV output
							//  1: Use fixed UV value set (enable Reg67_MANU and Reg68_MANV)
		(0b0 << 0));		// Auto output window
							//  0: Sensor DOES NOT autommatically set window
							//  1: Sensor automatically sets output window
	WriteReg(Reg3D_COM13, formatSetting.Reg3D_COM13 |
		(0b1 << 7) |		// Gamma enable
		(0b1 << 6));		// UV saturation level - UV auto-adjustment
	WriteReg(Reg67_MANU,
		0x80);				// Manual U value when Reg3A_TSLB[4] = 1
	WriteReg(Reg68_MANV,
		0x80);				// Manual V value when Reg3A_TSLB[4] = 1
	SetReg_HStart(resolutionSetting.hStart);
	SetReg_HStop(resolutionSetting.hStart + 640);
	SetReg_VStart(resolutionSetting.vStart);
	SetReg_VStop(resolutionSetting.vStart + 480);
	return *this;
}

OV7670& OV7670::SetReg_Brightness(int8_t value)
{
	WriteReg(OV7670::Reg55_BRIGHT,
		(value < 0)? ((0b1 << 7) | static_cast<uint8_t>(-value)) : static_cast<uint8_t>(value));
	return *this;
}

int8_t OV7670::GetReg_Brightness()
{
	uint8_t value = ReadReg(OV7670::Reg55_BRIGHT);
	return (value & (0b1 << 7))? -static_cast<int8_t>(value & 0b01111111) : static_cast<int8_t>(value & 0b01111111);
}

OV7670& OV7670::SetReg_Contrast(uint8_t value)
{
	WriteReg(OV7670::Reg56_CONTRAS, value);
	return *this;
}

uint8_t OV7670::GetReg_Contrast()
{
	uint8_t value = ReadReg(OV7670::Reg56_CONTRAS);
	return value;
}

OV7670& OV7670::SetReg_Sharpness(uint8_t value)
{
	WriteRegBits(OV7670::Reg3F_EDGE, 0, 5, value);
	return *this;
}

uint8_t OV7670::GetReg_Sharpness()
{
	uint8_t value = ReadRegBits(OV7670::Reg3F_EDGE, 0, 5);
	return value;
}

OV7670& OV7670::SetReg_SharpnessMax(uint8_t value)
{
	WriteRegBits(OV7670::Reg75, 0, 5, value);
	return *this;
}

uint8_t OV7670::GetReg_SharpnessMax()
{
	uint8_t value = ReadRegBits(OV7670::Reg75, 0, 5);
	return value;
}

OV7670& OV7670::SetReg_SharpnessMin(uint8_t value)
{
	WriteRegBits(OV7670::Reg76, 0, 5, value);
	return *this;
}

uint8_t OV7670::GetReg_SharpnessMin()
{
	uint8_t value = ReadRegBits(OV7670::Reg76, 0, 5);
	return value;
}

OV7670& OV7670::SetReg_Denoise(uint8_t value)
{
	WriteReg(OV7670::Reg4C_DNSTH, value);
	return *this;
}

uint8_t OV7670::GetReg_Denoise()
{
	uint8_t value = ReadReg(OV7670::Reg4C_DNSTH);
	return value;
}

OV7670& OV7670::SetReg_DenoiseOffset(uint8_t value)
{
	WriteReg(OV7670::Reg77, value);
	return *this;
}

uint8_t OV7670::GetReg_DenoiseOffset()
{
	uint8_t value = ReadReg(OV7670::Reg77);
	return value;
}

OV7670& OV7670::SetReg_AwbBlue(uint8_t value)
{
	WriteReg(OV7670::Reg01_BLUE, value);
	return *this;
}

uint8_t OV7670::GetReg_AwbBlue()
{
	uint8_t value = ReadReg(OV7670::Reg01_BLUE);
	return value;
}

OV7670& OV7670::SetReg_AwbRed(uint8_t value)
{
	WriteReg(OV7670::Reg02_RED, value);
	return *this;
}

uint8_t OV7670::GetReg_AwbRed()
{
	uint8_t value = ReadReg(OV7670::Reg02_RED);
	return value;
}

OV7670& OV7670::SetReg_AwbGreen(uint8_t value)
{
	WriteReg(OV7670::Reg6A_GGAIN, value);
	return *this;
}

uint8_t OV7670::GetReg_AwbGreen()
{
	uint8_t value = ReadReg(OV7670::Reg6A_GGAIN);
	return value;
}

OV7670& OV7670::SetReg_Exposure(uint32_t value)
{
	WriteReg(OV7670::Reg07_AECHH, static_cast<uint8_t>((value >> 10) & 0xff));
	WriteReg(OV7670::Reg10_AECH, static_cast<uint8_t>((value >> 2) & 0xff));
	WriteRegBits(OV7670::Reg04_COM1, 0, 2, static_cast<uint8_t>(value & 0b11));
	return *this;
}

uint32_t OV7670::GetReg_Exposure()
{
	uint32_t value = ReadReg(OV7670::Reg07_AECHH);
	value = (value << 8) | ReadReg(OV7670::Reg10_AECH);
	value = (value << 2) | ReadRegBits(OV7670::Reg04_COM1, 0, 2);
	return value;
}

OV7670& OV7670::SetReg_DummyPixels(uint16_t value)
{
	WriteRegBits(Reg2A_EXHCH, 4, 4, static_cast<uint8_t>(value >> 8));
	WriteReg(Reg2B_EXHCL, static_cast<uint8_t>(value & 0xff));
	return *this;
}

uint16_t OV7670::GetReg_DummyPixels()
{
	uint16_t value = ReadRegBits(Reg2A_EXHCH, 4, 4);
	value = (value << 8) | ReadReg(Reg2B_EXHCL);
	return value;
}

OV7670& OV7670::SetReg_DummyRows(uint16_t value)
{
	WriteReg(Reg93_DM_LNH, static_cast<uint8_t>(value >> 8));
	WriteReg(Reg92_DM_LNL, static_cast<uint8_t>(value & 0xff));
	return *this;
}

uint16_t OV7670::GetReg_DummyRows()
{
	uint16_t value = ReadReg(Reg93_DM_LNH);
	value = (value << 8) | ReadReg(Reg92_DM_LNL);
	return value;
}

OV7670& OV7670::SetReg_HStart(uint16_t value)
{
	WriteReg(Reg17_HSTART, static_cast<uint8_t>(value >> 3));
	WriteRegBits(Reg32_HREF, 0, 3, static_cast<uint8_t>(value & 0b111));
	return *this;
}

uint16_t OV7670::GetReg_HStart()
{
	uint16_t value = ReadReg(Reg17_HSTART);
	value = (value << 3) | (ReadRegBits(Reg32_HREF, 0, 3));
	return value;
}

OV7670& OV7670::SetReg_HStop(uint16_t value)
{
	value = value % 784;
	WriteReg(Reg18_HSTOP, static_cast<uint8_t>(value >> 3));
	WriteRegBits(Reg32_HREF, 3, 3, static_cast<uint8_t>(value & 0b111));
	return *this;
}

uint16_t OV7670::GetReg_HStop()
{
	uint16_t value = ReadReg(Reg18_HSTOP);
	value = (value << 3) | (ReadRegBits(Reg32_HREF, 3, 3));
	return value;
}

OV7670& OV7670::SetReg_VStart(uint16_t value)
{
	WriteReg(Reg19_VSTRT, static_cast<uint8_t>(value >> 2));
	WriteRegBits(Reg03_VREF, 0, 2, static_cast<uint8_t>(value & 0b11));
	return *this;
}

uint16_t OV7670::GetReg_VStart()
{
	uint16_t value = ReadReg(Reg19_VSTRT);
	value = (value << 2) | (ReadRegBits(Reg03_VREF, 0, 2));
	return value;
}

OV7670& OV7670::SetReg_VStop(uint16_t value)
{
	WriteReg(Reg1A_VSTOP, static_cast<uint8_t>(value >> 2));
	WriteRegBits(Reg03_VREF, 2, 2, static_cast<uint8_t>(value & 0b11));
	return *this;
}

uint16_t OV7670::GetReg_VStop()
{
	uint16_t value = ReadReg(Reg1A_VSTOP);
	value = (value << 2) | (ReadRegBits(Reg03_VREF, 2, 2));
	return value;
}

OV7670& OV7670::ResetAllReg()
{
	WriteReg(Reg12_COM7,
		(0b1 << 7));		// SCCB Register Reset
							//  0: No change
							//  1: Resets all registers to default values
	Tickable::Sleep(100);
	return *this;
}

OV7670& OV7670::EnableColorMode(bool enableFlag)
{
	// Undocumented, but VERY IMPORTANT to capture colors!
	WriteReg(RegB0, enableFlag? 0x84 : 0x00);
	return *this;
}

void OV7670::FreeResource()
{
	image_.Free();
}

void OV7670::DoCapture()
{
	if (!image_.IsValid()) {
		if (!image_.Allocate(
			(format_ == Format::RawBayerRGB)?		Image::Format::RGB :
			(format_ == Format::ProcessedBayerRGB)?	Image::Format::RGB :
			(format_ == Format::YUV422)?			Image::Format::YUV422 :
			//(format_ == Format::GRB422)?			Image::Format::GRB422 :
			(format_ == Format::RGB565)?			Image::Format::RGB565 :
			//(format_ == Format::RGB555)?			Image::Format::RGB555 :
			//(format_ == Format::RGB444)?			Image::Format::RGB444 :
			Image::Format::RGB, size_)) return;
	}
	if (updateResolutionAndFormatFlag_) {
		SetupReg_ResolutionAndFormat();
		updateResolutionAndFormatFlag_ = false;
	}
	sm_.set_enabled(false);
	sm_.clear_fifos().exec(programToReset_);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(image_.GetPointer())
		.set_trans_count_trig(image_.GetBytesBuff() / sizeof(uint32_t));
	sm_.set_enabled();
	sm_.put(image_.GetWidth() * 2 - 1);
	while (pChannel_->is_busy()) Tickable::Tick();
}

const char* OV7670::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "reso:%s format:%s i2c%d D0:%d XCLK:%d PCLK:%d HREF:%d VSYNC:%d freq:%dHz",
		(resolution_ == Resolution::VGA)?		"vga" :
		(resolution_ == Resolution::QVGA)?		"qvga" :
		(resolution_ == Resolution::QQVGA)?		"qqvga" :
		(resolution_ == Resolution::QQQVGA)?	"qqqvga" :
		(resolution_ == Resolution::CIF)?		"cif" :
		(resolution_ == Resolution::QCIF)?		"qcif" :
		(resolution_ == Resolution::QQCIF)?		"qqcif" : "unknown",
		(format_ == Format::RawBayerRGB)?		"raw-rgb" :
		(format_ == Format::ProcessedBayerRGB)?	"processed-rgb" :
		(format_ == Format::YUV422)?			"yuv422" :
		(format_ == Format::GRB422)?			"grb422" :
		(format_ == Format::RGB565)?			"rgb565" :
		(format_ == Format::RGB555)?			"rgb555" :
		(format_ == Format::RGB444)?			"rgb444" : "unknown",
		::i2c_get_index(i2c_),
		pinAssign_.D0.pin, pinAssign_.XCLK.pin, pinAssign_.PCLK.pin, pinAssign_.HREF.pin, pinAssign_.VSYNC.pin,
		freq_);
	return buff;
}

}
