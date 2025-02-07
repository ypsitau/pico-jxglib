//==============================================================================
// jxglib/Drawable.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_H
#define PICO_JXGLIB_DRAWABLE_H
#include <stdio.h>
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
	using Dir = Image::WriterDir;
	using DrawDir = Image::WriterDir;
	struct Capability {
		static const uint32_t Device		= (1 << 0);
		static const uint32_t DrawImage		= (1 << 1);
		static const uint32_t DrawImageFast	= (1 << 2);
		static const uint32_t ScrollVert	= (1 << 3);
		static const uint32_t ScrollHorz	= (1 << 4);
		static const uint32_t TransparentBg	= (1 << 5);
	};
	class DrawImageFastHandler {
	public:
		virtual void OnDrawImageFastCompleted() = 0;
	};
	class Dispatcher {
	public:
		Dispatcher() {}
	public:
		virtual bool Initialize() = 0;
		virtual void Refresh() = 0;
		virtual void Fill(const Color& color) = 0;
		virtual void DrawPixel(int x, int y, const Color& color) = 0;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) = 0;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir) = 0;
		virtual void DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir) = 0;
		virtual void DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler) = 0;
		virtual void ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip) = 0;
		virtual void ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip) = 0;
	};
	class DispatcherNone : public Dispatcher {
	public:
		DispatcherNone() {}
	public:
		virtual bool Initialize() override { return true; };
		virtual void Refresh() override {}
		virtual void Fill(const Color& color) override {}
		virtual void DrawPixel(int x, int y, const Color& color) override {}
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override {}
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir) override {}
		virtual void DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir) override {}
		virtual void DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler) override {}
		virtual void ScrollHorz(DirHorz dirHorz, int width, const Rect& rectClip) override {};
		virtual void ScrollVert(DirVert dirVert, int height, const Rect& rectClip) override {};
	};
	struct Context {
		Color colorFg;
		Color colorBg;
		const FontSet* pFontSet;
		int fontScaleWidth, fontScaleHeight;
		float charWidthRatio, lineHeightRatio;
		bool transparentBgFlag;
	public:
		Context() : colorFg{Color::white}, colorBg{Color::black},
			pFontSet{&FontSet::None}, fontScaleWidth{1}, fontScaleHeight{1},
			charWidthRatio(1.0), lineHeightRatio(1.0), transparentBgFlag{false} {}
	public:
		Context& SetColor(const Color& color) { this->colorFg = color; return *this; }
		const Color& GetColor() const { return this->colorFg; }
		Context& SetColorBg(const Color& color) { this->colorBg = color; return *this; }
		const Color& GetColorBg() const { return this->colorBg; }
		Context& SetFont(const FontSet& fontSet, int fontScale = 1) {
			this->pFontSet = &fontSet;
			this->fontScaleWidth = this->fontScaleHeight = fontScale;
			return *this;
		}
		const FontSet& GetFont() const { return *this->pFontSet; }
		Context& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
			this->pFontSet = &fontSet; this->fontScaleWidth = fontScaleWidth, this->fontScaleHeight = fontScaleHeight;
			return *this;
		}
		Context& SetFontScale(int fontScale) {
			this->fontScaleWidth = this->fontScaleHeight = fontScale;
			return *this;
		}
		Context& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
			this->fontScaleWidth = fontScaleWidth, this->fontScaleHeight = fontScaleHeight;
			return *this;
		}
		Context& SetSpacingRatio(float charWidthRatio, float lineHeightRatio) {
			this->charWidthRatio = charWidthRatio, this->lineHeightRatio = lineHeightRatio;
			return *this;
		}
		Context& SetTransparentBg(bool transparentBgFlag) {
			this->transparentBgFlag = transparentBgFlag;
			return *this;
		}
	public:
		int CalcAdvanceX(const FontEntry& fontEntry) const {
			return static_cast<int>(fontEntry.xAdvance * fontScaleWidth * charWidthRatio);
		}
		int CalcAdvanceY() const {
			return static_cast<int>(pFontSet->yAdvance * fontScaleHeight * lineHeightRatio);
		}
	};
protected:
	uint32_t capabilities_;
	const Format* pFormat_;
	int width_, height_;
	Context context_;
