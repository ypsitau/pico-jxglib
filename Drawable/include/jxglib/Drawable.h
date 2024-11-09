//==============================================================================
// jxglib/Drawable.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_H
#define PICO_JXGLIB_DRAWABLE_H
#include "pico/stdlib.h"
#include "jxglib/Rect.h"
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
class Drawable {
public:
	struct Context {
		const FontSet* pFontSet;
		int fontScaleX, fontScaleY;
	public:
		Context() : pFontSet{nullptr}, fontScaleX{1}, fontScaleY{1} {}
	};
protected:
	int width_, height_;
	Context context_;
public:
	Drawable(int width, int height) : width_{width}, height_{height} {}
public:
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
public:
	void SetFont(const FontSet& fontSet, int fontScale = 1) {
		context_.pFontSet = &fontSet; context_.fontScaleX = context_.fontScaleY = fontScale;
	}
	void SetFont(const FontSet& fontSet, int fontScaleX, int fontScaleY) {
		context_.pFontSet = &fontSet; context_.fontScaleX = fontScaleX, context_.fontScaleY = fontScaleY;
	}
	void SetFontScale(int fontScale) { context_.fontScaleX = context_.fontScaleY = fontScale; }
	void SetFontScale(int fontScaleX, int fontScaleY) { context_.fontScaleX = fontScaleX, context_.fontScaleY = fontScaleY; }
public:
	void DrawPixel(const Point& pt) { DrawPixel(pt.x, pt.y); }
	void DrawHLine(const Point& pt, int width) { DrawHLine(pt.x, pt.y, width); }
	void DrawVLine(const Point& pt, int height) { DrawVLine(pt.x, pt.y, height); }
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawLine(const Point& pt1, const Point& pt2) { DrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }
	void DrawRect(int x, int y, int width, int height);
	void DrawRect(const Point& pt, const Size& size) { DrawRect(pt.x, pt.y, size.width, size.height); }
	void DrawRect(const Rect& rc) { DrawRect(rc.x, rc.y, rc.width, rc.height); }
	void DrawRectFill(const Point& pt, const Size& size) { DrawRectFill(pt.x, pt.y, size.width, size.height); }
	void DrawRectFill(const Rect& rc) { DrawRect(rc.x, rc.y, rc.width, rc.height); }
	void DrawBitmap(const Point& pt, const void* data, int width, int height, bool transparentBgFlag = false, int scaleX = 1, int scaleY = 1) {
		DrawBitmap(pt.x, pt.y, data, width, height, transparentBgFlag, scaleX, scaleY);
	}
	void DrawChar(int x, int y, const FontEntry& fontEntry);
	void DrawChar(const Point& pt, const FontEntry& fontEntry) { DrawChar(pt.x, pt.y, fontEntry); }
	void DrawChar(int x, int y, uint32_t code);
	void DrawChar(const Point& pt, uint32_t code) { DrawChar(pt.x, pt.y, code); }
	void DrawString(int x, int y, const char* str, const char* strEnd = nullptr);
	void DrawString(const Point& pt, const char* str, const char* strEnd = nullptr) {
		DrawString(pt.x, pt.y, str, strEnd);
	}
	const char* DrawStringWrap(int x, int y, int width, int height, const char* str, int htLine = -1);
	const char* DrawStringWrap(int x, int y, const char* str, int htLine = -1) {
		return DrawStringWrap(x, y, -1, -1, str, htLine);
	}
	const char* DrawStringWrap(const Point& pt, const char* str, int htLine = -1) {
		return DrawStringWrap(pt.x, pt.y, str, htLine);
	}
	const char* DrawStringWrap(const Rect& rcBBox, const char* str, int htLine = -1) {
		return DrawStringWrap(rcBBox.x, rcBBox.y, rcBBox.width, rcBBox.height, str, htLine);
	}
public:
	virtual void DrawPixel(int x, int y) = 0;
	virtual void DrawHLine(int x, int y, int width) = 0;
	virtual void DrawVLine(int x, int y, int height) = 0;
	virtual void DrawRectFill(int x, int y, int width, int height) = 0;
	virtual void DrawBitmap(int x, int y, const void* data, int width, int height, bool transparentBgFlag, int scaleX = 1, int scaleY = 1) = 0;
};

}

#endif
