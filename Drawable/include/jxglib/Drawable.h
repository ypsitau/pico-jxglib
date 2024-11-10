//==============================================================================
// jxglib/Drawable.h
//==============================================================================
#ifndef PICO_JXGLIB_DRAWABLE_H
#define PICO_JXGLIB_DRAWABLE_H
#include "pico/stdlib.h"
#include "stdio.h"
#include "jxglib/Color.h"
#include "jxglib/Rect.h"
#include "jxglib/Font.h"
#include "jxglib/Image.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Drawable
//------------------------------------------------------------------------------
class Drawable {
public:
	class GetterColor_SrcRGB {
	public:
		using Type = Color;
	public:
		Type operator()(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
	};
	class GetterRGB565_SrcRGB {
	public:
		using Type = uint16_t;
	public:
		Type operator()(const uint8_t* p) { return Color::RGB565(p[0], p[1], p[2]); }
	};
	class GetterRGB565_SrcRGB565 {
	public:
		using Type = uint16_t;
	public:
		Type operator()(const uint8_t* p) { return *reinterpret_cast<const Type*>(p); }
	};
	class SetterRGB565_SrcRGB565 {
	public:
		using Type = uint16_t;
	public:
		void operator()(uint8_t* p, uint16_t data) { *reinterpret_cast<uint16_t*>(p) = data; }
	};
	class ScannerBase {
	protected:
		int nCols_, nRows_;
		int bytesPerCol_, bytesPerRow_;
		uint8_t* p_;
		uint8_t* pRow_;
		int iCol_, iRow_;
	public:
		ScannerBase(void* p, int nCols, int nRows, int bytesPerCol, int bytesPerRow) :
			p_{reinterpret_cast<uint8_t*>(p)}, pRow_{p_}, nCols_{nCols}, nRows_{nRows},
			bytesPerCol_{bytesPerCol}, bytesPerRow_{bytesPerRow},
			iCol_{0}, iRow_{0} {}
		void MoveForward() {
			iCol_++;
			p_ += bytesPerCol_;
			if (iCol_ == nCols_) {
				iCol_ = 0;
				iRow_++;
				pRow_ += bytesPerRow_;
				p_ = pRow_;
			}
		}
		bool HasDone() const { return iRow_ >= nRows_; }
	};
	template<typename T_Getter> class Scanner : public ScannerBase {
	public:
		static Scanner HorzNW(const void* p, int width, int height, int bytesPerPixel, int bytesPerLine) {
			return Scanner(p, width, height, bytesPerPixel, bytesPerLine);
		}
	public:
		Scanner(const void* p, int nCols, int nRows, int bytesPerCol, int bytesPerRow) :
				ScannerBase(const_cast<void*>(p), nCols, nRows, bytesPerCol, bytesPerRow) {}
		typename T_Getter::Type ScanForward() {
			auto rtn  = T_Getter()(p_);
			MoveForward();
			return rtn;
		}
	};
	template<typename T_Setter> class Drawer : public ScannerBase {
	public:
		Drawer HorzNW(void* p, int width, int height, int bytesPerPixel, int bytesPerLine) {
			return Drawer(p, width, height, bytesPerPixel, bytesPerLine);
		}
	public:
		Drawer(void* p, int nCols, int nRows, int bytesPerCol, int bytesPerRow) :
				ScannerBase(p, nCols, nRows, bytesPerCol, bytesPerRow) {}
		void DrawForward(const typename T_Setter::Type& data) {
			auto rtn  = T_Setter()(p_, data);
			MoveForward();
		}
	};
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
	virtual void DrawImage(int x, int y, const Image& image) = 0;
};

}

#endif
