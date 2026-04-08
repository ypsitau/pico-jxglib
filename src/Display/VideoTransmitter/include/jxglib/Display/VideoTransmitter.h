//==============================================================================
// jxglib/Display/VideoTransmitter.h
//==============================================================================
#ifndef PICO_JXGLIB_DISPLAY_VIDEOTRANSMITTER_H
#define PICO_JXGLIB_DISPLAY_VIDEOTRANSMITTER_H
#include "pico/stdlib.h"
#include "jxglib/USBDevice/VideoTransmitter.h"
#include "jxglib/Canvas.h"
#include "jxglib/Display.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
class VideoTransmitter : public Base {
public:
	class DispatcherEx : public Dispatcher {
	private:
		VideoTransmitter& videoTransmitter_;
	public:
		DispatcherEx(VideoTransmitter& videoTransmitter) : videoTransmitter_{videoTransmitter} {}
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
	};
private:
	DispatcherEx dispatcherEx_;
public:
	VideoTransmitter();
public:
	// virtual functions of Display::Base
	virtual const char* GetName() const override { return "VideoTransmitter"; }
	virtual const char* GetVariantName() const override { return ""; }
	virtual const char* GetRemarks(char* buff, int lenMax) const override;
};

}

#endif
