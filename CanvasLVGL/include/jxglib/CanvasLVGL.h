//==============================================================================
// jxglib/CanvasLVGL.h
//==============================================================================
#ifndef PICO_JXGLIB_CANVASLVGL_H
#define PICO_JXGLIB_CANVASLVGL_H
#include <lvgl.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// CanvasLVGL
//------------------------------------------------------------------------------
class CanvasLVGL {
public:
	using AttachDir = Image::WriterDir;
private:
	Drawable* pDrawableOut_;
	lv_obj_t* screen_;
public:
	static CanvasLVGL Instance;
public:
	CanvasLVGL() : pDrawableOut_{nullptr} {}
public:
	Drawable& GetDrawableOut() { return *pDrawableOut_; }
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, AttachDir attachDir = AttachDir::Normal);
	bool AttachOutput(Drawable& drawable, AttachDir attachDir) {
		return AttachOutput(drawable, Rect::Empty, attachDir);
	}
	static lv_obj_t* GetScreen() { return Instance.screen_; }
	void DoFlush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
private:
	static void Flush_CB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
};

}

#endif
