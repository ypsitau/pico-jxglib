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
		TouchScreen& Initialize(Drawable& drawable) {
			Drawable::Dir dir = drawable.GetDirection();
			TSC2046::Initialize(dir.IsVert());
			adjusterX_.Set(0.18, -26, 345);
			adjusterY_.Set(0.27, -41, 520);
			if (dir.IsVert()) Swap(&adjusterX_, &adjusterY_);
			adjusterX_.SetValueMax(drawable.GetWidth() - 1).SetNeg(dir.IsRightToLeft());
			adjusterY_.SetValueMax(drawable.GetHeight() - 1).SetNeg(dir.IsTopToBottom());
			return *this;
		}
	};
public:
	ILI9488(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, Format::RGB, 320, 480, width, height, pinAssign) {}
	inline TFT_LCD& Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ILI9488;
};

inline TFT_LCD& ILI9488::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		rgbInterfaceFormat:		RGBInterfaceFormat::BPP18,
		mcuInterfaceFormat:		MCUInterfaceFormat::BPP18,
		lineAddressOrder:		LineAddressOrder::TopToBottom,
		rgbBgrOrder:			RGBBGROrder::BGR,
		displayDataLatchOrder:	DisplayDataLatchOrder::LeftToRight,
		invertHorzFlag:			true,
		invertVertFlag:			false,
		displayInversionOnFlag:	false,
		gammaCurve:				0x01,
	};
	return TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
