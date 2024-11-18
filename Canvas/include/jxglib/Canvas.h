//==============================================================================
// jxglib/Canvas.h
//==============================================================================
#ifndef PICO_JXGLIB_CANVAS_H
#define PICO_JXGLIB_CANVAS_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/Drawable.h"
#include "jxglib/Font.h"
#include "jxglib/Image.h"
#include "jxglib/UTF8Decoder.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
class Canvas : public Drawable {
public:
	class Core {
	protected:
		Image& image_;
	public:
		Core(Image& image) : image_(image) {}
	public:
		virtual void Fill_(const Color& color) = 0;
		virtual void DrawPixel_(int x, int y, const Color& color) = 0;
		virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) = 0;
		virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) = 0;
		virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) = 0;
	};
	class CoreNone : public Core {
	public:
		CoreNone(Image& image) : Core(image) {}
	public:
		virtual void Fill_(const Color& color) override {}
		virtual void DrawPixel_(int x, int y, const Color& color) override {}
		virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override {}
		virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override {}
		virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override {}
	};
	class CoreRGB565 : public Core {
	public:
		CoreRGB565(Image& image) : Core(image) {}
	public:
		virtual void Fill_(const Color& color) override;
		virtual void DrawPixel_(int x, int y, const Color& color) override;
		virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override;
		virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override;
		virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override;
	};
private:
	Drawable* pDrawableOut_;
	Image image_;
	Core* pCore_;
private:
	CoreNone coreNone_;
	CoreRGB565 coreRGB565_;
public:
	Canvas() : pDrawableOut_{nullptr}, pCore_{&coreNone_}, coreNone_(image_), coreRGB565_(image_) {}
public:
	bool AttachOutput(Drawable& drawable);
public:
	virtual void Refresh_() override;
	virtual void Fill_(const Color& color) override {
		pCore_->Fill_(color);
	}
	virtual void DrawPixel_(int x, int y, const Color& color) override {
		pCore_->DrawPixel_(x, y, color);
	}
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override {
		pCore_->DrawRectFill_(x, y, width, height, color);
	}
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override {
		pCore_->DrawBitmap_(x, y, data, width, height, color, pColorBg, scaleX, scaleY);
	}
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override {
		pCore_->DrawImage_(x, y, image, pRectClip, imageDir);
	}
};

}

#endif
