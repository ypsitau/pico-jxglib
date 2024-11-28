//==============================================================================
// jxglib/Canvas.h
//==============================================================================
#ifndef PICO_JXGLIB_CANVAS_H
#define PICO_JXGLIB_CANVAS_H
#include <memory>
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
	class DispatcherEx : public Dispatcher {
	protected:
		Canvas& canvas_;
	public:
		DispatcherEx(Canvas& canvas) : canvas_{canvas} {}
	public:
		virtual void Initialize() override;
		virtual void Refresh() override;
	};
	template<typename T_Color> class Dispatcher_T : public DispatcherEx {
	public:
		Dispatcher_T(Canvas& canvas) : DispatcherEx(canvas) {}
	public:
		virtual void Fill(const Color& color) override;
		virtual void DrawPixel(int x, int y, const Color& color) override;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override;
		virtual void DrawImage(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override;
		virtual void ScrollHorz(DirHorz dirHorz, int width, const Rect* pRect) override;
		virtual void ScrollVert(DirVert dirVert, int height, const Rect* pRect) override;
	};
	using AttachDir = ImageDir;
private:
	Drawable* pDrawableOut_;
	Image imageOwn_;
	struct Output {
		Rect rect;
		AttachDir attachDir;
	} output_;
	std::unique_ptr<DispatcherEx> pDispatcherEx_;
public:
	Canvas() : Drawable(Capability::DrawImage | Capability::ScrollHorz | Capability::ScrollVert), pDrawableOut_{nullptr} {}
public:
	Drawable* GetDrawableOut() { return pDrawableOut_; }
	Image& GetImageOwn() { return imageOwn_; }
	const Output& GetOutput() const { return output_; }
	bool AttachOutput(Drawable& drawable, const Rect* pRect = nullptr, AttachDir attachDir = AttachDir::Normal);
	bool AttachOutput(Drawable& drawable, const Rect& rect, AttachDir attachDir = AttachDir::Normal) {
		return AttachOutput(drawable, &rect, attachDir);
	}
	bool AttachOutput(Drawable& drawable, AttachDir attachDir) {
		return AttachOutput(drawable, nullptr, attachDir);
	}
};

}

#endif
