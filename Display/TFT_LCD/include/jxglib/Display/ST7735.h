//==============================================================================
// jxglib/Display/ST7735.h
//==============================================================================
#ifndef PICO_JXGLIB_DISPLAY_ST7735_H
#define PICO_JXGLIB_DISPLAY_ST7735_H
#include "jxglib/Display/TFT_LCD.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// ST7735
//------------------------------------------------------------------------------
class ST7735 : public TFT_LCD {
public:
	ST7735(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, Format::RGB565, 128, 160, width, height, pinAssign) {}
	ST7735(spi_inst_t* spi, int width, int height, const PinAssignNoCS& pinAssign) :
			TFT_LCD(spi, Format::RGB565, 128, 160, width, height, pinAssign) {}
	inline TFT_LCD& Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ST7735;
	class TypeB : public TFT_LCD {
	public:
		TypeB(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
				TFT_LCD(spi, Format::RGB565, 128, 160, width, height, pinAssign) {}
		TypeB(spi_inst_t* spi, int width, int height, const PinAssignNoCS& pinAssign) :
				TFT_LCD(spi, Format::RGB565, 128, 160, width, height, pinAssign) {}
		inline TFT_LCD& Initialize(Dir displayDir = Dir::Normal);
	public:
		// Virtual functions of Display
		virtual const char* GetVariantName() const override { return "TypeB"; }
	};
public:
	// Virtual functions of Display
	virtual const char* GetName() const override { return "ST7735"; }
	virtual const char* GetVariantName() const override { return "TypeA"; }
};

inline TFT_LCD& ST7735::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.rgbInterfaceFormat		= RGBInterfaceFormat::BPP16,
		.mcuInterfaceFormat		= MCUInterfaceFormat::BPP16,
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= false,
		.invertVertFlag			= false,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	return TFT_LCD::Initialize(displayDir, configData);
}

inline TFT_LCD& ST7735::TypeB::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.rgbInterfaceFormat		= RGBInterfaceFormat::BPP16,
		.mcuInterfaceFormat		= MCUInterfaceFormat::BPP16,
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::RGB,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= true,
		.invertVertFlag			= true,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x02,
	};
	return TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
