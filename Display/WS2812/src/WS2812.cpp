//==============================================================================
// WS2812.cpp
//==============================================================================
#include "jxglib/Display/WS2812.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// WS2812
//------------------------------------------------------------------------------
WS2812::WS2812(int width, int height) :
	Base(Capability::Device | Capability::ScrollHorz | Capability::ScrollVert, Format::RGB, 16, 16), dispatcherEx_(*this)
{
	SetDispatcher(dispatcherEx_);
}

WS2812& WS2812::Initialize()
{
	canvas_.Allocate(GetFormat(), GetWidth(), GetHeight());
	return *this;
}

//------------------------------------------------------------------------------
// WS2812::DispatcherEx
//------------------------------------------------------------------------------
bool WS2812::DispatcherEx::Initialize()
{
	return true;
}

void WS2812::DispatcherEx::Refresh()
{
}

void WS2812::DispatcherEx::Fill(const Color& color)
{
	GetCanvas().Fill(color);
}

void WS2812::DispatcherEx::DrawPixel(int x, int y, const Color& color)
{
	GetCanvas().DrawPixel(x, y, color);
}

void WS2812::DispatcherEx::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	GetCanvas().DrawRectFill(x, y, width, height, color);
}

void WS2812::DispatcherEx::DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
	int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir)
{
	GetCanvas().DrawBitmap(x, y, data, width, height, color, pColorBg, scaleX, scaleY, rectClip, drawDir);
}

void WS2812::DispatcherEx::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
	GetCanvas().DrawImage(x, y, image, rectClip, drawDir);
}

void WS2812::DispatcherEx::DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler)
{
	GetCanvas().DrawImageFast(x, y, image, blockFlag, pHandler);
}

void WS2812::DispatcherEx::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	GetCanvas().ScrollHorz(dirHorz, wdScroll, rectClip);
}

void WS2812::DispatcherEx::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	GetCanvas().ScrollVert(dirVert, htScroll, rectClip);
}

}
