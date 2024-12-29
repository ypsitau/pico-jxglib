//==============================================================================
// jxglib/ST7735.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7735_H
#define PICO_JXGLIB_ST7735_H
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7735
//------------------------------------------------------------------------------
class ST7735 : public TFT_LCD {
public:
	ST7735(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, 128, 160, width, height, pinAssign) {}
	ST7735(spi_inst_t* spi, int width, int height, const PinAssignNoCS& pinAssign) :
			TFT_LCD(spi, 128, 160, width, height, pinAssign) {}
	inline void Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ST7735;
	class TypeB : public TFT_LCD {
	public:
		TypeB(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
				TFT_LCD(spi, 128, 160, width, height, pinAssign) {}
		TypeB(spi_inst_t* spi, int width, int height, const PinAssignNoCS& pinAssign) :
				TFT_LCD(spi, 128, 160, width, height, pinAssign) {}
		inline void Initialize(Dir displayDir = Dir::Normal);
	};
};

inline void ST7735::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= false,
		.invertVertFlag			= false,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

inline void ST7735::TypeB::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::RGB,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= true,
		.invertVertFlag			= true,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x02,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
