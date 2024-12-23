//==============================================================================
// jxglib/ILI9341.h
//==============================================================================
#ifndef PICO_JXGLIB_ILI9341_H
#define PICO_JXGLIB_ILI9341_H
#include "jxglib/TFT_LCD.h"
#include "jxglib/TSC2046.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ILI9341
//------------------------------------------------------------------------------
class ILI9341 : public TFT_LCD {
public:
	class TouchScreen : public TSC2046 {
	public:
		TouchScreen(spi_inst_t* spi, const GPIO& gpio_CS, const GPIO& gpio_IRQ) : TSC2046(spi, gpio_CS, gpio_IRQ) {}
	public:
		void Initialize(Drawable& drawable) {
			TSC2046::Initialize(drawable.IsHVFlipped());
			if (drawable.IsHVFlipped()) {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.18, -38));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.14, -18));
			} else {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, -0.14, 257));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.18, -35));
			}
		}
	};
public:
	ILI9341(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_CS, const GPIO& gpio_BL) :
			TFT_LCD(spi, 240, 320, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	inline void Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ILI9341;
};

inline void ILI9341::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= false,
		.invertVertFlag			= true,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
