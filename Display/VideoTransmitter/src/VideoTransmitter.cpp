//==============================================================================
// VideoTransmitter.cpp
//==============================================================================
#include "jxglib/Display/VideoTransmitter.h"

namespace jxglib::Display {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
VideoTransmitter::VideoTransmitter() :
	Base(Capability::Device | Capability::ScrollHorz | Capability::ScrollVert, Format::RGB, 160, 120),
	dispatcherEx_(*this)
{
	SetDispatcher(dispatcherEx_);
}

const char* VideoTransmitter::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "VideoTransmitter");
	return buff;
}

//------------------------------------------------------------------------------
// VideoTransmitter::DispatcherEx
//------------------------------------------------------------------------------
bool VideoTransmitter::DispatcherEx::Initialize()
{
	return true;
}

void VideoTransmitter::DispatcherEx::Refresh()
{
}

void VideoTransmitter::DispatcherEx::Fill(const Color& color)
{
}

void VideoTransmitter::DispatcherEx::DrawPixel(int x, int y, const Color& color)
{
}

void VideoTransmitter::DispatcherEx::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
}

void VideoTransmitter::DispatcherEx::DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
	int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir)
{
}

void VideoTransmitter::DispatcherEx::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
}

void VideoTransmitter::DispatcherEx::DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler)
{
}

void VideoTransmitter::DispatcherEx::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
}

void VideoTransmitter::DispatcherEx::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
}

}
