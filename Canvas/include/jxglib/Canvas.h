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
		virtual void Fill_(Canvas& canvas, const Color& color) const = 0;
		virtual void DrawPixel_(Canvas& canvas, int x, int y, const Color& color) const = 0;
		virtual void DrawRectFill_(Canvas& canvas, int x, int y, int width, int height, const Color& color) const = 0;
		virtual void DrawBitmap_(Canvas& canvas, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const = 0;
		virtual void DrawImage_(Canvas& canvas, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const = 0;
		virtual void ScrollHorz_(Canvas& canvas, DirHorz dirHorz, int wdScroll, const Rect* pRect) const = 0;
		virtual void ScrollVert_(Canvas& canvas, DirVert dirVert, int htScroll, const Rect* pRect) const = 0;
	};
	class DispatcherNone : public Dispatcher {
	public:
		DispatcherNone() {}
	public:
		virtual void Fill_(Canvas& canvas, const Color& color) const override {}
		virtual void DrawPixel_(Canvas& canvas, int x, int y, const Color& color) const override {}
		virtual void DrawRectFill_(Canvas& canvas, int x, int y, int width, int height, const Color& color) const override {}
		virtual void DrawBitmap_(Canvas& canvas, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const override {}
		virtual void DrawImage_(Canvas& canvas, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const override {}
		virtual void ScrollHorz_(Canvas& canvas, DirHorz dirHorz, int width, const Rect* pRect) const override {};
		virtual void ScrollVert_(Canvas& canvas, DirVert dirVert, int height, const Rect* pRect) const override {};
	};
	class DispatcherRGB565 : public Dispatcher {
	public:
		DispatcherRGB565() {}
	public:
		virtual void Fill_(Canvas& canvas, const Color& color) const override;
		virtual void DrawPixel_(Canvas& canvas, int x, int y, const Color& color) const override;
		virtual void DrawRectFill_(Canvas& canvas, int x, int y, int width, int height, const Color& color) const override;
		virtual void DrawBitmap_(Canvas& canvas, int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) const override;
		virtual void DrawImage_(Canvas& canvas, int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) const override;
		virtual void ScrollHorz_(Canvas& canvas, DirHorz dirHorz, int width, const Rect* pRect) const override;
		virtual void ScrollVert_(Canvas& canvas, DirVert dirVert, int height, const Rect* pRect) const override;
	};
	using AttachDir = ImageDir;
private:
	Drawable* pDrawableOut_;
	Image imageOwn_;
	const Dispatcher* pDispatcher_;
	struct Output {
		Rect rect;
		AttachDir attachDir;
	} output_;
private:
	static const DispatcherNone dispatcherNone;
	static const DispatcherRGB565 dispatcherRGB565;
public:
	Canvas() : Drawable(Capability::DrawImage | Capability::ScrollHorz | Capability::ScrollVert),
			pDrawableOut_{nullptr}, pDispatcher_{&dispatcherNone} {}
public:
	Image& GetImageOwn() { return imageOwn_; }
	bool AttachOutput(Drawable& drawable, const Rect* pRect = nullptr, AttachDir attachDir = AttachDir::Normal);
	bool AttachOutput(Drawable& drawable, AttachDir attachDir) { return AttachOutput(drawable, nullptr, attachDir); }
protected:
	virtual void Refresh_() override;
	virtual void Fill_(const Color& color) override {
		pDispatcher_->Fill_(*this, color);
	}
	virtual void DrawPixel_(int x, int y, const Color& color) override {
		pDispatcher_->DrawPixel_(*this, x, y, color);
	}
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override {
		pDispatcher_->DrawRectFill_(*this, x, y, width, height, color);
	}
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override {
		pDispatcher_->DrawBitmap_(*this, x, y, data, width, height, color, pColorBg, scaleX, scaleY);
	}
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override {
		pDispatcher_->DrawImage_(*this, x, y, image, pRectClip, imageDir);
	}
	virtual void ScrollHorz_(DirHorz dirHorz, int wdScroll, const Rect* pRect) override {
		pDispatcher_->ScrollHorz_(*this, dirHorz, wdScroll, pRect);
	}
	virtual void ScrollVert_(DirVert dirVert, int htScroll, const Rect* pRect) override {
		pDispatcher_->ScrollVert_(*this, dirVert, htScroll, pRect);
	}
};

}

#endif
