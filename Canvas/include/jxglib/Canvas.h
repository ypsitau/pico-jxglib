//==============================================================================
// jxglib/Canvas.h
//==============================================================================
#ifndef PICO_JXGLIB_CANVAS_H
#define PICO_JXGLIB_CANVAS_H
#include "pico/stdlib.h"
#include "jxglib/Color.h"
#include "jxglib/Drawable.h"
#include "jxglib/Font.h"
#include "jxglib/Image.h"
#include "jxglib/UTF8Decoder.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
class Canvas : public Drawable {
private:
	Image& image_;
public:
	Canvas(Image& image) : Drawable(image.GetWidth(), image.GetHeight()), image_{image} {}
public:
	void Clear();
	void Fill();
	virtual void DrawPixel_(int x, int y, const Color& color) override;
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override;
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
		const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override;
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override;
};

}

#endif
