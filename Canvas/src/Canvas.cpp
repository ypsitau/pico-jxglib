//==============================================================================
// Canvas.cpp
//==============================================================================
#include "jxglib/Canvas.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
const Canvas::DispatcherNone Canvas::dispatcherNone;
const Canvas::DispatcherRGB565 Canvas::dispatcherRGB565;

bool Canvas::AttachOutput(Drawable& drawableOut)
{
	const Format& format = drawableOut.GetFormat();
	SetCapacity(format, drawableOut.GetWidth(), drawableOut.GetHeight());
	pDrawableOut_ = &drawableOut;
	imageOwn_.Alloc(format, drawableOut.GetWidth(), drawableOut.GetHeight());
	imageOwn_.FillZero();
	if (format.IsBitmap()) {
	} else if (format.IsGray()) {
	} else if (format.IsRGB()) {
	} else if (format.IsRGBA()) {
	} else if (format.IsRGB565()) {
		pDispatcher_ = &dispatcherRGB565;
	}
	return true;
}

void Canvas::Refresh_()
{
	if (!pDrawableOut_) return;
	pDrawableOut_->DrawImage(0, 0, imageOwn_, nullptr, ImageDir::HorzFromNW);
	pDrawableOut_->Refresh();
}

//------------------------------------------------------------------------------
// Canvas::DispatcherRGB565
//------------------------------------------------------------------------------
void Canvas::DispatcherRGB565::Fill_(Image& imageOwn, const Color& color) const
{
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, 0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}


void Canvas::DispatcherRGB565::DrawPixel_(Image& imageOwn, int x, int y, const Color& color) const
{
}

void Canvas::DispatcherRGB565::DrawRectFill_(Image& imageOwn, int x, int y, int width, int height, const Color& color) const
{
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

void Canvas::DispatcherRGB565::DrawBitmap_(Image& imageOwn, int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY) const
{
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	const ColorRGB565 colorFg(color);
	const ColorRGB565 colorBg(pColorBg? ColorRGB565(*pColorBg) : ColorRGB565::black);
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Image::Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	for (int iRow = 0; iRow < height; iRow++) {
		const uint8_t* pSrc;
		for (int iScaleY = 0; iScaleY < scaleY; iScaleY++) {
			pSrc = pSrcLeft;
			int iBit = 0;
			uint8_t bits = *pSrc++;
			for (int iCol = 0; iCol < width; iCol++, iBit++, bits <<= 1) {
				if (iBit == 8) {
					iBit = 0;
					bits = *pSrc++;
				}
				const ColorRGB565& color = (bits & 0x80)? colorFg : colorBg;
				for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) writer.WriteForward(color);
			}
		}
		pSrcLeft = pSrc;
	}
}

void Canvas::DispatcherRGB565::DrawImage_(Image& imageOwn, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const
{
	int xSkip = 0, ySkip = 0;
	int width, height;
	if (Image::IsDirHorz(imageDir)) {
		width = image.GetWidth(), height = image.GetHeight();
	} else {
		width = image.GetHeight(), height = image.GetWidth();
	}
	if (!AdjustRange(&x, &width, 0, imageOwn.GetWidth(), &xSkip)) return;
	if (!AdjustRange(&y, &height, 0, imageOwn.GetHeight(), &ySkip)) return;
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Image::Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcGray>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGBA>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB565>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	}
}

}
