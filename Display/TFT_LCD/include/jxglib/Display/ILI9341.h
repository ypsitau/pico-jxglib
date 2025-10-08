//==============================================================================
// jxglib/Display/ILI9341.h
//==============================================================================
#ifndef PICO_JXGLIB_DISPLAY_ILI9341_H
#define PICO_JXGLIB_DISPLAY_ILI9341_H
#include "jxglib/Display/TFT_LCD.h"
#include "jxglib/TouchScreen/TSC2046.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// ILI9341
//------------------------------------------------------------------------------
class ILI9341 : public TFT_LCD {
public:
	class TouchScreen : public jxglib::TouchScreen::TSC2046 {
	public:
		TouchScreen(spi_inst_t* spi, const PinAssign& pinAssign) : jxglib::TouchScreen::TSC2046(spi, pinAssign) {}
	public:
		TouchScreen& Initialize(Drawable& drawable) {
			Drawable::Dir dir = drawable.GetDirection();
			jxglib::TouchScreen::TSC2046::Initialize(dir.IsVert());
			adjusterX_.Set(0.14, -20, 260);
			adjusterY_.Set(0.18, -30, 350);
			if (dir.IsVert()) Swap(&adjusterX_, &adjusterY_);
			adjusterX_.SetValueMax(drawable.GetWidth() - 1).SetNeg(dir.IsRightToLeft());
			adjusterY_.SetValueMax(drawable.GetHeight() - 1).SetNeg(dir.IsTopToBottom());
			return *this;
		}
	};
public:
	ILI9341(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, Format::RGB565, 240, 320, width, height, pinAssign) {}
	virtual TFT_LCD& Initialize(Dir displayDir = Dir::Normal) override;
public:
	using TypeA = ILI9341;
public:	// Virtual functions of Display
	virtual const char* GetName() const override { return "ILI9341"; }
	virtual const char* GetVariantName() const override { return "TypeA"; }
};

inline TFT_LCD& ILI9341::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		rgbInterfaceFormat:		RGBInterfaceFormat::BPP16,
		mcuInterfaceFormat:		MCUInterfaceFormat::BPP16,
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
