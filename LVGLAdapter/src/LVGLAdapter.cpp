//==============================================================================
// LVGLAdapter.cpp
//==============================================================================
#include "jxglib/LVGLAdapter.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LVGLAdapter
//------------------------------------------------------------------------------
LVGLAdapter::InputDumb LVGLAdapter::inputDumb_;

LVGLAdapter::LVGLAdapter(bool doubleBuffFlag, int nPartial) :
	doubleBuffFlag_{doubleBuffFlag}, nPartial_{nPartial}, pDrawableOut_{nullptr}, disp_{nullptr},
	pInput_Pointer_{&inputDumb_}, pInput_Keypad_{&inputDumb_}, pInput_Button_{&inputDumb_}, pInput_Encoder_{&inputDumb_}
{}

bool LVGLAdapter::AttachOutput(Drawable& drawable, const Rect& rect)
{
	pDrawableOut_ = &drawable;
	::lv_init();
	rectOut_ = rect;
	Rect rectBound(0, 0, drawable.GetWidth(), drawable.GetHeight());
	if (rectOut_.IsEmpty()) { rectOut_ = rectBound; } else if (!rectOut_.Adjust(rectBound)) return false;
	disp_ = ::lv_display_create(rectOut_.width, rectOut_.height);
	::lv_display_set_flush_cb(disp_, FlushCB);
	::lv_display_set_user_data(disp_, this);
	uint32_t buffSize = rectOut_.width * rectOut_.height / nPartial_ * ::lv_color_format_get_size(::lv_display_get_color_format(disp_));
	void* buff1 = ::lv_malloc(buffSize);
	if (!buff1) return false;
	void* buff2 = nullptr;
	if (doubleBuffFlag_) {
		buff2 = ::lv_malloc(buffSize);
		if (!buff2) return false;
	}
	::lv_display_set_buffers(disp_, buff1, buff2, buffSize,
				(nPartial_ > 1)? LV_DISPLAY_RENDER_MODE_PARTIAL : LV_DISPLAY_RENDER_MODE_FULL);
	return true;
}

void LVGLAdapter::Flush(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Image image(Image::Format::RGB565, ::lv_area_get_width(area), ::lv_area_get_height(area), buf);
	GetDrawableOut().DrawImageFast(rectOut_.x + area->x1, rectOut_.y + area->y1, image);
	::lv_disp_flush_ready(disp);
}

void LVGLAdapter::SetInput_Pointer(Input& input)
{
	pInput_Pointer_ = &input;
	RegisterInput(LV_INDEV_TYPE_POINTER, IndevReadPointerCB);
}

void LVGLAdapter::SetInput_Keypad(Input& input)
{
	pInput_Keypad_ = &input;
	RegisterInput(LV_INDEV_TYPE_KEYPAD, IndevReadKeypadCB);
}

void LVGLAdapter::SetInput_Button(Input& input)
{
	pInput_Button_ = &input;
	RegisterInput(LV_INDEV_TYPE_BUTTON, IndevReadButtonCB);
}

void LVGLAdapter::SetInput_Encoder(Input& input)
{
	pInput_Encoder_ = &input;
	RegisterInput(LV_INDEV_TYPE_ENCODER, IndevReadEncoderCB);
}

void LVGLAdapter::RegisterInput(lv_indev_type_t indev_type, lv_indev_read_cb_t cb)
{
	lv_indev_t* indev = ::lv_indev_create();
	::lv_indev_set_type(indev, indev_type);
	::lv_indev_set_read_cb(indev, cb);
	::lv_indev_set_user_data(indev, this);
	::lv_indev_set_display(indev, disp_);
}

void LVGLAdapter::FlushCB(lv_disp_t* disp, const lv_area_t* area, unsigned char* buf)
{
	LVGLAdapter* pSelf = reinterpret_cast<LVGLAdapter*>(::lv_display_get_user_data(disp));
	pSelf->Flush(disp, area, buf);
}

void LVGLAdapter::IndevReadPointerCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	LVGLAdapter* pSelf = reinterpret_cast<LVGLAdapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Pointer_->Handle(indev, data);
}

void LVGLAdapter::IndevReadKeypadCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	LVGLAdapter* pSelf = reinterpret_cast<LVGLAdapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Keypad_->Handle(indev, data);
}

void LVGLAdapter::IndevReadButtonCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	LVGLAdapter* pSelf = reinterpret_cast<LVGLAdapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Button_->Handle(indev, data);
}

void LVGLAdapter::IndevReadEncoderCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	LVGLAdapter* pSelf = reinterpret_cast<LVGLAdapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Encoder_->Handle(indev, data);
}

}
