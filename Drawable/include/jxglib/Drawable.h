//==============================================================================
// jxglib/Drawable.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_H
#define PICO_JXGLIB_DRAWABLE_H
#include <stdio.h>
#include <memory>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/Font.h"
#include "jxglib/Image.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
class Drawable {
public:
	using Format = Image::Format;
	using ImageDir = Image::SequencerDir;
	struct Capability {
		static const uint32_t Device		= (1 << 0);
		static const uint32_t DrawImage		= (1 << 1);
		static const uint32_t ScrollVert	= (1 << 2);
		static const uint32_t ScrollHorz	= (1 << 3);
	};
	class Dispatcher {
	public:
		Dispatcher() {}
	public:
		virtual void Initialize() = 0;
		virtual void Refresh() = 0;
		virtual void Fill(const Color& color) = 0;
		virtual void DrawPixel(int x, int y, const Color& color) = 0;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) = 0;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) = 0;
		virtual void DrawImage(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) = 0;
		virtual void ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect* pRect) = 0;
		virtual void ScrollVert(DirVert dirVert, int htScroll, const Rect* pRect) = 0;
	};
	class DispatcherNone : public Dispatcher {
	public:
		DispatcherNone() {}
	public:
		virtual void Initialize() override {};
		virtual void Refresh() override {}
		virtual void Fill(const Color& color) override {}
		virtual void DrawPixel(int x, int y, const Color& color) override {}
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override {}
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override {}
		virtual void DrawImage(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override {}
		virtual void ScrollHorz(DirHorz dirHorz, int width, const Rect* pRect) override {};
		virtual void ScrollVert(DirVert dirVert, int height, const Rect* pRect) override {};
	};
	struct Context {
		Color colorFg;
		Color colorBg;
		const FontSet* pFontSet;
		int fontScaleWidth, fontScaleHeight;
		float charWidthRatio, lineHeightRatio;
	public:
		Context() : colorFg{Color::white}, colorBg{Color::black},
			pFontSet{&FontSet::None}, fontScaleWidth{1}, fontScaleHeight{1},
			charWidthRatio(1.0), lineHeightRatio(1.0) {}
	};
	class StringCont {
	private:
		Point pos_;
		const char* str_;
	public:
		StringCont() : str_{nullptr} {}
		StringCont(const Point& pos, const char* str) : pos_{pos}, str_{str} {}
		const Point& GetPosition() const { return pos_; }
		const char* GetString() const { return str_; }
		void Update(const Point& pos, const char* str) { pos_ = pos; str_ = str; }
		bool IsDone() const { return !*str_; }
	};
protected:
	uint32_t capabilities_;
	const Format* pFormat_;
	int width_, height_;
	Context context_;
	std::unique_ptr<Dispatcher> pDispatcher_;
public:
	Drawable(uint32_t capabilities) :
			capabilities_{capabilities}, pFormat_{&Format::None},
			width_{0}, height_{0}, pDispatcher_{new DispatcherNone()} {}
	Drawable(uint32_t capabilities, const Format& format, int width, int height) :
			capabilities_{capabilities}, pFormat_{&format},
			width_{width}, height_{height}, pDispatcher_{new DispatcherNone()} {}
public:
	void SetCapacity(const Format format, int width, int height) {
		pFormat_ = &format, width_ = width, height_ = height;
	}
	bool IsDevice() const { return !!(capabilities_ & Capability::Device); }
	bool CanDrawImage() const { return !!(capabilities_ & Capability::DrawImage); }
	bool CanScrollHorz() const { return !!(capabilities_ & Capability::ScrollHorz); }
	bool CanScrollVert() const { return !!(capabilities_ & Capability::ScrollVert); }
	const Format& GetFormat() const { return *pFormat_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
public:
	bool IsFormatNone() const { return pFormat_->IsIdentical(Format::None); }
	bool IsFormatBitmap() const { return pFormat_->IsIdentical(Format::Bitmap); }
	bool IsFormatGray() const { return pFormat_->IsIdentical(Format::Gray); }
	bool IsFormatRGB() const { return pFormat_->IsIdentical(Format::RGB); }
	bool IsFormatRGBA() const { return pFormat_->IsIdentical(Format::RGBA); }
	bool IsFormatRGB565() const { return pFormat_->IsIdentical(Format::RGB565); }
public:
	Drawable& SetColor(const Color& color) { context_.colorFg = color; return *this; }
	const Color& GetColor() const { return context_.colorFg; }
	Drawable& SetColorBg(const Color& color) { context_.colorBg = color; return *this; }
	const Color& GetColorBg() const { return context_.colorBg; }
	Drawable& SetFont(const FontSet& fontSet, int fontScale = 1) {
		context_.pFontSet = &fontSet;
		context_.fontScaleWidth = context_.fontScaleHeight = fontScale;
		return *this;
	}
	const FontSet& GetFont() const { return *context_.pFontSet; }
	Drawable& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		context_.pFontSet = &fontSet; context_.fontScaleWidth = fontScaleWidth, context_.fontScaleHeight = fontScaleHeight;
		return *this;
	}
	Drawable& SetFontScale(int fontScale) {
		context_.fontScaleWidth = context_.fontScaleHeight = fontScale;
		return *this;
	}
	Drawable& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		context_.fontScaleWidth = fontScaleWidth, context_.fontScaleHeight = fontScaleHeight;
		return *this;
	}
	Drawable& SetSpacingRatio(float charWidthRatio, float lineHeightRatio) {
		context_.charWidthRatio = charWidthRatio, context_.lineHeightRatio = lineHeightRatio;
		return *this;
	}
	int CalcAdvanceX(const FontEntry& fontEntry) const {
		return static_cast<int>(fontEntry.xAdvance * context_.fontScaleWidth * context_.charWidthRatio);
	}
	int CalcAdvanceY() const {
		return static_cast<int>(context_.pFontSet->yAdvance * context_.fontScaleHeight * context_.lineHeightRatio);
	}
