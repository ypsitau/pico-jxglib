//==============================================================================
// jxglib/Camera/OV7670.h
//==============================================================================
#ifndef PICO_JXGLIB_CAMERA_OV7670_H
#define PICO_JXGLIB_CAMERA_OV7670_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"
#include "jxglib/PWM.h"
#include "jxglib/Camera.h"

namespace jxglib::Camera {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
class OV7670 : public Base {
public:
	enum Resolution { VGA, QVGA, QQVGA, CIF, QCIF, QQCIF };
	enum Format { RawBayerRGB, ProcessedBayerRGB, YUV422, GRB422, RGB565, RGB555, RGB444 };
	struct ResolutionSetting {
		uint8_t Reg11_CLKRC;
		uint8_t Reg12_COM7;
		uint8_t Reg0C_COM3;
		uint8_t Reg3E_COM14;
		uint8_t Reg70_SCALING_XSC;
		uint8_t Reg71_SCALING_YSC;
		uint8_t Reg72_SCALING_DCWCTR;
		uint8_t Reg73_SCALING_PCLK_DIV;
		uint8_t RegA2_SCALING_PCLK_DELAY;
	};
	struct FormatSetting {
		uint8_t Reg12_COM7;
		uint8_t Reg40_COM15;
		uint8_t Reg8C_RGB444;
	};
	struct PinAssign {
		const GPIO& DIN0;
		const GPIO& XLK;
		const GPIO& PLK;
		const GPIO& HREF;
		const GPIO& VSYNC;
	};
	static const uint8_t Reg00_GAIN					= 0x00;
	static const uint8_t Reg01_BLUE					= 0x01;
	static const uint8_t Reg02_RED					= 0x02;
	static const uint8_t Reg03_VREF					= 0x03;
	static const uint8_t Reg04_COM1					= 0x04;
	static const uint8_t Reg05_BAVE					= 0x05;
	static const uint8_t Reg06_GBAVE				= 0x06;
	static const uint8_t Reg07_AECHH				= 0x07;
	static const uint8_t Reg08_RAVE					= 0x08;
	static const uint8_t Reg09_COM2					= 0x09;
	static const uint8_t Reg0A_PID					= 0x0a;
	static const uint8_t Reg0B_VER					= 0x0b;
	static const uint8_t Reg0C_COM3					= 0x0c;
	static const uint8_t Reg0D_COM4					= 0x0d;
	static const uint8_t Reg0E_COM5					= 0x0e;
	static const uint8_t Reg0F_COM6					= 0x0f;
	static const uint8_t Reg10_AECH					= 0x10;
	static const uint8_t Reg11_CLKRC				= 0x11;
	static const uint8_t Reg12_COM7					= 0x12;
	static const uint8_t Reg13_COM8					= 0x13;
	static const uint8_t Reg14_COM9					= 0x14;
	static const uint8_t Reg15_COM10				= 0x15;
	static const uint8_t Reg17_HSTART				= 0x17;
	static const uint8_t Reg18_HSTOP				= 0x18;
	static const uint8_t Reg19_VSTRT				= 0x19;
	static const uint8_t Reg1A_VSTOP				= 0x1a;
	static const uint8_t Reg1B_PSHFT				= 0x1b;
	static const uint8_t Reg1C_MIDH					= 0x1c;
	static const uint8_t Reg1D_MIDL					= 0x1d;
	static const uint8_t Reg1E_MVFP					= 0x1e;
	static const uint8_t Reg1F_LAEC					= 0x1f;
	static const uint8_t Reg20_ADCCTR0				= 0x20;
	static const uint8_t Reg21_ADCCTR1				= 0x21;
	static const uint8_t Reg22_ADCCTR2				= 0x22;
	static const uint8_t Reg23_ADCCTR3				= 0x23;
	static const uint8_t Reg24_AEW					= 0x24;
	static const uint8_t Reg25_AEB					= 0x25;
	static const uint8_t Reg26_VPT					= 0x26;
	static const uint8_t Reg27_BBIAS				= 0x27;
	static const uint8_t Reg28_GBBIAS				= 0x28;
	static const uint8_t Reg2A_EXHCH				= 0x2a;
	static const uint8_t Reg2B_EXHCL				= 0x2b;
	static const uint8_t Reg2C_RBIAS				= 0x2c;
	static const uint8_t Reg2D_ADVFL				= 0x2d;
	static const uint8_t Reg2E_ADVFH				= 0x2e;
	static const uint8_t Reg2F_YAVE					= 0x2f;
	static const uint8_t Reg30_HSYST				= 0x30;
	static const uint8_t Reg31_HSYEN				= 0x31;
	static const uint8_t Reg32_HREF					= 0x32;
	static const uint8_t Reg33_CHLF					= 0x33;
	static const uint8_t Reg34_ARBLM				= 0x34;
	static const uint8_t Reg37_ADC					= 0x37;
	static const uint8_t Reg38_ACOM					= 0x38;
	static const uint8_t Reg39_OFON					= 0x39;
	static const uint8_t Reg3A_TSLB					= 0x3a;
	static const uint8_t Reg3B_COM11				= 0x3b;
	static const uint8_t Reg3C_COM12				= 0x3c;
	static const uint8_t Reg3D_COM13				= 0x3d;
	static const uint8_t Reg3E_COM14				= 0x3e;
	static const uint8_t Reg3F_EDGE					= 0x3f;
	static const uint8_t Reg40_COM15				= 0x40;
	static const uint8_t Reg41_COM16				= 0x41;
	static const uint8_t Reg42_COM17				= 0x42;
	static const uint8_t Reg43_AWBC1				= 0x43;
	static const uint8_t Reg44_AWBC2				= 0x44;
	static const uint8_t Reg45_AWBC3				= 0x45;
	static const uint8_t Reg46_AWBC4				= 0x46;
	static const uint8_t Reg47_AWBC5				= 0x47;
	static const uint8_t Reg48_AWBC6				= 0x48;
	static const uint8_t Reg4B						= 0x4b;
	static const uint8_t Reg4C_DNSTH				= 0x4c;
	static const uint8_t Reg4D_DM_POS				= 0x4d;
	static const uint8_t Reg4F_MTX1					= 0x4f;
	static const uint8_t Reg50_MTX2					= 0x50;
	static const uint8_t Reg51_MTX3					= 0x51;
	static const uint8_t Reg52_MTX4					= 0x52;
	static const uint8_t Reg53_MTX5					= 0x53;
	static const uint8_t Reg54_MTX6					= 0x54;
	static const uint8_t Reg55_BRIGHT				= 0x55;
	static const uint8_t Reg56_CONTRAS				= 0x56;
	static const uint8_t Reg57_CONTRAS_CENTER		= 0x57;
	static const uint8_t Reg58_MTXS					= 0x58;
	static const uint8_t Reg59_AWBC7				= 0x59;
	static const uint8_t Reg5A_AWBC8				= 0x5a;
	static const uint8_t Reg5B_AWBC9				= 0x5b;
	static const uint8_t Reg5C_AWBC10				= 0x5c;
	static const uint8_t Reg5D_AWBC11				= 0x5d;
	static const uint8_t Reg5E_AWBC12				= 0x5e;
	static const uint8_t Reg5F_B_LMT				= 0x5f;
	static const uint8_t Reg60_R_LMT				= 0x60;
	static const uint8_t Reg61_G_LMT				= 0x61;
	static const uint8_t Reg62_LCC1					= 0x62;
	static const uint8_t Reg63_LCC2					= 0x63;
	static const uint8_t Reg64_LCC3					= 0x64;
	static const uint8_t Reg65_LCC4					= 0x65;
	static const uint8_t Reg66_LCC5					= 0x66;
	static const uint8_t Reg67_MANU					= 0x67;
	static const uint8_t Reg68_MANV					= 0x68;
	static const uint8_t Reg69_GFIX					= 0x69;
	static const uint8_t Reg6A_GGAIN				= 0x6a;
	static const uint8_t Reg6B_DBLV					= 0x6b;
	static const uint8_t Reg6C_AWBCTR3				= 0x6c;
	static const uint8_t Reg6D_AWBCTR2				= 0x6d;
	static const uint8_t Reg6E_AWBCTR1				= 0x6e;
	static const uint8_t Reg6F_AWBCTR0				= 0x6f;
	static const uint8_t Reg70_SCALING_XSC			= 0x70;
	static const uint8_t Reg71_SCALING_YSC			= 0x71;
	static const uint8_t Reg72_SCALING_DCWCTR		= 0x72;
	static const uint8_t Reg73_SCALING_PCLK_DIV		= 0x73;
	static const uint8_t Reg74						= 0x74;
	static const uint8_t Reg75						= 0x75;
	static const uint8_t Reg76						= 0x76;
	static const uint8_t Reg77						= 0x77;
	static const uint8_t Reg7A_SLOP					= 0x7a;
	static const uint8_t Reg7B_GAM1					= 0x7b;
	static const uint8_t Reg7C_GAM2					= 0x7c;
	static const uint8_t Reg7D_GAM3					= 0x7d;
	static const uint8_t Reg7E_GAM4					= 0x7e;
	static const uint8_t Reg7F_GAM5					= 0x7f;
	static const uint8_t Reg80_GAM6					= 0x80;
	static const uint8_t Reg81_GAM7					= 0x81;
	static const uint8_t Reg82_GAM8					= 0x82;
	static const uint8_t Reg83_GAM9					= 0x83;
	static const uint8_t Reg84_GAM10				= 0x84;
	static const uint8_t Reg85_GAM11				= 0x85;
	static const uint8_t Reg86_GAM12				= 0x86;
	static const uint8_t Reg87_GAM13				= 0x87;
	static const uint8_t Reg88_GAM14				= 0x88;
	static const uint8_t Reg89_GAM15				= 0x89;
	static const uint8_t Reg8C_RGB444				= 0x8c;
	static const uint8_t Reg92_DM_LNL				= 0x92;
	static const uint8_t Reg93_DM_LNH				= 0x93;
	static const uint8_t Reg94_LCC6					= 0x94;
	static const uint8_t Reg95_LCC7					= 0x95;
	static const uint8_t Reg9D_BD50ST				= 0x9d;
	static const uint8_t Reg9E_BD60ST				= 0x9e;
	static const uint8_t Reg9F_HAECC1				= 0x9f;
	static const uint8_t RegA0_HAECC2				= 0xa0;
	static const uint8_t RegA2_SCALING_PCLK_DELAY	= 0xa2;
	static const uint8_t RegA4_NT_CTRL				= 0xa4;
	static const uint8_t RegA5_BD50MAX				= 0xa5;
	static const uint8_t RegA6_HAECC3				= 0xa6;
	static const uint8_t RegA7_HAECC4				= 0xa7;
	static const uint8_t RegA8_HAECC5				= 0xa8;
	static const uint8_t RegA9_HAECC6				= 0xa9;
	static const uint8_t RegAA_HAECC7				= 0xaa;
	static const uint8_t RegAB_BD60MAX				= 0xab;
	static const uint8_t RegAC_STR_OPT				= 0xac;
	static const uint8_t RegAD_STR_R				= 0xad;
	static const uint8_t RegAE_STR_G				= 0xae;
	static const uint8_t RegAF_STR_B				= 0xaf;
	static const uint8_t RegB0						= 0xb0;
	static const uint8_t RegB1_ABLC1				= 0xb1;
	static const uint8_t RegB3_THL_ST				= 0xb3;
	static const uint8_t RegB5_THL_DLT				= 0xb5;
	static const uint8_t RegBE_AD_CHB				= 0xbe;
	static const uint8_t RegBF_AD_CHR				= 0xbf;
	static const uint8_t RegC0_AD_CHGB				= 0xc0;
	static const uint8_t RegC1_AD_CHGR				= 0xc1;
	static const uint8_t RegC9_SATCTR				= 0xc9;
public:
	static const ResolutionSetting resolutionSetting_VGA;
	static const ResolutionSetting resolutionSetting_QVGA;
	static const ResolutionSetting resolutionSetting_QQVGA;
	static const ResolutionSetting resolutionSetting_CIF;
	static const ResolutionSetting resolutionSetting_QCIF;
	static const ResolutionSetting resolutionSetting_QQCIF;
public:
	static const FormatSetting formatSetting_RawBayerRGB;
	static const FormatSetting formatSetting_ProcessedBayerRGB;
	static const FormatSetting formatSetting_YUV422;
	static const FormatSetting formatSetting_GRB422;
	static const FormatSetting formatSetting_RGB565;
	static const FormatSetting formatSetting_RGB555;
	static const FormatSetting formatSetting_RGB444;
public:
	static const uint8_t I2CAddr = 0x21;
private:
	Resolution resolution_;
	Format format_;
	i2c_inst_t* i2c_;
	PinAssign pinAssign_;
	uint32_t freq_;
	PIO::StateMachine sm_;
	PIO::Program program_;
	PIO::Program programToReset_;
	DMA::Channel* pChannel_;
	DMA::ChannelConfig channelConfig_;
	Image image_;
public:
	OV7670(Resolution resolution, Format format, i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq = 24000000);
public:
	bool Initialize();
	void SetupRegisters();
public:
	OV7670& WriteReg(uint8_t reg, uint8_t value);
	OV7670& WriteReg(uint8_t reg, uint8_t mask, uint8_t value);
	OV7670& WriteRegBit(uint8_t reg, int iBit, bool value) { return WriteReg(reg, (0b1 << iBit), (value << iBit)); }
	uint8_t ReadReg(uint8_t reg);
	bool ReadRegBit(uint8_t reg, int iBit) { return (ReadReg(reg) >> iBit) & 0b1; }
	void ReadRegs(uint8_t reg, uint8_t values[], int count);
public:
	OV7670& ResetAllRegisters();
public:
	// virtual functions of Base
	virtual const Image& Capture() override;
	virtual const char* GetName() const override { return "OV7670"; }
	virtual const char* GetRemarks(char* buff, int lenMax) const override;
};

}

#endif
