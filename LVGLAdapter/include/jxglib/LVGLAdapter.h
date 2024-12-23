//==============================================================================
// jxglib/LVGLAdapter.h
//==============================================================================
#ifndef PICO_JXGLIB_LVGLAdapter_H
#define PICO_JXGLIB_LVGLAdapter_H
#include <lvgl.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"
#include "jxglib/TouchScreen.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"

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
	class InputTouchScreen : public LVGLAdapter::Input {
	private:
		TouchScreen* pTouchScreen_;
	public:
		InputTouchScreen() {}
	public:
		void SetTouchScreen(TouchScreen& touchScreen) { pTouchScreen_ = &touchScreen; }
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
	};
	class InputKeyUART : public LVGLAdapter::Input {
	private:
		VT100::Decoder& vt100Decoder_;
	public:
		InputKeyUART(VT100::Decoder& vt100Decoder) : vt100Decoder_{vt100Decoder} {}
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
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
	InputTouchScreen inputTouchScreen_;
	InputKeyUART inputKeyUART_;
	static InputDumb inputDumb_;
public:
	static VT100::Decoder vt100Decoder;
public:
	LVGLAdapter(bool doubleBuffFlag = true, int nPartial = 10);
public:
	Drawable& GetDrawableOut() { return *pDrawableOut_; }
	lv_display_t* Get_lv_display() { return disp_; }
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty);
	void Flush(lv_display_t* disp, const lv_area_t* area, unsigned char* buf);
	void SetDefault();
	lv_indev_t* SetInput_Pointer(Input& input);
	lv_indev_t* SetInput_Keypad(Input& input);
	lv_indev_t* SetInput_Button(Input& input);
	lv_indev_t* SetInput_Encoder(Input& input);
public:
	lv_indev_t* AttachInput(TouchScreen& touchScreen);
	lv_indev_t* AttachInput(UART& uart);
private:
	lv_indev_t* RegisterInput(lv_indev_type_t indev_type, lv_indev_read_cb_t cb);
private:
	static void FlushCB(lv_display_t* disp, const lv_area_t* area, unsigned char* buf);
	static void IndevReadPointerCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadKeypadCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadButtonCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void IndevReadEncoderCB(lv_indev_t* indev_drv, lv_indev_data_t* data);
	static void HandlerUART0(void);
	static void HandlerUART1(void);
};

}

#endif