public:
	Drawable& Refresh() { pDispatcher_->Refresh(); return *this; }
	Drawable& Clear() { Fill(context_.colorBg); return *this; }
	Drawable& Fill(const Color& color) { pDispatcher_->Fill(color); return *this; }
public:
	Drawable& DrawPixel(int x, int y, const Color& color) { pDispatcher_->DrawPixel(x, y, color); return *this; }
	Drawable& DrawPixel(int x, int y) { return DrawPixel(x, y, context_.colorFg); }
	Drawable& DrawPixel(const Point& pt, const Color& color) { return DrawPixel(pt.x, pt.y, color); }
	Drawable& DrawPixel(const Point& pt) { return DrawPixel(pt, context_.colorFg); }
	Drawable& DrawHLine(int x, int y, int width, const Color& color) { return DrawRectFill(x, y, width, 1, color); }
	Drawable& DrawHLine(int x, int y, int width) { return DrawHLine(x, y, width, context_.colorFg); }
	Drawable& DrawHLine(const Point& pt, int width, const Color& color) { return DrawHLine(pt.x, pt.y, width, color); }
	Drawable& DrawHLine(const Point& pt, int width) { return DrawHLine(pt, width, context_.colorFg); }
	Drawable& DrawVLine(int x, int y, int height, const Color& color) { return DrawRectFill(x, y, 1, height, color); }
	Drawable& DrawVLine(int x, int y, int height) { return DrawVLine(x, y, height, context_.colorFg); }
	Drawable& DrawVLine(const Point& pt, int height, const Color& color) { return DrawVLine(pt.x, pt.y, height, color); }
	Drawable& DrawVLine(const Point& pt, int height) { return DrawVLine(pt, height, context_.colorFg); }
	Drawable& DrawLine(int x0, int y0, int x1, int y1, const Color& color);
	Drawable& DrawLine(int x0, int y0, int x1, int y1) { return DrawLine(x0, y0, x1, y1, context_.colorFg); }
	Drawable& DrawLine(const Point& pt1, const Point& pt2, const Color& color) { return DrawLine(pt1.x, pt1.y, pt2.x, pt2.y, color); }
	Drawable& DrawLine(const Point& pt1, const Point& pt2) { return DrawLine(pt1, pt2, context_.colorFg); }
	Drawable& DrawRect(int x, int y, int width, int height, const Color& color);
	Drawable& DrawRect(int x, int y, int width, int height) { return DrawRect(x, y, width, height, context_.colorFg); }
	Drawable& DrawRect(const Point& pt, const Size& size, const Color& color) { return DrawRect(pt.x, pt.y, size.width, size.height, color); }
	Drawable& DrawRect(const Point& pt, const Size& size) { return DrawRect(pt, size, context_.colorFg); }
	Drawable& DrawRect(const Rect& rc, const Color& color) { return DrawRect(rc.x, rc.y, rc.width, rc.height, color); }
	Drawable& DrawRect(const Rect& rc) { return DrawRect(rc, context_.colorFg); }
	Drawable& DrawRectFill(int x, int y, int width, int height, const Color& color) {
		pDispatcher_->DrawRectFill(x, y, width, height, color);
		return *this;
	}
	Drawable& DrawRectFill(int x, int y, int width, int height) { return DrawRectFill(x, y, width, height, context_.colorFg); }
	Drawable& DrawRectFill(const Point& pt, const Size& size, const Color& color) { return DrawRectFill(pt.x, pt.y, size.width, size.height, color); }
	Drawable& DrawRectFill(const Point& pt, const Size& size) { return DrawRectFill(pt, size, context_.colorFg); }
	Drawable& DrawRectFill(const Rect& rc, const Color& color) { return DrawRectFill(rc.x, rc.y, rc.width, rc.height, color); }
	Drawable& DrawRectFill(const Rect& rc) { return DrawRectFill(rc, context_.colorFg); }
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height,
				const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) {
		pDispatcher_->DrawBitmap(x, y, data, width, height, color, pColorBg, scaleX, scaleY);
		return *this;
	}
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height, bool transparentBgFlag = false, int scaleX = 1, int scaleY = 1) {
		DrawBitmap(x, y, data, width, height, context_.colorFg, transparentBgFlag? nullptr : &context_.colorBg, scaleX, scaleY);
		return *this;
	}
	Drawable& DrawBitmap(const Point& pt, const void* data, int width, int height, bool transparentBgFlag = false, int scaleX = 1, int scaleY = 1) {
		DrawBitmap(pt.x, pt.y, data, width, height, transparentBgFlag, scaleX, scaleY);
		return *this;
	}
	Drawable& DrawImage(int x, int y, const Image& image, const Rect* pRectClip = nullptr, ImageDir imageDir = ImageDir::Normal) {
		pDispatcher_->DrawImage(x, y, image, pRectClip, imageDir);
		return *this;
	}
	Drawable& ScrollHorz(DirHorz dirHorz, int wdScroll) {
		pDispatcher_->ScrollHorz(dirHorz, wdScroll, nullptr);
		return *this;
	}
	Drawable& ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rect) {
		pDispatcher_->ScrollHorz(dirHorz, wdScroll, &rect);
		return *this;
	}
	Drawable& ScrollVert(DirVert dirVert, int htScroll) {
		pDispatcher_->ScrollVert(dirVert, htScroll, nullptr);
		return *this;
	}
	Drawable& ScrollVert(DirVert dirVert, int htScroll, const Rect& rect) {
		pDispatcher_->ScrollVert(dirVert, htScroll, &rect);
		return *this;
	}
	Drawable& DrawChar(int x, int y, const FontEntry& fontEntry);
	Drawable& DrawChar(const Point& pt, const FontEntry& fontEntry) { DrawChar(pt.x, pt.y, fontEntry); return *this; }
	Drawable& DrawChar(int x, int y, uint32_t code);
	Drawable& DrawChar(const Point& pt, uint32_t code) { DrawChar(pt.x, pt.y, code); return *this; }
	Drawable& DrawString(int x, int y, const char* str, const char* strEnd = nullptr, StringCont* pStringCont = nullptr);
	Drawable& DrawString(const Point& pt, const char* str, const char* strEnd = nullptr, StringCont* pStringCont = nullptr) {
		DrawString(pt.x, pt.y, str, strEnd, pStringCont);
		return *this;
	}
	Drawable& DrawString(StringCont& stringCont, const char* strEnd = nullptr) {
		DrawString(stringCont.GetPosition(), stringCont.GetString(), strEnd);
		return *this;
	}
	Drawable& DrawStringWrap(int x, int y, int width, int height, const char* str, StringCont* pStringCont = nullptr);
	Drawable& DrawStringWrap(int x, int y, const char* str, StringCont* pStringCont = nullptr) {
		return DrawStringWrap(x, y, -1, -1, str, pStringCont);
	}
	Drawable& DrawStringWrap(const Point& pt, const char* str, StringCont* pStringCont = nullptr) {
		return DrawStringWrap(pt.x, pt.y, str, pStringCont);
	}
	Drawable& DrawStringWrap(const Rect& rcBBox, const char* str, StringCont* pStringCont = nullptr) {
		return DrawStringWrap(rcBBox.x, rcBBox.y, rcBBox.width, rcBBox.height, str, pStringCont);
	}
};

}

#endif
