//==============================================================================
// OV7670.cpp
//==============================================================================
#include "jxglib/Device/OV7670.h"

#define OV7670_REG_GAIN 0x00               //< AGC gain bits 7:0 (9:8 in VREF)
#define OV7670_REG_BLUE 0x01               //< AWB blue channel gain
#define OV7670_REG_RED 0x02                //< AWB red channel gain
#define OV7670_REG_VREF 0x03               //< Vert frame control bits
#define OV7670_REG_COM1 0x04               //< Common control 1
#define OV7670_COM1_R656 0x40              //< COM1 enable R656 format
#define OV7670_REG_BAVE 0x05               //< U/B average level
#define OV7670_REG_GbAVE 0x06              //< Y/Gb average level
#define OV7670_REG_AECHH 0x07              //< Exposure value - AEC 15:10 bits
#define OV7670_REG_RAVE 0x08               //< V/R average level
#define OV7670_REG_COM2 0x09               //< Common control 2
#define OV7670_COM2_SSLEEP 0x10            //< COM2 soft sleep mode
#define OV7670_REG_PID 0x0A                //< Product ID MSB (read-only)
#define OV7670_REG_VER 0x0B                //< Product ID LSB (read-only)
#define OV7670_REG_COM3 0x0C               //< Common control 3
#define OV7670_COM3_SWAP 0x40              //< COM3 output data MSB/LSB swap
#define OV7670_COM3_SCALEEN 0x08           //< COM3 scale enable
#define OV7670_COM3_DCWEN 0x04             //< COM3 DCW enable
#define OV7670_REG_COM4 0x0D               //< Common control 4
#define OV7670_REG_COM5 0x0E               //< Common control 5
#define OV7670_REG_COM6 0x0F               //< Common control 6
#define OV7670_REG_AECH 0x10               //< Exposure value 9:2
#define OV7670_REG_CLKRC 0x11              //< Internal clock
#define OV7670_CLK_EXT 0x40                //< CLKRC Use ext clock directly
#define OV7670_CLK_SCALE 0x3F              //< CLKRC Int clock prescale mask
#define OV7670_REG_COM7 0x12               //< Common control 7
#define OV7670_COM7_RESET 0x80             //< COM7 SCCB register reset
#define OV7670_COM7_SIZE_MASK 0x38         //< COM7 output size mask
#define OV7670_COM7_PIXEL_MASK 0x05        //< COM7 output pixel format mask
#define OV7670_COM7_SIZE_VGA 0x00          //< COM7 output size VGA
#define OV7670_COM7_SIZE_CIF 0x20          //< COM7 output size CIF
#define OV7670_COM7_SIZE_QVGA 0x10         //< COM7 output size QVGA
#define OV7670_COM7_SIZE_QCIF 0x08         //< COM7 output size QCIF
#define OV7670_COM7_RGB 0x04               //< COM7 pixel format RGB
#define OV7670_COM7_YUV 0x00               //< COM7 pixel format YUV
#define OV7670_COM7_BAYER 0x01             //< COM7 pixel format Bayer RAW
#define OV7670_COM7_PBAYER 0x05            //< COM7 pixel fmt proc Bayer RAW
#define OV7670_COM7_COLORBAR 0x02          //< COM7 color bar enable
#define OV7670_REG_COM8 0x13               //< Common control 8
#define OV7670_COM8_FASTAEC 0x80           //< COM8 Enable fast AGC/AEC algo,
#define OV7670_COM8_AECSTEP 0x40           //< COM8 AEC step size unlimited
#define OV7670_COM8_BANDING 0x20           //< COM8 Banding filter enable
#define OV7670_COM8_AGC 0x04               //< COM8 AGC (auto gain) enable
#define OV7670_COM8_AWB 0x02               //< COM8 AWB (auto white balance)
#define OV7670_COM8_AEC 0x01               //< COM8 AEC (auto exposure) enable
#define OV7670_REG_COM9 0x14               //< Common control 9 - max AGC value
#define OV7670_REG_COM10 0x15              //< Common control 10
#define OV7670_COM10_HSYNC 0x40            //< COM10 HREF changes to HSYNC
#define OV7670_COM10_PCLK_HB 0x20          //< COM10 Suppress PCLK on hblank
#define OV7670_COM10_HREF_REV 0x08         //< COM10 HREF reverse
#define OV7670_COM10_VS_EDGE 0x04          //< COM10 VSYNC chg on PCLK rising
#define OV7670_COM10_VS_NEG 0x02           //< COM10 VSYNC negative
#define OV7670_COM10_HS_NEG 0x01           //< COM10 HSYNC negative
#define OV7670_REG_HSTART 0x17             //< Horiz frame start high bits
#define OV7670_REG_HSTOP 0x18              //< Horiz frame end high bits
#define OV7670_REG_VSTART 0x19             //< Vert frame start high bits
#define OV7670_REG_VSTOP 0x1A              //< Vert frame end high bits
#define OV7670_REG_PSHFT 0x1B              //< Pixel delay select
#define OV7670_REG_MIDH 0x1C               //< Manufacturer ID high byte
#define OV7670_REG_MIDL 0x1D               //< Manufacturer ID low byte
#define OV7670_REG_MVFP 0x1E               //< Mirror / vert-flip enable
#define OV7670_MVFP_MIRROR 0x20            //< MVFP Mirror image
#define OV7670_MVFP_VFLIP 0x10             //< MVFP Vertical flip
#define OV7670_REG_LAEC 0x1F               //< Reserved
#define OV7670_REG_ADCCTR0 0x20            //< ADC control
#define OV7670_REG_ADCCTR1 0x21            //< Reserved
#define OV7670_REG_ADCCTR2 0x22            //< Reserved
#define OV7670_REG_ADCCTR3 0x23            //< Reserved
#define OV7670_REG_AEW 0x24                //< AGC/AEC upper limit
#define OV7670_REG_AEB 0x25                //< AGC/AEC lower limit
#define OV7670_REG_VPT 0x26                //< AGC/AEC fast mode op region
#define OV7670_REG_BBIAS 0x27              //< B channel signal output bias
#define OV7670_REG_GbBIAS 0x28             //< Gb channel signal output bias
#define OV7670_REG_EXHCH 0x2A              //< Dummy pixel insert MSB
#define OV7670_REG_EXHCL 0x2B              //< Dummy pixel insert LSB
#define OV7670_REG_RBIAS 0x2C              //< R channel signal output bias
#define OV7670_REG_ADVFL 0x2D              //< Insert dummy lines MSB
#define OV7670_REG_ADVFH 0x2E              //< Insert dummy lines LSB
#define OV7670_REG_YAVE 0x2F               //< Y/G channel average value
#define OV7670_REG_HSYST 0x30              //< HSYNC rising edge delay
#define OV7670_REG_HSYEN 0x31              //< HSYNC falling edge delay
#define OV7670_REG_HREF 0x32               //< HREF control
#define OV7670_REG_CHLF 0x33               //< Array current control
#define OV7670_REG_ARBLM 0x34              //< Array ref control - reserved
#define OV7670_REG_ADC 0x37                //< ADC control - reserved
#define OV7670_REG_ACOM 0x38               //< ADC & analog common - reserved
#define OV7670_REG_OFON 0x39               //< ADC offset control - reserved
#define OV7670_REG_TSLB 0x3A               //< Line buffer test option
#define OV7670_TSLB_NEG 0x20               //< TSLB Negative image enable
#define OV7670_TSLB_YLAST 0x04             //< TSLB UYVY or VYUY, see COM13
#define OV7670_TSLB_AOW 0x01               //< TSLB Auto output window
#define OV7670_REG_COM11 0x3B              //< Common control 11
#define OV7670_COM11_NIGHT 0x80            //< COM11 Night mode
#define OV7670_COM11_NMFR 0x60             //< COM11 Night mode frame rate mask
#define OV7670_COM11_HZAUTO 0x10           //< COM11 Auto detect 50/60 Hz
#define OV7670_COM11_BAND 0x08             //< COM11 Banding filter val select
#define OV7670_COM11_EXP 0x02              //< COM11 Exposure timing control
#define OV7670_REG_COM12 0x3C              //< Common control 12
#define OV7670_COM12_HREF 0x80             //< COM12 Always has HREF
#define OV7670_REG_COM13 0x3D              //< Common control 13
#define OV7670_COM13_GAMMA 0x80            //< COM13 Gamma enable
#define OV7670_COM13_UVSAT 0x40            //< COM13 UV saturation auto adj
#define OV7670_COM13_UVSWAP 0x01           //< COM13 UV swap, use w TSLB[3]
#define OV7670_REG_COM14 0x3E              //< Common control 14
#define OV7670_COM14_DCWEN 0x10            //< COM14 DCW & scaling PCLK enable
#define OV7670_REG_EDGE 0x3F               //< Edge enhancement adjustment
#define OV7670_REG_COM15 0x40              //< Common control 15
#define OV7670_COM15_RMASK 0xC0            //< COM15 Output range mask
#define OV7670_COM15_R10F0 0x00            //< COM15 Output range 10 to F0
#define OV7670_COM15_R01FE 0x80            //< COM15              01 to FE
#define OV7670_COM15_R00FF 0xC0            //< COM15              00 to FF
#define OV7670_COM15_RGBMASK 0x30          //< COM15 RGB 555/565 option mask
#define OV7670_COM15_RGB 0x00              //< COM15 Normal RGB out
#define OV7670_COM15_RGB565 0x10           //< COM15 RGB 565 output
#define OV7670_COM15_RGB555 0x30           //< COM15 RGB 555 output
#define OV7670_REG_COM16 0x41              //< Common control 16
#define OV7670_COM16_AWBGAIN 0x08          //< COM16 AWB gain enable
#define OV7670_REG_COM17 0x42              //< Common control 17
#define OV7670_COM17_AECWIN 0xC0           //< COM17 AEC window must match COM4
#define OV7670_COM17_CBAR 0x08             //< COM17 DSP Color bar enable
#define OV7670_REG_AWBC1 0x43              //< Reserved
#define OV7670_REG_AWBC2 0x44              //< Reserved
#define OV7670_REG_AWBC3 0x45              //< Reserved
#define OV7670_REG_AWBC4 0x46              //< Reserved
#define OV7670_REG_AWBC5 0x47              //< Reserved
#define OV7670_REG_AWBC6 0x48              //< Reserved
#define OV7670_REG_REG4B 0x4B              //< UV average enable
#define OV7670_REG_DNSTH 0x4C              //< De-noise strength
#define OV7670_REG_MTX1 0x4F               //< Matrix coefficient 1
#define OV7670_REG_MTX2 0x50               //< Matrix coefficient 2
#define OV7670_REG_MTX3 0x51               //< Matrix coefficient 3
#define OV7670_REG_MTX4 0x52               //< Matrix coefficient 4
#define OV7670_REG_MTX5 0x53               //< Matrix coefficient 5
#define OV7670_REG_MTX6 0x54               //< Matrix coefficient 6
#define OV7670_REG_BRIGHT 0x55             //< Brightness control
#define OV7670_REG_CONTRAS 0x56            //< Contrast control
#define OV7670_REG_CONTRAS_CENTER 0x57     //< Contrast center
#define OV7670_REG_MTXS 0x58               //< Matrix coefficient sign
#define OV7670_REG_LCC1 0x62               //< Lens correction option 1
#define OV7670_REG_LCC2 0x63               //< Lens correction option 2
#define OV7670_REG_LCC3 0x64               //< Lens correction option 3
#define OV7670_REG_LCC4 0x65               //< Lens correction option 4
#define OV7670_REG_LCC5 0x66               //< Lens correction option 5
#define OV7670_REG_MANU 0x67               //< Manual U value
#define OV7670_REG_MANV 0x68               //< Manual V value
#define OV7670_REG_GFIX 0x69               //< Fix gain control
#define OV7670_REG_GGAIN 0x6A              //< G channel AWB gain
#define OV7670_REG_DBLV 0x6B               //< PLL & regulator control
#define OV7670_REG_AWBCTR3 0x6C            //< AWB control 3
#define OV7670_REG_AWBCTR2 0x6D            //< AWB control 2
#define OV7670_REG_AWBCTR1 0x6E            //< AWB control 1
#define OV7670_REG_AWBCTR0 0x6F            //< AWB control 0
#define OV7670_REG_SCALING_XSC 0x70        //< Test pattern X scaling
#define OV7670_REG_SCALING_YSC 0x71        //< Test pattern Y scaling
#define OV7670_REG_SCALING_DCWCTR 0x72     //< DCW control
#define OV7670_REG_SCALING_PCLK_DIV 0x73   //< DSP scale control clock divide
#define OV7670_REG_REG74 0x74              //< Digital gain control
#define OV7670_REG_REG76 0x76              //< Pixel correction
#define OV7670_REG_SLOP 0x7A               //< Gamma curve highest seg slope
#define OV7670_REG_GAM_BASE 0x7B           //< Gamma register base (1 of 15)
#define OV7670_GAM_LEN 15                  //< Number of gamma registers
#define OV7670_R76_BLKPCOR 0x80            //< REG76 black pixel corr enable
#define OV7670_R76_WHTPCOR 0x40            //< REG76 white pixel corr enable
#define OV7670_REG_RGB444 0x8C             //< RGB 444 control
#define OV7670_R444_ENABLE 0x02            //< RGB444 enable
#define OV7670_R444_RGBX 0x01              //< RGB444 word format
#define OV7670_REG_DM_LNL 0x92             //< Dummy line LSB
#define OV7670_REG_LCC6 0x94               //< Lens correction option 6
#define OV7670_REG_LCC7 0x95               //< Lens correction option 7
#define OV7670_REG_HAECC1 0x9F             //< Histogram-based AEC/AGC ctrl 1
#define OV7670_REG_HAECC2 0xA0             //< Histogram-based AEC/AGC ctrl 2
#define OV7670_REG_SCALING_PCLK_DELAY 0xA2 //< Scaling pixel clock delay
#define OV7670_REG_BD50MAX 0xA5            //< 50 Hz banding step limit
#define OV7670_REG_HAECC3 0xA6             //< Histogram-based AEC/AGC ctrl 3
#define OV7670_REG_HAECC4 0xA7             //< Histogram-based AEC/AGC ctrl 4
#define OV7670_REG_HAECC5 0xA8             //< Histogram-based AEC/AGC ctrl 5
#define OV7670_REG_HAECC6 0xA9             //< Histogram-based AEC/AGC ctrl 6
#define OV7670_REG_HAECC7 0xAA             //< Histogram-based AEC/AGC ctrl 7
#define OV7670_REG_BD60MAX 0xAB            //< 60 Hz banding step limit
#define OV7670_REG_ABLC1 0xB1              //< ABLC enable
#define OV7670_REG_THL_ST 0xB3             //< ABLC target
#define OV7670_REG_SATCTR 0xC9             //< Saturation control

