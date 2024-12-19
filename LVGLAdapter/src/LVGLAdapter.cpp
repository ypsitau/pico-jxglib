//==============================================================================
// LVGLAdapter.cpp
//==============================================================================
#include "jxglib/LVGLAdapter.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LVGLAdapter
//------------------------------------------------------------------------------
LVGLAdapter LVGLAdapter::Instance;

bool LVGLAdapter::AttachOutput(Drawable& drawable, const Rect& rect)
{
	const int nPartial = 10;
	pDrawableOut_ = &drawable;
	::lv_init();
	Rect rectAdj(rect);
	Rect rectBound(0, 0, drawable.GetWidth(), drawable.GetHeight());
	if (rectAdj.IsEmpty()) {
		rectAdj = rectBound;
	} else {
		rectAdj.Adjust(rectBound);
	}
	rectOut_ = rectAdj;
	lv_display_t* disp = ::lv_display_create(rectAdj.width, rectAdj.height);
	::lv_display_set_flush_cb(disp, FlushCB);
	uint32_t bufSize = rectAdj.width * rectAdj.height / nPartial * ::lv_color_format_get_size(::lv_display_get_color_format(disp));
	void* buf1 = ::lv_malloc(bufSize);
	void* buf2 = ::lv_malloc(bufSize);
	::lv_display_set_buffers(disp, buf1, buf2, bufSize, LV_DISPLAY_RENDER_MODE_PARTIAL);
	return true;
}

void LVGLAdapter::Flush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Image image(Image::Format::RGB565, ::lv_area_get_width(area), ::lv_area_get_height(area), buf);
	GetDrawableOut().DrawImageFast(rectOut_.x + area->x1, rectOut_.y + area->y1, image);
	::lv_disp_flush_ready(disp);
}

void LVGLAdapter::FlushCB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Instance.Flush(disp, area, buf);
}

}
