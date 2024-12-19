//==============================================================================
// jxglib/LVGLAdapter.h
//==============================================================================
#ifndef PICO_JXGLIB_LVGLAdapter_H
#define PICO_JXGLIB_LVGLAdapter_H
#include <lvgl.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LVGLAdapter
//------------------------------------------------------------------------------
class LVGLAdapter {
public:
	using AttachDir = Image::WriterDir;
private:
	Drawable* pDrawableOut_;
	Rect rectOut_;
public:
	static LVGLAdapter Instance;
public:
	LVGLAdapter() : pDrawableOut_{nullptr} {}
public:
	Drawable& GetDrawableOut() { return *pDrawableOut_; }
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty);
	void Flush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
private:
	static void FlushCB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
};

}

#endif
