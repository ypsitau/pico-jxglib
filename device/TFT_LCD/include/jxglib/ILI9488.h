//==============================================================================
// jxglib/ILI9488.h
//==============================================================================
#ifndef PICO_JXGLIB_ILI9488_H
#define PICO_JXGLIB_ILI9488_H
#include "jxglib/TFT_LCD.h"
#include "jxglib/TSC2046.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ILI9488
//------------------------------------------------------------------------------
class ILI9488 : public TFT_LCD {
public:
	class TouchScreen : public TSC2046 {
	public:
		TouchScreen(spi_inst_t* spi, const PinAssign& pinAssign) : TSC2046(spi, pinAssign) {}
	public:
		void Initialize(Drawable& drawable) {
			Drawable::Dir dir = drawable.GetDirection();
			TSC2046::Initialize(dir.IsVert());
			if (dir.IsHorz()) {
				if (dir.IsLeftToRight()) {
					SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.18, -20));
				} else {
					SetAdjusterX(Adjuster(drawable.GetWidth() - 1, -0.18, 350));
				}
				if (dir.IsTopToBottom()) {
					SetAdjusterY(Adjuster(drawable.GetHeight() - 1, -0.26, 510));
				} else {
					SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.26, -30));
				}
			} else {
				if (dir.IsLeftToRight()) {
					SetAdjusterX(Adjuster(drawable.GetWidth() - 1, 0.26, -30));
				} else {
					SetAdjusterX(Adjuster(drawable.GetWidth() - 1, -0.26, 510));
				}
				if (dir.IsTopToBottom()) {
					SetAdjusterY(Adjuster(drawable.GetHeight() - 1, -0.18, 350));
				} else {
					SetAdjusterY(Adjuster(drawable.GetHeight() - 1, 0.18, -20));
				}
			}
		}
	};
public:
	ILI9488(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, Format::RGB, 320, 480, width, height, pinAssign) {}
	inline void Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ILI9488;
};

inline void ILI9488::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.rgbInterfaceFormat		= RGBInterfaceFormat::BPP18,
		.mcuInterfaceFormat		= MCUInterfaceFormat::BPP18,
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
