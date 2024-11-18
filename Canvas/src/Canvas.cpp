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
	SetCapacity(drawableOut.GetFormat(), drawableOut.GetWidth(), drawableOut.GetHeight());
	pDrawableOut_ = &drawableOut;
	image_.Alloc(drawableOut.GetFormat(), drawableOut.GetWidth(), drawableOut.GetHeight());
	return true;
}

void Canvas::Refresh_()
{
	if (!pDrawableOut_) return;
	pDrawableOut_->DrawImage(0, 0, image_, nullptr, ImageDir::HorzFromNW);
	pDrawableOut_->Refresh();
}

void Canvas::Fill_(const Color& color)
{
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(image_, 0, 0, image_.GetWidth(), image_.GetHeight()));

}


void Canvas::DrawPixel_(int x, int y, const Color& color)
{
}

void Canvas::DrawRectFill_(int x, int y, int width, int height, const Color& color)
{
}

void Canvas::DrawBitmap_(int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
}

void Canvas::DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir)
{
}


}
