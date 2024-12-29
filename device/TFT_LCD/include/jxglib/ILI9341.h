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
		TouchScreen(spi_inst_t* spi, const PinAssign& pinAssign) : TSC2046(spi, pinAssign) {}
	public:
		void Initialize(Drawable& drawable) {
			Drawable::Dir dir = drawable.GetDirection();
			TSC2046::Initialize(dir.IsVert());
			if (dir.IsRotate0()) {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.14, -20));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, -0.18, 350));
			} else if (dir.IsRotate90()) {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, -0.18, 350));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, -0.14, 260));
			} else if (dir.IsRotate180()) {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, -0.14, 260));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.18, -30));
			} else if (dir.IsRotate270()) {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.18, -30));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.14, -20));
			} else {
				SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.14, -20));
				SetAdjusterY(Adjuster(drawable.GetHeight() - 1, -0.18, 350));
			}
		}
	};
public:
	ILI9341(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, Format::RGB565, 240, 320, width, height, pinAssign) {}
	inline void Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ILI9341;
};

inline void ILI9341::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.rgbInterfaceFormat		= RGBInterfaceFormat::BPP16,
		.mcuInterfaceFormat		= MCUInterfaceFormat::BPP16,
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= true,
		.invertVertFlag			= false,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
