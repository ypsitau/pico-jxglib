//==============================================================================
// jxglib/Drawable.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_H
#define PICO_JXGLIB_DRAWABLE_H
#include "pico/stdlib.h"
#include "stdio.h"
#include "jxglib/Common.h"
#include "jxglib/Font.h"
#include "jxglib/Image.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
class Drawable {
public:
	struct Context {
		Color colorFg;
		Color colorBg;
		const FontSet* pFontSet;
		int fontScaleWidth, fontScaleHeight;
		float charWidthRatio, lineHeightRatio;
	public:
		Context() : colorFg{Color::white}, colorBg{Color::black},
			pFontSet{nullptr}, fontScaleWidth{1}, fontScaleHeight{1},
			charWidthRatio(1.0), lineHeightRatio(1.0) {}
	};
	class StringCont {
	private:
		Point pos_;
		const char* str_;
	public:
		StringCont(const Point& pos, const char* str) : pos_{pos}, str_{str} {}
		const Point& GetPosition() const { return pos_; }
		const char* GetString() const { return str_; }
		void Update(const Point& pos, const char* str) { pos_ = pos; str_ = str; }
	};
	using ImageDir = Image::SequencerDir;
protected:
	int width_, height_;
	Context context_;
public:
	Drawable(int width, int height) : width_{width}, height_{height} {}
public:
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
public:
	Drawable& SetColor(const Color& color) { context_.colorFg = color; return *this; }
	Drawable& SetColorBg(const Color& color) { context_.colorBg = color; return *this; }
	Drawable& SetFont(const FontSet& fontSet, int fontScale = 1) {
		context_.pFontSet = &fontSet; context_.fontScaleWidth = context_.fontScaleHeight = fontScale;
		return *this;
	}
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
public:
	Drawable& Refresh() { Refresh_(); return *this; }
	Drawable& Clear() { Fill(context_.colorBg); return *this; }
	Drawable& Fill(const Color& color) { Fill_(color); return *this; }
public:
	Drawable& DrawPixel(int x, int y, const Color& color) { DrawPixel_(x, y, color); return *this; }
	Drawable& DrawRectFill(int x, int y, int width, int height, const Color& color) {
		DrawRectFill_(x, y, width, height, color);
		return *this;
	}
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height,
				const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) {
		DrawBitmap_(x, y, data, width, height, color, pColorBg, scaleX, scaleY);
		return *this;
	}
	Drawable& DrawImage(int x, int y, const Image& image, const Rect* pRectClip = nullptr, ImageDir imageDir = ImageDir::Normal) {
		DrawImage_(x, y, image, pRectClip, imageDir);
		return *this;
	}
public:
	Drawable& DrawPixel(int x, int y) { DrawPixel(x, y, context_.colorFg); return *this; }
	Drawable& DrawPixel(const Point& pt) { DrawPixel(pt.x, pt.y); return *this; }
	Drawable& DrawHLine(int x, int y, int width) { DrawRectFill(x, y, width, 1); return *this; }
	Drawable& DrawHLine(const Point& pt, int width) { DrawHLine(pt.x, pt.y, width); return *this; }
	Drawable& DrawVLine(int x, int y, int height) { DrawRectFill(x, y, 1, height); return *this; }
	Drawable& DrawVLine(const Point& pt, int height) { DrawVLine(pt.x, pt.y, height); return *this; }
	Drawable& DrawLine(int x0, int y0, int x1, int y1);
	Drawable& DrawLine(const Point& pt1, const Point& pt2) { DrawLine(pt1.x, pt1.y, pt2.x, pt2.y); return *this; }
	Drawable& DrawRect(int x, int y, int width, int height);
	Drawable& DrawRect(const Point& pt, const Size& size) { DrawRect(pt.x, pt.y, size.width, size.height); return *this; }
	Drawable& DrawRect(const Rect& rc) { DrawRect(rc.x, rc.y, rc.width, rc.height); return *this; }
	Drawable& DrawRectFill(int x, int y, int width, int height) { DrawRectFill(x, y, width, height, context_.colorFg); return *this; }
	Drawable& DrawRectFill(const Point& pt, const Size& size) { DrawRectFill(pt.x, pt.y, size.width, size.height); return *this; }
	Drawable& DrawRectFill(const Rect& rc) { DrawRect(rc.x, rc.y, rc.width, rc.height); return *this; }
	Drawable& DrawBitmap(int x, int y, const void* data, int width, int height, bool transparentBgFlag = false, int scaleX = 1, int scaleY = 1) {
		DrawBitmap(x, y, data, width, height, context_.colorFg, transparentBgFlag? nullptr : &context_.colorBg, scaleX, scaleY);
		return *this;
	}
	Drawable& DrawBitmap(const Point& pt, const void* data, int width, int height, bool transparentBgFlag = false, int scaleX = 1, int scaleY = 1) {
		DrawBitmap(pt.x, pt.y, data, width, height, transparentBgFlag, scaleX, scaleY);
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
public:
	virtual void Refresh_() = 0;
	virtual void Fill_(const Color& color) = 0;
	virtual void DrawPixel_(int x, int y, const Color& color) = 0;
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) = 0;
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
		const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) = 0;
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) = 0;
};

}

#endif
