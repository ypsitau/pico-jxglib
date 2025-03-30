//==============================================================================
// jxglib/LVGL/Adapter.h
//==============================================================================
#ifndef PICO_JXGLIB_LVGL_Adapter_H
#define PICO_JXGLIB_LVGL_Adapter_H
#include <lvgl/lvgl.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"
#include "jxglib/Mouse.h"
#include "jxglib/Keyboard.h"
#include "jxglib/TouchScreen.h"

namespace jxglib::LVGL {

//------------------------------------------------------------------------------
// LVGL::Adapter
//------------------------------------------------------------------------------
class Adapter {
public:
	class Input {
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) = 0;
	};
	class InputDumb : public Input {
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override {};
	};
	class InputTouchScreen : public Input {
	private:
		Adapter& adapter_;
		TouchScreen* pTouchScreen_;
	public:
		InputTouchScreen(Adapter& adapter) : adapter_{adapter} {}
	public:
		void SetTouchScreen(TouchScreen& touchScreen) { pTouchScreen_ = &touchScreen; }
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
	};
	class InputMouse : public Input {
	private:
		Mouse* pMouse_;
	public:
		InputMouse() {}
	public:
		void SetMouse(Mouse& mouse) { pMouse_ = &mouse; }
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
	};
	class InputKeyboard : public Input {
	private:
		Keyboard* pKeyboard_;
	public:
		InputKeyboard() : pKeyboard_{&KeyboardDumb::Instance} {}
	public:
		void SetKeyboard(Keyboard& keyboard) { pKeyboard_ = &keyboard; }
	public:
		virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
	};
	class DrawImageFastHandler : public Drawable::DrawImageFastHandler {
	public:
		lv_display_t* disp;
	public:
		virtual void OnDrawImageFastCompleted() override { ::lv_display_flush_ready(disp); }
	};
private:
	bool doubleBuffFlag_;
	int nPixelsBuff_;
	int nPartialBuff_;
	Drawable* pDrawableOut_;
	Rect rectOut_;
	lv_display_t* disp_;
	Input* pInput_Pointer_;
	Input* pInput_Keypad_;
	Input* pInput_Button_;
	Input* pInput_Encoder_;
	DrawImageFastHandler drawImageFastHandler_;
private:
	InputTouchScreen inputTouchScreen_;
	InputMouse inputMouse_;
	InputKeyboard inputKeyboard_;
public:
	static InputDumb inputDumb_;
	static const uint8_t argbData_MouseCursor[];
	static const lv_image_dsc_t imageDsc_MouseCursor;
public:
	Adapter();
public:
	Drawable& GetDrawableOut() { return *pDrawableOut_; }
	lv_display_t* Get_lv_display() { return disp_; }
	Adapter& EnableDoubleBuff(bool doubleBuffFlag = true) { doubleBuffFlag_ = doubleBuffFlag; return *this; }
	Adapter& SetPixelsBuff(int nPixelsBuff) { nPixelsBuff_ = nPixelsBuff; return *this; }
	Adapter& SetPartialNum(int nPartialBuff) { nPartialBuff_ = nPartialBuff; return *this; }
	void Flush(lv_display_t* disp, const lv_area_t* area, unsigned char* buf);
	void SetDefault();
	const Rect& GetRectOut() const { return rectOut_; }
	lv_indev_t* SetInput_Pointer(Input& input);
	lv_indev_t* SetInput_Keypad(Input& input);
	lv_indev_t* SetInput_Button(Input& input);
	lv_indev_t* SetInput_Encoder(Input& input);
private:
	lv_indev_t* RegisterInput(lv_indev_type_t indev_type, lv_indev_read_cb_t cb);
public:
	Adapter& AttachDrawable(Drawable& drawable, const Rect& rect = Rect::Empty);
	Adapter& AttachCanvas(Drawable& canvas, const Rect& rect = Rect::Empty) { return AttachDrawable(canvas, rect); }
	Adapter& AttachDisplay(Drawable& display, const Rect& rect = Rect::Empty) { return AttachDrawable(display, rect); }
	Adapter& AttachMouse(Mouse& mouse, lv_indev_t** p_indev = nullptr);
	Adapter& AttachTouchScreen(TouchScreen& touchScreen, lv_indev_t** p_indev = nullptr);
	Adapter& AttachKeyboard(Keyboard& keyboard, lv_indev_t** p_indev = nullptr, bool setGroupFlag = true);
private:
	static void FlushCB(lv_display_t* disp, const lv_area_t* area, unsigned char* buf);
	static void IndevReadPointerCB(lv_indev_t* indev, lv_indev_data_t* data);
	static void IndevReadKeypadCB(lv_indev_t* indev, lv_indev_data_t* data);
	static void IndevReadButtonCB(lv_indev_t* indev, lv_indev_data_t* data);
	static void IndevReadEncoderCB(lv_indev_t* indev, lv_indev_data_t* data);
};

}

#endif
