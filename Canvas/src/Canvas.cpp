//==============================================================================
// Canvas.cpp
//==============================================================================
#include "jxglib/Canvas.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas::Dispatcher_T
//------------------------------------------------------------------------------
template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::Fill(const Color& color)
{
	Image& imageOwn = canvas_.GetImageOwn();
	using Writer = Image::Writer<Image::Setter_T<T_Color, T_Color> >;
	Writer writer(Writer::HorzFromNW(imageOwn, 0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()));
	T_Color colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::DrawPixel(int x, int y, const Color& color)
{
	Image& imageOwn = canvas_.GetImageOwn();
	T_Color colorDst(color);
	Image::Setter_T<T_Color, T_Color>().Set(imageOwn.GetPointer(x, y), colorDst);
}

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	Image& imageOwn = canvas_.GetImageOwn();
	using Writer = Image::Writer<Image::Setter_T<T_Color, T_Color> >;
	Writer writer(Writer::HorzFromNW(imageOwn, x, y, width, height));
	T_Color colorDst(color);
	while (!writer.HasDone()) writer.WriteForward(colorDst);
}

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::DrawBitmap(int x, int y, const void* data, int width, int height,
	const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	Image& imageOwn = canvas_.GetImageOwn();
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	const T_Color colorFg(color);
	const T_Color colorBg(pColorBg? T_Color(*pColorBg) : T_Color::black);
	using Writer = Image::Writer<Image::Setter_T<T_Color, T_Color> >;
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
					const T_Color& color = bit? colorFg : colorBg;
					for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) writer.WriteForward(color);
				} else {
					writer.MoveForward(scaleX);
				}
			}
		}
		pSrcLeft = pSrc;
	}
}

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
	Image& imageOwn = canvas_.GetImageOwn();
	drawDir = canvas_.GetOutput().attachDir.Transform(drawDir);
	int xSrc = rectClip.x, ySrc = rectClip.y;
	int wdImage = rectClip.width, htImage = rectClip.height;
	int xSkip = 0, ySkip = 0;

	//if (!AdjustRange(&x, &width, 0, imageOwn.GetWidth(), &xSkip)) return;
	//if (!AdjustRange(&y, &height, 0, imageOwn.GetHeight(), &ySkip)) return;
	
	using Writer = Image::Writer<Image::Setter_T<T_Color, T_Color> >;
	Image::Writer writer(Writer::Create(imageOwn, x, y, wdImage, htImage, drawDir));
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::Getter_T<T_Color, ColorGray> >;
		Image::Reader reader(Reader::Normal(image, xSkip, ySkip, wdImage, htImage));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::Getter_T<T_Color, Color> >;
		Image::Reader reader(Reader::Normal(image, xSkip, ySkip, wdImage, htImage));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::Getter_T<T_Color, ColorA> >;
		Image::Reader reader(Reader::Normal(image, xSkip, ySkip, wdImage, htImage));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::Getter_T<T_Color, ColorRGB565> >;
		Image::Reader reader(Reader::Normal(image, xSkip, ySkip, wdImage, htImage));
		while (!reader.HasDone()) writer.WriteForward(reader.ReadForward());
	}
}

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	Image& imageOwn = canvas_.GetImageOwn();
	Rect rect = rectClip.IsEmpty()? Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()) : rectClip;
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
	int nPixels = rect.width - wdScroll;
	for (int iPixel = 0; iPixel < nPixels; iPixel++) {
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

template<typename T_Color>
void Canvas::Dispatcher_T<T_Color>::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	Image& imageOwn = canvas_.GetImageOwn();
	Rect rect = rectClip.IsEmpty()? Rect(0, 0, imageOwn.GetWidth(), imageOwn.GetHeight()) : rectClip;
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
	int nLines = rect.height - htScroll;
	for (int iLine = 0; iLine < nLines; iLine++) {
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

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
bool Canvas::AttachOutput(Drawable& drawableOut, const Rect& rect, AttachDir attachDir)
{
	const Format& formatOut = drawableOut.GetFormat();
	int wdImage, htImage;
	if (rect.IsEmpty()) {
		if (attachDir.IsHorz()) {
			wdImage = drawableOut.GetWidth(), htImage = drawableOut.GetHeight();
		} else {
			wdImage = drawableOut.GetHeight(), htImage = drawableOut.GetWidth();
		}
		output_.rect = Rect(0, 0, drawableOut.GetWidth(), drawableOut.GetHeight());
	} else {
		if (attachDir.IsHorz()) {
			wdImage = rect.width, htImage = rect.height;
		} else {
			wdImage = rect.height, htImage = rect.width;
		}
		output_.rect = rect;
	}
	SetCapacity(formatOut, wdImage, htImage);
	output_.attachDir = attachDir;
	pDrawableOut_ = &drawableOut;
	if (!imageOwn_.Alloc(formatOut, wdImage, htImage)) return false;
	imageOwn_.FillZero();
	if (formatOut.IsBitmap()) {
		return false;
	} else if (formatOut.IsGray()) {
		//pDispatcherEx_.reset(new Dispatcher_T<ColorGray>(*this));
		return false;
	} else if (formatOut.IsRGB()) {
		pDispatcherEx_.reset(new Dispatcher_T<Color>(*this));
	} else if (formatOut.IsRGBA()) {
		//pDispatcherEx_.reset(new Dispatcher_T<ColorA>(*this));
		return false;
	} else if (formatOut.IsRGB565()) {
		pDispatcherEx_.reset(new Dispatcher_T<ColorRGB565>(*this));
	} else {
		return false;
	}
	SetDispatcher(*pDispatcherEx_);
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
	//pDrawableOut->DrawImage(0, 0, imageOwn, &output.rect, output.attachDir);
	pDrawableOut->DrawImage(output.rect.x, output.rect.y, imageOwn, Rect::Empty, output.attachDir);
	pDrawableOut->Refresh();
}

}
