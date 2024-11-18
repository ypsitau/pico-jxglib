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
#include "jxglib/Util.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Canvas
//------------------------------------------------------------------------------
class Canvas : public Drawable {
public:
	class Dispatcher {
	public:
		Dispatcher() {}
	public:
		virtual void Fill_(Image& imageOwn, const Color& color) const = 0;
		virtual void DrawPixel_(Image& imageOwn, int x, int y, const Color& color) const = 0;
		virtual void DrawRectFill_(Image& imageOwn, int x, int y, int width, int height, const Color& color) const = 0;
		virtual void DrawBitmap_(Image& imageOwn, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const = 0;
		virtual void DrawImage_(Image& imageOwn, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const = 0;
	};
	class DispatcherNone : public Dispatcher {
	public:
		DispatcherNone() {}
	public:
		virtual void Fill_(Image& imageOwn, const Color& color) const override {}
		virtual void DrawPixel_(Image& imageOwn, int x, int y, const Color& color) const override {}
		virtual void DrawRectFill_(Image& imageOwn, int x, int y, int width, int height, const Color& color) const override {}
		virtual void DrawBitmap_(Image& imageOwn, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const override {}
		virtual void DrawImage_(Image& imageOwn, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const override {}
	};
	class DispatcherRGB565 : public Dispatcher {
	public:
		DispatcherRGB565() {}
	public:
		virtual void Fill_(Image& imageOwn, const Color& color) const override;
		virtual void DrawPixel_(Image& imageOwn, int x, int y, const Color& color) const override;
		virtual void DrawRectFill_(Image& imageOwn, int x, int y, int width, int height, const Color& color) const override;
		virtual void DrawBitmap_(Image& imageOwn, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const override;
		virtual void DrawImage_(Image& imageOwn, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const override;
	};
private:
	Drawable* pDrawableOut_;
	Image imageOwn_;
	const Dispatcher* pDispatcher_;
	struct Output {
		Rect rect;
		ImageDir imageDir;
	} output_;
private:
	static const DispatcherNone dispatcherNone;
	static const DispatcherRGB565 dispatcherRGB565;
public:
	Canvas() : pDrawableOut_{nullptr}, pDispatcher_{&dispatcherNone} {}
public:
	bool AttachOutput(Drawable& drawable, const Rect* pRect = nullptr, ImageDir imageDir = ImageDir::Normal);
	bool AttachOutput(Drawable& drawable, ImageDir imageDir) { return AttachOutput(drawable, nullptr, imageDir); }
public:
	virtual void Refresh_() override;
	virtual void Fill_(const Color& color) override {
		pDispatcher_->Fill_(imageOwn_, color);
	}
	virtual void DrawPixel_(int x, int y, const Color& color) override {
		pDispatcher_->DrawPixel_(imageOwn_, x, y, color);
	}
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override {
		pDispatcher_->DrawRectFill_(imageOwn_, x, y, width, height, color);
	}
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override {
		pDispatcher_->DrawBitmap_(imageOwn_, x, y, data, width, height, color, pColorBg, scaleX, scaleY);
	}
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override {
		pDispatcher_->DrawImage_(imageOwn_, x, y, image, pRectClip, imageDir);
	}
};

}

#endif
