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
	class Input {
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) = 0;
	};
	class InputDumb : public Input {
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override {};
	};
private:
	bool doubleBuffFlag_;
	int nPartial_;
	Drawable* pDrawableOut_;
	Rect rectOut_;
	lv_display_t* disp_;
	Input* pInput_Pointer_;
	Input* pInput_Keypad_;
	Input* pInput_Button_;
	Input* pInput_Encoder_;
private:
	static InputDumb inputDumb_;
public:
	LVGLAdapter(bool doubleBuffFlag = true, int nPartial = 10);
public:
	Drawable& GetDrawableOut() { return *pDrawableOut_; }
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty);
	void Flush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
	void SetDefault() { ::lv_disp_set_default(disp_); }
	void SetInput_Pointer(Input& input);
	void SetInput_Keypad(Input& input);
	void SetInput_Button(Input& input);
	void SetInput_Encoder(Input& input);
private:
	void RegisterInput(lv_indev_type_t indev_type, lv_indev_read_cb_t cb);
private:
	static void FlushCB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf);
	static void IndevReadPointerCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadKeypadCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadButtonCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadEncoderCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
};

}

#endif
