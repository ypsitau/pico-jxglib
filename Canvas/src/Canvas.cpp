//==============================================================================
// Canvas.cpp
//==============================================================================
#include "jxglib/Canvas.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
bool Canvas::AttachOutput(Drawable& drawableOut)
{
	const Format& format = drawableOut.GetFormat();
	SetCapacity(format, drawableOut.GetWidth(), drawableOut.GetHeight());
	pDrawableOut_ = &drawableOut;
	image_.Alloc(format, drawableOut.GetWidth(), drawableOut.GetHeight());
	image_.FillZero();
	if (format.IsBitmap()) {
	} else if (format.IsGray()) {
	} else if (format.IsRGB()) {
	} else if (format.IsRGBA()) {
	} else if (format.IsRGB565()) {
		pCore_ = &coreRGB565_;
	}
	return true;
}

void Canvas::Refresh_()
{
	if (!pDrawableOut_) return;
	pDrawableOut_->DrawImage(0, 0, image_, nullptr, ImageDir::HorzFromNW);
	pDrawableOut_->Refresh();
}

//------------------------------------------------------------------------------
// Canvas::CoreRGB565
//------------------------------------------------------------------------------
void Canvas::CoreRGB565::Fill_(const Color& color)
{
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(image_, 0, 0, image_.GetWidth(), image_.GetHeight()));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}


void Canvas::CoreRGB565::DrawPixel_(int x, int y, const Color& color)
{
}

void Canvas::CoreRGB565::DrawRectFill_(int x, int y, int width, int height, const Color& color)
{
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(image_, x, y, width, height));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

void Canvas::CoreRGB565::DrawBitmap_(int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
}

void Canvas::CoreRGB565::DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir)
{
}

}
