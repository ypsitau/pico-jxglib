//==============================================================================
// jxglib/Display/WS2812.h
//==============================================================================
#ifndef PICO_JXGLIB_DISPLAY_WS2812_H
#define PICO_JXGLIB_DISPLAY_WS2812_H
#include "pico/stdlib.h"
#include "jxglib/Device/WS2812.h"
#include "jxglib/Canvas.h"
#include "jxglib/Display.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// WS2812
//------------------------------------------------------------------------------
class WS2812 : public Base {
public:
	class DispatcherEx : public Dispatcher {
	private:
		uint32_t timeStamp_;
		WS2812& ws2812_;
	public:
		DispatcherEx(WS2812& ws2812) : ws2812_{ws2812} {}
	public:
		virtual bool Initialize() override;
		virtual void Refresh() override;
		virtual void Fill(const Color& color) override;
		virtual void DrawPixel(int x, int y, const Color& color) override;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
					int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler) override;
		virtual void ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip) override;
		virtual void ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip) override;
	public:
		Canvas& GetCanvas() { return ws2812_.canvas_; }
	};
private:
	Device::WS2812 device_;
	Canvas canvas_;
	DispatcherEx dispatcherEx_;
	uint8_t seqDir_;
	bool zigzagFlag_;
public:
	WS2812(int width, int height, uint8_t seqDir, bool zigzagFlag);
public:
	WS2812& Initialize(const GPIO& gpio);
public:
	Canvas& GetCanvas() { return canvas_; }
	Device::WS2812& GetDevice() { return device_; }
	uint8_t GetSeqDir() const { return seqDir_; }
	bool GetZigzagFlag() const { return zigzagFlag_; }
};

}

#endif
