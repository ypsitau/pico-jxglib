//==============================================================================
// jxglib/AdapterLVGL.h
//==============================================================================
#ifndef PICO_JXGLIB_AdapterLVGL_H
#define PICO_JXGLIB_AdapterLVGL_H
#include <lvgl.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// AdapterLVGL
//------------------------------------------------------------------------------
class AdapterLVGL {
public:
	using AttachDir = Image::WriterDir;
private:
	Drawable* pDrawableOut_;
	lv_obj_t* screen_;
public:
	static AdapterLVGL Instance;
public:
	AdapterLVGL() : pDrawableOut_{nullptr} {}
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