#define OV7670_REG_LAST OV7670_REG_SATCTR //< Maximum register address

namespace jxglib::Device {

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
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
		(0b01 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b000 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b0000010 << 0),	// Scaling output delay
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
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
		(0b01 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b001 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b0000010 << 0),	// Scaling output delay
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
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b10 << 4) |		// Vertical downsampling rate
		(0b10 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b010 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b0000010 << 0),	// Scaling output delay
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
		(0b0 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0),		// PCLK divider (only when COM14[4] = 1)
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
		(0b01 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b001 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b0000010 << 0),	// Scaling output delay
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
		(0b0 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b001 << 0),		// PCLK divider (only when COM14[4] = 1)
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b01 << 4) |		// Vertical downsampling rate
		(0b01 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b001 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b1010010 << 0),	// Scaling output delay
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
		(0b0 << 3) |		// Manual scaling enable for pre-defined resolution modes such as CIF, QCIF, and QVGA
		(0b010 << 0),		// PCLK divider (only when COM14[4] = 1)
	Reg70_SCALING_XSC:
		(0b0111010 << 0),	// Horizontal scale factor
	Reg71_SCALING_YSC:
		(0b0110101 << 0),	// Vertical scale factor
	Reg72_SCALING_DCWCTR:
		(0b10 << 4) |		// Vertical downsampling rate
		(0b10 << 0),		// Horizontal downsampling rate
	Reg73_SCALING_PCLK_DIV:
		(0b0 << 3) |		// Bypass clock divider for DSP scale control
		(0b010 << 0),		// Clock divider control for DSP scale control	
	RegA2_SCALING_PCLK_DELAY:
		(0b0101010 << 0),	// Scaling output delay
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
};

