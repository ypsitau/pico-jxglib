//==============================================================================
// CanvasLVGL.cpp
//==============================================================================
#include "jxglib/CanvasLVGL.h"

namespace jxglib {

//------------------------------------------------------------------------------
// CanvasLVGL
//------------------------------------------------------------------------------
CanvasLVGL CanvasLVGL::Instance;

bool CanvasLVGL::AttachOutput(Drawable& drawable, const Rect& rect, AttachDir attachDir)
{
	pDrawableOut_ = &drawable;
	::lv_init();
	lv_display_t* disp = ::lv_display_create(240, 320);
	::lv_display_set_flush_cb(disp, Flush_CB);
	lv_draw_buf_t* buf1 = ::lv_draw_buf_create(240, 32, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
	lv_draw_buf_t* buf2 = ::lv_draw_buf_create(240, 32, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
	::lv_display_set_draw_buffers(disp, buf1, buf2);
	//lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
	::lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
	screen_ = ::lv_obj_create(nullptr);
	return true;
}

void CanvasLVGL::DoFlush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Image image(Image::Format::RGB565, ::lv_area_get_width(area), ::lv_area_get_height(area), buf);
	GetDrawableOut().DrawImageFast(area->x1, area->y1, image);
	::lv_disp_flush_ready(disp);
}

void CanvasLVGL::Flush_CB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Instance.DoFlush(disp, area, buf);
}

}
