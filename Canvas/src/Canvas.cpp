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
bool Canvas::AttachOutput(Drawable& drawableOut, const Rect* pRect, AttachDir attachDir)
{
	const Format& format = drawableOut.GetFormat();
	int width, height;
	if (pRect) {
		width = pRect->width, height = pRect->height;
		output_.rect = *pRect;
	} else {
		if (Image::IsDirHorz(attachDir)) {
			width = drawableOut.GetWidth(), height = drawableOut.GetHeight();
		} else {
			width = drawableOut.GetHeight(), height = drawableOut.GetWidth();
		}
		output_.rect = Rect(0, 0, drawableOut.GetWidth(), drawableOut.GetHeight());
	}
	SetCapacity(format, width, height);
	output_.attachDir = attachDir;
	pDrawableOut_ = &drawableOut;
	imageOwn_.Alloc(format, width, height);
	imageOwn_.FillZero();
	if (format.IsBitmap()) {
	} else if (format.IsGray()) {
	} else if (format.IsRGB()) {
	} else if (format.IsRGBA()) {
	} else if (format.IsRGB565()) {
		pDispatcher_.reset(new DispatcherRGB565(*this));
	}
	return true;
}

//------------------------------------------------------------------------------
// Canvas::DispatcherEx
//------------------------------------------------------------------------------
void Canvas::DispatcherEx::Initialize()
{
	// do nothing
}

void Canvas::DispatcherEx::Refresh()
{
	Drawable* pDrawableOut = canvas_.GetDrawableOut();
	Image& imageOwn = canvas_.GetImageOwn();
	const Output& output = canvas_.GetOutput();
	if (!pDrawableOut) return;
	pDrawableOut->DrawImage(0, 0, imageOwn, &output.rect, output.attachDir);
	pDrawableOut->Refresh();
}

//------------------------------------------------------------------------------
// Canvas::DispatcherRGB565
//------------------------------------------------------------------------------
void Canvas::DispatcherRGB565::Fill(const Color& color)
{
	Image& imageOwn = canvas_.GetImageOwn();
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, 0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}


void Canvas::DispatcherRGB565::DrawPixel(int x, int y, const Color& color)
{
}

void Canvas::DispatcherRGB565::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	Image& imageOwn = canvas_.GetImageOwn();
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	ColorRGB565 colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

void Canvas::DispatcherRGB565::DrawBitmap(int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	Image& imageOwn = canvas_.GetImageOwn();
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	const ColorRGB565 colorFg(color);
	const ColorRGB565 colorBg(pColorBg? ColorRGB565(*pColorBg) : ColorRGB565::black);
	using Writer = Image::Writer<Image::PutColorRGB565_DstRGB565>;
	Image::Writer writer(Writer::HorzFromNW(imageOwn, x, y, width * scaleX, height * scaleY));
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

void Canvas::DispatcherRGB565::DrawImage(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir)
{
	Image& imageOwn = canvas_.GetImageOwn();
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

void Canvas::DispatcherRGB565::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect* pRect)
{
	Image& imageOwn = canvas_.GetImageOwn();
	Rect rect = pRect? *pRect : Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight());
	if (rect.width <= wdScroll) return;
	uint16_t* pDstPixel;
	const uint16_t* pSrcPixel;
	int advancePerPixel;
	int advancePerLine = imageOwn.GetWidth();
	Rect rectErase;
	if (dirHorz == DirHorz::Left) {
		pDstPixel = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y));
		pSrcPixel = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x + wdScroll, rect.y));
		advancePerPixel = 1;
		rectErase = Rect(rect.x + rect.width - wdScroll, rect.y, wdScroll, rect.height);
	} else if (dirHorz == DirHorz::Right) {
		pDstPixel = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x + rect.width - 1, rect.y));
		pSrcPixel = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x + rect.width - 1 - wdScroll, rect.y));
		advancePerPixel = -1;
		rectErase = Rect(rect.x, rect.y, wdScroll, rect.height);
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
	canvas_.DrawRectFill(rectErase, canvas_.GetColorBg());
}

void Canvas::DispatcherRGB565::ScrollVert(DirVert dirVert, int htScroll, const Rect* pRect)
{
	Image& imageOwn = canvas_.GetImageOwn();
	Rect rect = pRect? *pRect : Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight());
	if (rect.height <= htScroll) return;
	uint16_t* pDstLine;
	const uint16_t* pSrcLine;
	int advancePerLine;
	const int advancePerPixel = 1;
	Rect rectErase;
	if (dirVert == DirVert::Up) {
		pDstLine = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y));
		pSrcLine = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + htScroll));
		advancePerLine = imageOwn.GetWidth();
		rectErase = Rect(rect.x, rect.y + rect.height - htScroll, rect.width, htScroll);
	} else if (dirVert == DirVert::Down) {
		pDstLine = reinterpret_cast<uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + rect.height - 1));
		pSrcLine = reinterpret_cast<const uint16_t*>(imageOwn.GetPointer(rect.x, rect.y + rect.height - 1 - htScroll));
		advancePerLine = -imageOwn.GetWidth();
		rectErase = Rect(rect.x, rect.y, rect.width, htScroll);
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
	canvas_.DrawRectFill(rectErase, canvas_.GetColorBg());
}

}
