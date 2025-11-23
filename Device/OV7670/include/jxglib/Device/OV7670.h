//==============================================================================
// jxglib/Device/OV7670.h
//==============================================================================
#ifndef PICO_JXGLIB_DEVICE_OV7670_H
#define PICO_JXGLIB_DEVICE_OV7670_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"
#include "jxglib/PWM.h"
#include "jxglib/Image.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
class OV7670 {
public:
	struct PinAssign {
		const GPIO& DIN0;
		const GPIO& XLK;
		const GPIO& PLK;
		const GPIO& HREF;
		const GPIO& VSYNC;
	};
	struct Reg {
		static const uint8_t GAIN		= 0x00;
		static const uint8_t BLUE		= 0x01;
		static const uint8_t RED		= 0x02;
		static const uint8_t VREF		= 0x03;
		static const uint8_t COM1		= 0x04;
		static const uint8_t BAVE		= 0x05;
		static const uint8_t GBAVE		= 0x06;
		static const uint8_t AECHH		= 0x07;
		static const uint8_t RAVE		= 0x08;
		static const uint8_t COM2		= 0x09;
		static const uint8_t PID		= 0x0a;
		static const uint8_t VER		= 0x0b;
		static const uint8_t COM3		= 0x0c;
		static const uint8_t COM4		= 0x0d;
		static const uint8_t COM5		= 0x0e;
		static const uint8_t COM6		= 0x0f;
		static const uint8_t AECH		= 0x10;
		static const uint8_t CLKRC		= 0x11;
		static const uint8_t COM7		= 0x12;
		static const uint8_t COM8		= 0x13;
		static const uint8_t COM9		= 0x14;
		static const uint8_t COM10		= 0x15;
		static const uint8_t HSTART		= 0x17;
		static const uint8_t HSTOP		= 0x18;
		static const uint8_t VSTRT		= 0x19;
		static const uint8_t VSTOP		= 0x1a;
		static const uint8_t PSHFT		= 0x1b;
		static const uint8_t MIDH		= 0x1c;
		static const uint8_t MIDL		= 0x1d;
		static const uint8_t MVFP		= 0x1e;
		static const uint8_t LAEC		= 0x1f;
		static const uint8_t ADCCTR0	= 0x20;
		static const uint8_t ADCCTR1	= 0x21;
		static const uint8_t ADCCTR2	= 0x22;
		static const uint8_t ADCCTR3	= 0x23;
		static const uint8_t AEW		= 0x24;
		static const uint8_t AEB		= 0x25;
		static const uint8_t VPT		= 0x26;
		static const uint8_t BBIAS		= 0x27;
		static const uint8_t GBBIAS		= 0x28;
		static const uint8_t EXHCH		= 0x2a;
		static const uint8_t EXHCL		= 0x2b;
		static const uint8_t RBIAS		= 0x2c;
		static const uint8_t ADVFL		= 0x2d;
		static const uint8_t ADVFH		= 0x2e;
		static const uint8_t YAVE		= 0x2f;
		static const uint8_t HSYST		= 0x30;
		static const uint8_t HSYEN		= 0x31;
		static const uint8_t HREF		= 0x32;
		static const uint8_t CHLF		= 0x33;
		static const uint8_t ARBLM		= 0x34;
		static const uint8_t ADC		= 0x37;
		static const uint8_t ACOM		= 0x38;
		static const uint8_t OFON		= 0x39;
		static const uint8_t TSLB		= 0x3a;
		static const uint8_t COM11		= 0x3b;
		static const uint8_t COM12		= 0x3c;
		static const uint8_t COM13		= 0x3d;
		static const uint8_t COM14		= 0x3e;
		static const uint8_t EDGE		= 0x3f;
		static const uint8_t COM15		= 0x40;
		static const uint8_t COM16		= 0x41;
		static const uint8_t COM17		= 0x42;
		static const uint8_t AWBC1		= 0x43;
		static const uint8_t AWBC2		= 0x44;
		static const uint8_t AWBC3		= 0x45;
		static const uint8_t AWBC4		= 0x46;
		static const uint8_t AWBC5		= 0x47;
		static const uint8_t AWBC6		= 0x48;
		static const uint8_t REG4B		= 0x4b;
		static const uint8_t DNSTH		= 0x4c;
		static const uint8_t MTX1		= 0x4f;
		static const uint8_t MTX2		= 0x50;
		static const uint8_t MTX3		= 0x51;
		static const uint8_t MTX4		= 0x52;
		static const uint8_t MTX5		= 0x53;
		static const uint8_t MTX6		= 0x54;
		static const uint8_t BRIGHT		= 0x55;
		static const uint8_t CONTRAS	= 0x56;
		static const uint8_t CONTRAS_CENTER = 0x57;
		static const uint8_t MTXS		= 0x58;
		static const uint8_t LCC1		= 0x62;
		static const uint8_t LCC2		= 0x63;
		static const uint8_t LCC3		= 0x64;
		static const uint8_t LCC4		= 0x65;
		static const uint8_t LCC5		= 0x66;
		static const uint8_t MANU		= 0x67;
		static const uint8_t MANV		= 0x68;
		static const uint8_t GFIX		= 0x69;
		static const uint8_t GGAIN		= 0x6a;
		static const uint8_t DBLV		= 0x6b;
		static const uint8_t AWBCTR3	= 0x6c;
		static const uint8_t AWBCTR2	= 0x6d;
		static const uint8_t AWBCTR1	= 0x6e;
		static const uint8_t AWBCTR0	= 0x6f;
		static const uint8_t SCALING_XSC	= 0x70;
		static const uint8_t SCALING_YSC	= 0x71;
		static const uint8_t SCALING_DCWCTR	= 0x72;
		static const uint8_t SCALING_PCLK_DIV	= 0x73;
		static const uint8_t REG74		= 0x74;
		static const uint8_t REG75		= 0x75;
		static const uint8_t REG76		= 0x76;
		static const uint8_t REG77		= 0x77;
		static const uint8_t SLOP		= 0x7a;
		static const uint8_t GAM1		= 0x7b;
		static const uint8_t GAM2		= 0x7c;
		static const uint8_t GAM3		= 0x7d;
		static const uint8_t GAM4		= 0x7e;
		static const uint8_t GAM5		= 0x7f;
		static const uint8_t GAM6		= 0x80;
		static const uint8_t GAM7		= 0x81;
		static const uint8_t GAM8		= 0x82;
		static const uint8_t GAM9		= 0x83;
		static const uint8_t GAM10		= 0x84;
		static const uint8_t GAM11		= 0x85;
		static const uint8_t GAM12		= 0x86;
		static const uint8_t GAM13		= 0x87;
		static const uint8_t GAM14		= 0x88;
		static const uint8_t GAM15		= 0x89;
		static const uint8_t RGB444		= 0x8c;
		static const uint8_t DM_LNL		= 0x92;
		static const uint8_t DM_LNH		= 0x93;
		static const uint8_t LCC6		= 0x94;
		static const uint8_t LCC7		= 0x95;
		static const uint8_t BD50ST		= 0x9d;
		static const uint8_t BD60ST		= 0x9e;
		static const uint8_t HAECC1		= 0x9f;
		static const uint8_t HAECC2		= 0xa0;
		static const uint8_t SCALING_PCLK_DELAY	= 0xa2;
		static const uint8_t NT_CTRL	= 0xa4;
		static const uint8_t BD50MAX	= 0xa5;
		static const uint8_t HAECC3		= 0xa6;
		static const uint8_t HAECC4		= 0xa7;
		static const uint8_t HAECC5		= 0xa8;
		static const uint8_t HAECC6		= 0xa9;
		static const uint8_t HAECC7		= 0xaa;
		static const uint8_t BD60MAX	= 0xab;
		static const uint8_t STR_OPT	= 0xac;
		static const uint8_t STR_R		= 0xad;
		static const uint8_t STR_G		= 0xae;
		static const uint8_t STR_B		= 0xaf;
		static const uint8_t ABLC1		= 0xb1;
		static const uint8_t THL_ST		= 0xb3;
		static const uint8_t THL_DLT	= 0xb5;
		static const uint8_t AD_CHB		= 0xbe;
		static const uint8_t AD_CHR		= 0xbf;
		static const uint8_t AD_CHGB	= 0xc0;
		static const uint8_t AD_CHGR	= 0xc1;
		static const uint8_t SATCTR		= 0xc9;
	};
public:
	static const uint8_t I2CAddr = 0x21;
private:
	i2c_inst_t* i2c_;
	PinAssign pinAssign_;
	uint32_t freq_;
	PIO::StateMachine sm_;
	PIO::Program program_;
	PIO::Program programToReset_;
	DMA::Channel* pChannel_;
	DMA::ChannelConfig channelConfig_;
public:
	OV7670(i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq = 24000000);
public:
	void Run();
	bool IsRunning() const { return sm_.IsValid(); }
public:
	void WriteReg(uint8_t reg, uint8_t value);
	uint8_t ReadReg(uint8_t reg);
public:
	OV7670& Capture(Image& image);
};

}

#endif