private:
	Dispatcher* pDispatcher_;
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
	virtual Dir GetDirection() const { return Dir::Normal; }
	void SetDispatcher(Dispatcher& dispatcher) { pDispatcher_ = &dispatcher; }
	Dispatcher& GetDispatcher() { return *pDispatcher_; }
	bool IsDevice() const { return !!(capabilities_ & Capability::Device); }
	bool CanDrawImage() const { return !!(capabilities_ & Capability::DrawImage); }
	bool CanScrollHorz() const { return !!(capabilities_ & Capability::ScrollHorz); }
	bool CanScrollVert() const { return !!(capabilities_ & Capability::ScrollVert); }
	const Format& GetFormat() const { return *pFormat_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	Size GetSize() const { return Size(width_, height_); }
	Rect GetRect() const { return Rect(0, 0, width_, height_); }
	Context& GetContext() { return context_; }
	const Context& GetContext() const { return context_; }
public:
	bool IsFormatNone() const { return pFormat_->IsIdentical(Format::None); }
	bool IsFormatBitmap() const { return pFormat_->IsIdentical(Format::Bitmap); }
	bool IsFormatGray() const { return pFormat_->IsIdentical(Format::Gray); }
	bool IsFormatRGB() const { return pFormat_->IsIdentical(Format::RGB); }
	bool IsFormatRGBA() const { return pFormat_->IsIdentical(Format::RGBA); }
	bool IsFormatRGB565() const { return pFormat_->IsIdentical(Format::RGB565); }
public:
	Drawable& SetColor(const Color& color) { context_.SetColor(color); return *this; }
	const Color& GetColor() const { return context_.GetColor(); }
	Drawable& SetColorBg(const Color& color) { context_.SetColorBg(color); return *this; }
	const Color& GetColorBg() const { return context_.GetColorBg(); }
	Drawable& SetFont(const FontSet& fontSet, int fontScale = 1) { context_.SetFont(fontSet, fontScale); return *this; }
	const FontSet& GetFont() const { return context_.GetFont(); }
	Drawable& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		context_.SetFont(fontSet, fontScaleWidth, fontScaleHeight); return *this;
	}
	Drawable& SetFontScale(int fontScale) { context_.SetFontScale(fontScale); return *this; }
	Drawable& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		context_.SetFontScale(fontScaleWidth, fontScaleHeight); return *this;
	}
	Drawable& SetSpacingRatio(float charWidthRatio, float lineHeightRatio) {
		context_.SetSpacingRatio(charWidthRatio, lineHeightRatio); return *this;
	}
	int CalcFontHeight() const { return context_.pFontSet->yAdvance * context_.fontScaleHeight; }
	Size CalcStringSize(const char* str) const;
public:
	Drawable& Refresh() { GetDispatcher().Refresh(); return *this; }
	Drawable& Clear() { Fill(context_.colorBg); return *this; }
	Drawable& Fill(const Color& color) { GetDispatcher().Fill(color); return *this; }