OV7670::OV7670(Resolution resolution, Format format, i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq) :
	resolution_{resolution}, format_{format}, i2c_{i2c}, pinAssign_{pinAssign}, freq_{freq}
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

bool OV7670::Initialize()
{
	Size size =
		(resolution_ == Resolution::VGA)?	Size{640, 480} :
		(resolution_ == Resolution::QVGA)?	Size{320, 240} :
		(resolution_ == Resolution::QQVGA)?	Size{160, 120} :
		(resolution_ == Resolution::CIF)?	Size{352, 288} :
		(resolution_ == Resolution::QCIF)?	Size{176, 144} :
		(resolution_ == Resolution::QQCIF)?	Size{88, 72} : Size{320, 240};
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
	uint relAddrStart = 0;
	if (!image_.Allocate(
		(format_ == Format::RawBayerRGB)? Image::Format::RGB :
		(format_ == Format::ProcessedBayerRGB)? Image::Format::RGB :
		(format_ == Format::YUV422)? Image::Format::YUV422 :
		//(format_ == Format::GRB422)? Image::Format::GRB422 :
		(format_ == Format::RGB565)? Image::Format::RGB565 :
		//(format_ == Format::RGB555)? Image::Format::RGB555 :
		//(format_ == Format::RGB444)? Image::Format::RGB444 :
		Image::Format::RGB,
		size.width, size.height)) return false;
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
	//SetupParam();
	return true;
}

