//==============================================================================
// Canvas.cpp
//==============================================================================
#include "jxglib/Canvas.h"

namespace jxglib {

template<typename T> void Swap(T* a, T* b) {
	T tmp = *a;
	*a = *b, *b = tmp;
}

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
const Canvas::DispatcherNone Canvas::dispatcherNone;
const Canvas::DispatcherRGB565 Canvas::dispatcherRGB565;

bool Canvas::AttachOutput(Drawable& drawableOut, const Rect* pRect, AttachDir attachDir)
{
	const Format& format = drawableOut.GetFormat();
	int width, height;
	if (pRect) {
		width = pRect->width, height = pRect->height;
	} else {
		width = drawableOut.GetWidth(), height = drawableOut.GetHeight();
	}
	if (attachDir == AttachDir::Rotate90 || attachDir == AttachDir::Rotate270) Swap(&width, &height);
	SetCapacity(format, width, height);
	output_.rect = pRect? *pRect : Rect(0, 0, width, height);
	output_.attachDir = attachDir;
	pDrawableOut_ = &drawableOut;
	imageOwn_.Alloc(format, width, height);
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
	pDrawableOut_->DrawImage(0, 0, imageOwn_, &output_.rect, output_.attachDir);
	pDrawableOut_->Refresh();
}

//------------------------------------------------------------------------------
// Canvas::DispatcherRGB565
//------------------------------------------------------------------------------
void Canvas::DispatcherRGB565::Fill_(Canvas& canvas, const Color& color) const
{
	Image& imageOwn = canvas.GetImageOwn();
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, 0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}


void Canvas::DispatcherRGB565::DrawPixel_(Canvas& canvas, int x, int y, const Color& color) const
{
}

void Canvas::DispatcherRGB565::DrawRectFill_(Canvas& canvas, int x, int y, int width, int height, const Color& color) const
{
	Image& imageOwn = canvas.GetImageOwn();
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

void Canvas::DispatcherRGB565::DrawBitmap_(Canvas& canvas, int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY) const
{
	Image& imageOwn = canvas.GetImageOwn();
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
				uint8_t bit = bits & 0x80;
				if (bit || pColorBg) {
					const ColorRGB565& color = bit? colorFg : colorBg;
					for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) writer.WriteForward(color);
				} else {
					writer.MoveForward(scaleX);
				}
			}
		}
		pSrcLeft = pSrc;
	}
}

void Canvas::DispatcherRGB565::DrawImage_(Canvas& canvas, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const
{
	Image& imageOwn = canvas.GetImageOwn();
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
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir, true));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir, true));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGBA>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir, true));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB565>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir, true));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	}
}

void Canvas::DispatcherRGB565::ScrollHorz_(Canvas& canvas, DirHorz dirHorz, int wdScroll, const Rect* pRect) const
{
	Image& imageOwn = canvas.GetImageOwn();
	Rect rect = pRect? *pRect : Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight());
	if (rect.width <= wdScroll) return;
	uint16_t* pDstPixel;
	const uint16_t* pSrcPixel;
	int advancePerPixel;
	int advancePerLine = imageOwn.GetWidth();
	if (dirHorz == DirHorz::Left) {
		pDstPixel = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y));
		pSrcPixel = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x + wdScroll, rect.y));
		advancePerPixel = 1;
	} else if (dirHorz == DirHorz::Right) {
		pDstPixel = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x + rect.width - 1, rect.y));
		pSrcPixel = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x + rect.width - 1 - wdScroll, rect.y));
		advancePerPixel = -1;
	} else {
		return;
	}
	for (int nPixels = rect.width - wdScroll - 1; nPixels > 0; nPixels--) {
		uint16_t* pDstLine = pDstPixel;
		const uint16_t* pSrcLine = pSrcPixel;
		for (int nLines = rect.height; nLines > 0; nLines--) {
			*pDstLine = *pSrcLine;
			pDstLine += advancePerLine, pSrcLine += advancePerLine;
		}
		pDstPixel += advancePerPixel;
		pSrcPixel += advancePerPixel;
	}
}

void Canvas::DispatcherRGB565::ScrollVert_(Canvas& canvas, DirVert dirVert, int htScroll, const Rect* pRect) const
{
	Image& imageOwn = canvas.GetImageOwn();
	Rect rect = pRect? *pRect : Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight());
	if (rect.height <= htScroll) return;
	uint16_t* pDstLine;
	const uint16_t* pSrcLine;
	int advancePerLine;
	const int advancePerPixel = 1;
	if (dirVert == DirVert::Up) {
		pDstLine = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y));
		pSrcLine = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + htScroll));
		advancePerLine = imageOwn.GetWidth();
	} else if (dirVert == DirVert::Down) {
		pDstLine = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + rect.height - 1));
		pSrcLine = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + rect.height - 1 - htScroll));
		advancePerLine = -imageOwn.GetWidth();
	} else {
		return;
	}
	for (int nLines = rect.height - htScroll - 1; nLines > 0; nLines--) {
		uint16_t* pDstPixel = pDstLine;
		const uint16_t* pSrcPixel = pSrcLine;
		for (int nPixels = rect.width; nPixels > 0; nPixels--) {
			*pDstPixel = *pSrcPixel;
			pDstPixel += advancePerPixel, pSrcPixel += advancePerPixel;
		}
		pDstLine += advancePerLine;
		pSrcLine += advancePerLine;
	}
}

}