public:
	Drawable& DrawPixel(int x, int y, const Color& color) { GetDispatcher().DrawPixel(x, y, color); return *this; }
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
		GetDispatcher().DrawRectFill(x, y, width, height, color);
		return *this;
	}
	Drawable& DrawRectFill(int x, int y, int width, int height) { return DrawRectFill(x, y, width, height, context_.colorFg); }
	Drawable& DrawRectFill(const Point& pt, const Size& size, const Color& color) { return DrawRectFill(pt.x, pt.y, size.width, size.height, color); }
	Drawable& DrawRectFill(const Point& pt, const Size& size) { return DrawRectFill(pt, size, context_.colorFg); }
	Drawable& DrawRectFill(const Rect& rc, const Color& color) { return DrawRectFill(rc.x, rc.y, rc.width, rc.height, color); }
	Drawable& DrawRectFill(const Rect& rc) { return DrawRectFill(rc, context_.colorFg); }
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
					int scaleX = 1, int scaleY = 1, const Rect& rectClip = Rect::Empty, DrawDir drawDir = DrawDir::Normal) {
		GetDispatcher().DrawBitmap(x, y, data, width, height, color, pColorBg, scaleX, scaleY, rectClip, drawDir);
		return *this;
	}
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height, bool transparentBgFlag = false,
			int scaleX = 1, int scaleY = 1, const Rect& rectClip = Rect::Empty, DrawDir drawDir = DrawDir::Normal, const Context* pContext = nullptr) {
		if (!pContext) pContext = &context_;
		if (!(capabilities_ & Capability::TransparentBg)) transparentBgFlag = false;
		return DrawBitmap(x, y, data, width, height, pContext->colorFg,
						transparentBgFlag? nullptr : &pContext->colorBg, scaleX, scaleY, rectClip, drawDir);
	}
	Drawable& DrawBitmap(const Point& pt, const void* data, int width, int height, bool transparentBgFlag = false,
			int scaleX = 1, int scaleY = 1, const Rect& rectClip = Rect::Empty, DrawDir drawDir = DrawDir::Normal, const Context* pContext = nullptr) {
		return DrawBitmap(pt.x, pt.y, data, width, height, transparentBgFlag, scaleX, scaleY, rectClip, drawDir, pContext);
	}
	Drawable& DrawImage(int x, int y, const Image& image, const Rect& rectClip = Rect::Empty, DrawDir drawDir = DrawDir::Normal);
	Drawable& DrawImageFast(int x, int y, const Image& image, bool blockFlag = true, DrawImageFastHandler* pHandler = nullptr);
	Drawable& ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip = Rect::Empty);
	Drawable& ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip = Rect::Empty);
	Drawable& DrawChar(int x, int y, const FontEntry& fontEntry, bool transparentBgFlag = false, const Context* pContext = nullptr);
	Drawable& DrawChar(const Point& pt, const FontEntry& fontEntry, bool transparentBgFlag = false, const Context* pContext = nullptr) {
		return DrawChar(pt.x, pt.y, fontEntry, transparentBgFlag, pContext);
	}
	Drawable& DrawChar(int x, int y, uint32_t code, bool transparentBgFlag = false, const Context* pContext = nullptr);
	Drawable& DrawChar(const Point& pt, uint32_t code, bool transparentBgFlag = false, const Context* pContext = nullptr) {
		return DrawChar(pt.x, pt.y, code, transparentBgFlag, pContext);
	}
	Drawable& DrawString(int x, int y, const char* str, bool transparentBgFlag);
	Drawable& DrawString(const Point& pt, const char* str, bool transparentBgFlag) { return DrawString(pt.x, pt.y, str, transparentBgFlag); }
	Drawable& DrawStringWrap(int x, int y, int width, int height, const char* str, bool transparentBgFlag);
	Drawable& DrawStringWrap(int x, int y, const char* str, bool transparentBgFlag) { return DrawStringWrap(x, y, -1, -1, str, transparentBgFlag); }
	Drawable& DrawStringWrap(const Point& pt, const char* str, bool transparentBgFlag) { return DrawStringWrap(pt.x, pt.y, str, transparentBgFlag); }
	Drawable& DrawStringWrap(const Rect& rcBBox, const char* str, bool transparentBgFlag) {
		return DrawStringWrap(rcBBox.x, rcBBox.y, rcBBox.width, rcBBox.height, str, transparentBgFlag);
	}
	Drawable& DrawString(int x, int y, const char* str) { return DrawString(x, y, str, context_.transparentBgFlag); }
	Drawable& DrawString(const Point& pt, const char* str) { return DrawString(pt, str, context_.transparentBgFlag); }
	Drawable& DrawStringWrap(int x, int y, int width, int height, const char* str) { return DrawStringWrap(x, y, width, height, str, context_.transparentBgFlag); }
	Drawable& DrawStringWrap(int x, int y, const char* str) { return DrawStringWrap(x, y, str, context_.transparentBgFlag); }
	Drawable& DrawStringWrap(const Point& pt, const char* str) { return DrawStringWrap(pt, str, context_.transparentBgFlag); }
	Drawable& DrawStringWrap(const Rect& rcBBox, const char* str) { return DrawStringWrap(rcBBox, str, context_.transparentBgFlag); }
	Drawable& DrawFormatV(int x, int y, const char* format, va_list args);
	Drawable& DrawFormatV(const Point& pt, const char* format, va_list args) { return DrawFormatV(pt.x, pt.y, format, args); }
	Drawable& DrawFormat(int x, int y, const char* format, ...);
	Drawable& DrawFormat(const Point& pt, const char* format, ...);
public:
	Drawable& DrawCross(int x, int y, int width, int height, int wdLine = 1, int htLine = 1);
};

}

#endif