void OV7670::SetupParam()
{
	uint32_t hStart = 136, hStop = hStart + 640;
	uint32_t vStart = 12, vStop = vStart + 480;
	const ResolutionSetting& resolutionSetting =
		(resolution_ == Resolution::VGA)?	resolutionSetting_VGA :
		(resolution_ == Resolution::QVGA)?	resolutionSetting_QVGA :
		(resolution_ == Resolution::QQVGA)?	resolutionSetting_QQVGA :
		(resolution_ == Resolution::CIF)?	resolutionSetting_CIF :
		(resolution_ == Resolution::QCIF)?	resolutionSetting_QCIF :
		(resolution_ == Resolution::QQCIF)?	resolutionSetting_QQCIF : resolutionSetting_VGA;
	const FormatSetting& formatSetting =
		(format_ == Format::RawBayerRGB)?		formatSetting_RawBayerRGB :
		(format_ == Format::ProcessedBayerRGB)?	formatSetting_ProcessedBayerRGB :
		(format_ == Format::YUV422)?			formatSetting_YUV422 :
		(format_ == Format::GRB422)?			formatSetting_GRB422 :
		(format_ == Format::RGB565)?			formatSetting_RGB565 :
		(format_ == Format::RGB555)?			formatSetting_RGB555 :
		(format_ == Format::RGB444)?			formatSetting_RGB444 : formatSetting_RGB565;
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
	// Table 2-1. OV7670/OV7171 Output Formats
	// Table 2-2. Resolution Register Settings
	//-------------------------------------------------------------------------
	WriteReg(Reg11_CLKRC,				resolutionSetting.Reg11_CLKRC);
	WriteReg(Reg12_COM7,				resolutionSetting.Reg12_COM7 | formatSetting.Reg12_COM7 |
		(0b0 << 7) |		// SCCB Register Reset
							//  0: No change
							//  1: Resets all registers to default values
		(0b0 << 1));		// Color bar
							//  0: Disable
							//  1: Enable
	WriteReg(Reg40_COM15,				formatSetting.Reg40_COM15 |
		(0b11 < 6));		// Data format - output full range enable
							//  0x: Output range: [10] to [F0]
							//  10: Output range: [01] to [FE]
							//  11: Output range: [00] to [FF]
	WriteReg(Reg8C_RGB444,				formatSetting.Reg8C_RGB444);
	WriteReg(Reg0C_COM3,				resolutionSetting.Reg0C_COM3 |
		(0b0 << 6) |		// Output data MSB and LSB swap
		(0b0 << 5) |		// Tri-state option for output clock at power-down period
		(0b0 << 4));		// Tri-state option for output data at power-down period
	WriteReg(Reg3E_COM14,				resolutionSetting.Reg3E_COM14);
	WriteReg(Reg70_SCALING_XSC,			resolutionSetting.Reg70_SCALING_XSC |
		(0b0 << 7));		// Test pattern[0]
	WriteReg(Reg71_SCALING_YSC,			resolutionSetting.Reg71_SCALING_YSC |
		(0b0 << 7));		// Test pattern[1]
	WriteReg(Reg72_SCALING_DCWCTR,		resolutionSetting.Reg72_SCALING_DCWCTR |
		(0b0 << 7) |		// Vertical average calculation option
							//  0: Vertical truncation
							//  1: Vertical rounding
		(0b0 << 6) |		// Vertical down sampling option
							//  0: Vertical truncation
							//  1: Vertical rounding
		(0b0 << 3) |		// Horizontal average calculation option
							//  0: Horizontal truncation
							//  1: Horizontal rounding
		(0b0 << 2));		// Horizontal down sampling option
							//  0: Horizontal truncation
							//  1: Horizontal rounding
	WriteReg(Reg73_SCALING_PCLK_DIV,	resolutionSetting.Reg73_SCALING_PCLK_DIV |
		(0b1111 << 4));		// Reserved bits
	WriteReg(RegA2_SCALING_PCLK_DELAY,	resolutionSetting.RegA2_SCALING_PCLK_DELAY);
#if 0
	WriteReg(0xA1, 0x03);              // Reserved register?
	WriteReg(OV7670_REG_COM5, 0x61);
	WriteReg(OV7670_REG_COM6, 0x4B);
	WriteReg(0x16, 0x02);            // Reserved register?
	WriteReg(OV7670_REG_MVFP, 0x07); // 0x07,
	WriteReg(OV7670_REG_ADCCTR1, 0x02);
	WriteReg(OV7670_REG_ADCCTR2, 0x91);
	WriteReg(0x29, 0x07); // Reserved register?
	WriteReg(OV7670_REG_CHLF, 0x0B);
	WriteReg(0x35, 0x0B); // Reserved register?
	WriteReg(OV7670_REG_ADC, 0x1D);
	WriteReg(OV7670_REG_ACOM, 0x71);
	WriteReg(OV7670_REG_OFON, 0x2A);
	WriteReg(OV7670_REG_COM12, 0x78);
	WriteReg(0x4D, 0x40); // Reserved register?
	WriteReg(0x4E, 0x20); // Reserved register?
	WriteReg(OV7670_REG_GFIX, 0x5D);
	WriteReg(OV7670_REG_REG74, 0x19);
	WriteReg(0x8D, 0x4F); // Reserved register?
	WriteReg(0x8E, 0x00); // Reserved register?
	WriteReg(0x8F, 0x00); // Reserved register?
	WriteReg(0x90, 0x00); // Reserved register?
	WriteReg(0x91, 0x00); // Reserved register?
	WriteReg(OV7670_REG_DM_LNL, 0x00);
	WriteReg(0x96, 0x00); // Reserved register?
	WriteReg(0x9A, 0x80); // Reserved register?
	WriteReg(0xB0, 0x84); // Reserved register?
	WriteReg(OV7670_REG_ABLC1, 0x0C);
	WriteReg(0xB2, 0x0E); // Reserved register?
	WriteReg(OV7670_REG_THL_ST, 0x82);
	WriteReg(0xB8, 0x0A); // Reserved register?
	WriteReg(OV7670_REG_AWBC1, 0x14);
	WriteReg(OV7670_REG_AWBC2, 0xF0);
	WriteReg(OV7670_REG_AWBC3, 0x34);
	WriteReg(OV7670_REG_AWBC4, 0x58);
	WriteReg(OV7670_REG_AWBC5, 0x28);
	WriteReg(OV7670_REG_AWBC6, 0x3A);
	WriteReg(0x59, 0x88); // Reserved register?
	WriteReg(0x5A, 0x88); // Reserved register?
	WriteReg(0x5B, 0x44); // Reserved register?
	WriteReg(0x5C, 0x67); // Reserved register?
	WriteReg(0x5D, 0x49); // Reserved register?
	WriteReg(0x5E, 0x0E); // Reserved register?
	WriteReg(OV7670_REG_LCC3, 0x04);
	WriteReg(OV7670_REG_LCC4, 0x20);
	WriteReg(OV7670_REG_LCC5, 0x05);
	WriteReg(OV7670_REG_LCC6, 0x04);
	WriteReg(OV7670_REG_LCC7, 0x08);
	WriteReg(OV7670_REG_AWBCTR3, 0x0A);
	WriteReg(OV7670_REG_AWBCTR2, 0x55);
		//{OV7670_REG_MTX1, 0x80);
		//{OV7670_REG_MTX2, 0x80);
		//{OV7670_REG_MTX3, 0x00);
		//{OV7670_REG_MTX4, 0x22);
		//{OV7670_REG_MTX5, 0x5E);
		//{OV7670_REG_MTX6, 0x80); // 0x40?
	WriteReg(OV7670_REG_AWBCTR1, 0x11);
		//{OV7670_REG_AWBCTR0, 0x9F); // Or use 0x9E for advance AWB
	WriteReg(OV7670_REG_AWBCTR0, 0x9E); // Or use 0x9E for advance AWB
	WriteReg(OV7670_REG_BRIGHT, 0x00);
	WriteReg(OV7670_REG_CONTRAS, 0x40);
	WriteReg(OV7670_REG_CONTRAS_CENTER, 0x80); // 0x40?
#endif









#if 1
	WriteReg(OV7670_REG_TSLB, OV7670_TSLB_YLAST);    // No auto window
		//{OV7670_REG_COM10, OV7670_COM10_VS_NEG); // -VSYNC (req by SAMD PCC)
	WriteReg(Reg3A_TSLB,
		(0b0 << 5) | 		// Negative image enable
							//  0: Normal image
							//  1: Negative image
		(0b0 << 4) |		// UV output value
							//  0: Use normal UV output
							//  1: Use fixed UV value set
		(0b0 << 3) |		// Output sequence
							//  00: Y U Y V
							//  01: Y V Y U
							//  10: U Y V Y
							//  11: V Y U Y
		(0b0 << 0));		// Auto output window
							//  0: Sensor DOES NOT autommatically set window
							//  1: Sensor automatically sets output window
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
	//WriteReg(OV7670_REG_COM11, (1 << 3)); // 50Hz
	//	//{0x9D, 99); // Banding filter for 50 Hz at 15.625 MHz
	//WriteReg(0x9D, 89); // Banding filter for 50 Hz at 13.888 MHz
	//WriteReg(OV7670_REG_BD50MAX, 0x05);
	//WriteReg(OV7670_REG_BD60MAX, 0x07);
	WriteReg(Reg07_AECHH,
		0x00);				// 
	WriteReg(Reg10_AECH,
		0x00);				// 
	WriteReg(Reg04_COM1,
		(0b0 << 6) |		// CCIR656 format
		(0b00 << 0));		// AEC[1:0]
	WriteReg(Reg9D_BD50ST,
		0x99);				// 
	WriteReg(Reg9E_BD60ST,
		0x7f);				// 
	WriteReg(RegA5_BD50MAX,
		0x05);				// 50Hz Banding Step Limit
	WriteReg(RegAB_BD60MAX,
		0x07);				// 60Hz Banding Step Limit
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
		(0b0 << 1) |		// AWB Enable (see Table 5-1)
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
	//-------------------------------------------------------------------------
	// Table 5-1. White Balance Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg6C_AWBCTR3,
		0x02);				// AWB Control 3 0x0a
	WriteReg(Reg6D_AWBCTR2,
		0x55);				// AWB Control 2 0x55
	WriteReg(Reg6E_AWBCTR1,
		0xc0);				// AWB Control 1 0x11
	WriteReg(Reg6F_AWBCTR0,	// 0x9a
		0x90 |
		(0b1 << 3) |		// 0: AWB Adjusts R and B gain only
							// 1: AWB Adjusts R, G, and B gains
		(0b0 << 2) |		// 0: Maximum color gain is 2x
							// 1: Maximum color gain is 4x
		(0b1 << 1) |		// 
		(0b0 << 0));		// 0: Advanced AWB mode
							// 1: Normal AWB mode
	WriteReg(Reg01_BLUE,
		0xf0);				// AWB - Blue channel gain setting
	WriteReg(Reg02_RED,
		0x80);				// AWB - Red channel gain setting
	WriteReg(Reg6A_GGAIN,
		0x00);				// AWB - Green channel gain setting
	WriteReg(Reg5F_B_LMT,
		0x00);				// AWB Blue Gain Range
	WriteReg(Reg60_R_LMT,
		0x00);				// AWB Red Gain Range
	WriteReg(Reg61_G_LMT,
		0x00);				// AWB Green Gain Range
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
	//WriteReg(Reg43_AWBC1,
	//	0x14);				// Reserved
	//WriteReg(Reg44_AWBC2,
	//	0xf0);				// Reserved
	//WriteReg(Reg45_AWBC3,
	//	0x34);				// Reserved
	//WriteReg(Reg46_AWBC4,
	//	0x58);				// Reserved
	//WriteReg(Reg47_AWBC5,
	//	0x28);				// Reserved
	//WriteReg(Reg48_AWBC6,
	//	0x3a);				// Reserved
	//WriteReg(Reg59_AWBC7,
	//	0x00);
	//WriteReg(Reg5A_AWBC8,
	//	0x00);
	//WriteReg(Reg5B_AWBC9,
	//	0x00);
	//WriteReg(Reg5C_AWBC10,
	//	0x00);
	//WriteReg(Reg5D_AWBC11,
	//	0x00);
	//WriteReg(Reg5E_AWBC12,
	//	0x00);
	//-------------------------------------------------------------------------
	// Table 5-3. Gamma Related Registers and Parameters
	//-------------------------------------------------------------------------
#if 0
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
	WriteReg(Reg7A_SLOP,
		(256 - 208) * 40 / 30);	// Gamma curve highest segment slope
#endif
	//-------------------------------------------------------------------------
	// Table 5-4. Color Matrix Related Registers and Parameters
	//-------------------------------------------------------------------------
#if 0
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
#else
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
#endif
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
		(0b00000 << 0));	// Edge enhancement factor
	WriteReg(Reg75,
		(0b00101 << 0));	// Edge enhancement lower limit
	WriteReg(Reg76,
		(0b1 << 7) |		// Black pixel correction enable
							//  0: Disable
							//  1: Enable
		(0b1 << 6) |		// White pixel correction enable
							//  0: Disable
							//  1: Enable
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
		(0b1100 << 4) |		// UV Saturation control min
		(0b0000 << 0));		// UV saturation control result
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
		0x00);				// Brightness control
	WriteReg(Reg56_CONTRAS,
		0x40);				// Contrast control
	WriteReg(Reg57_CONTRAS_CENTER,
		0x80);				// Contrast center control
	//-------------------------------------------------------------------------
	// Table 6-1. Image Scaling Control Related Registers and Parameters
	//-------------------------------------------------------------------------
	WriteReg(Reg74,
		(0b1 << 4) |		// DG_Manu
		(0b01 << 0));		// Digital gain manual control
	//-------------------------------------------------------------------------
	// Table 6-2. Down Samling Control Related Registers and Parameters
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// Table 6-4. Windowing Control Registers
	//-------------------------------------------------------------------------
	WriteReg(Reg17_HSTART,
		static_cast<uint8_t>(hStart >> 3));
	WriteReg(Reg18_HSTOP,
		static_cast<uint8_t>(hStop >> 3));
	WriteReg(Reg32_HREF,
		static_cast<uint8_t>((0b10 << 6) | ((hStop & 0b111) << 3) |((hStart & 0b111) << 0)));
	WriteReg(Reg19_VSTRT,
		static_cast<uint8_t>(vStart >> 2));
	WriteReg(Reg1A_VSTOP,
		static_cast<uint8_t>(vStop >> 2));
	// (Reg03_VREF is set in Table 4-1)
	//-------------------------------------------------------------------------
	// Table 7-1. Output Drive Current
	//-------------------------------------------------------------------------
	WriteReg(Reg09_COM2,
		(0b0 << 4) |		// Soft sleep mode
							//  0: Disable standby mode
							//  1: Enable standby mode
		(0b00 << 0));		// Output drive capability
							//  00: 1x IOL/IOH Enable
							//  01: 2x IOL/IOH Enable
							//  10: 3x IOL/IOH Enable
							//  11: 4x IOL/IOH Enable
	//-------------------------------------------------------------------------
	WriteReg(Reg0E_COM5,
		0x61);				// Reserved
	WriteReg(Reg0F_COM6,
		(0b0 << 7) |		// Output of optical black line option
		(0b1 << 1));		// Reset all timing when format changes
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
	WriteReg(Reg92_DM_LNL,
		0x00);				// Dummy Line low 8 bits
	WriteReg(Reg93_DM_LNH,
		0x00);				// Dummy Line high 8 bits
	WriteReg(Reg3D_COM13,
		(0b1 << 7) |		// Gamma enable
		(0b1 << 6) |		// UV saturation level - UV auto-adjustment
		(0b0 << 0));		// UV swap
	WriteReg(Reg4B,
		(0b1 << 0));		// UV average enable
#endif
}

Image& OV7670::Capture()
{
	sm_.set_enabled(false);
	sm_.clear_fifos().exec(programToReset_);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(image_.GetPointer())
		.set_trans_count_trig(image_.GetBytesBuff() / sizeof(uint32_t));
	sm_.set_enabled();
	sm_.put(image_.GetWidth() * 2 - 1);
	while (pChannel_->is_busy()) Tickable::Tick();
	return image_;
}

}
