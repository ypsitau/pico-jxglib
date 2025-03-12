//==============================================================================
// LVGL::Adapter.cpp
//==============================================================================
#include "jxglib/LVGL.h"

namespace jxglib::LVGL {

//------------------------------------------------------------------------------
// LVGL::Adapter
//------------------------------------------------------------------------------
Adapter::InputDumb Adapter::inputDumb_;
VT100::Decoder Adapter::vt100Decoder;

Adapter::Adapter() : doubleBuffFlag_{false}, nPixelsBuff_{-1}, nPartialBuff_{10}, pDrawableOut_{nullptr}, disp_{nullptr},
	pInput_Pointer_{&inputDumb_}, pInput_Keypad_{&inputDumb_}, pInput_Button_{&inputDumb_}, pInput_Encoder_{&inputDumb_},
	inputTouchScreen_(*this), inputKeyUART_(vt100Decoder)
{}

bool Adapter::AttachOutput(Drawable& drawable, const Rect& rect, bool requiredFlag)
{
	pDrawableOut_ = &drawable;
	rectOut_ = rect;
	Rect rectBound(0, 0, drawable.GetWidth(), drawable.GetHeight());
	if (rectOut_.IsEmpty()) { rectOut_ = rectBound; } else if (!rectOut_.Adjust(rectBound)) return false;
	disp_ = ::lv_display_create(rectOut_.width, rectOut_.height);
	::lv_display_set_color_format(disp_,
			drawable.IsFormatRGB565()? LV_COLOR_FORMAT_RGB565 :
			drawable.IsFormatRGB()? LV_COLOR_FORMAT_RGB888 :
			drawable.IsFormatBitmap()? LV_COLOR_FORMAT_I1 : LV_COLOR_FORMAT_RGB565);
	::lv_display_set_flush_cb(disp_, FlushCB);
	::lv_display_set_user_data(disp_, this);
	uint32_t bytesBuff = (nPixelsBuff_ < 0)? (rectOut_.width * rectOut_.height / nPartialBuff_) : nPixelsBuff_;
	if (drawable.GetFormat().IsBitmap()) {
		bytesBuff /= 8;
		if (bytesBuff < 1024) bytesBuff = 1024;
	} else {
		bytesBuff *= ::lv_color_format_get_size(::lv_display_get_color_format(disp_));
	}
	void* buff1 = ::lv_malloc(bytesBuff);
	if (!buff1) {
		if (requiredFlag) panic("can't allocate the first buffer (%dbytes)", bytesBuff);
		return false;
	}
	void* buff2 = nullptr;
	if (doubleBuffFlag_) {
		buff2 = ::lv_malloc(bytesBuff);
		if (!buff2) {
			if (requiredFlag) panic("can't allocate the second buffer (%dbytes)", bytesBuff);
			return false;
		}
	}
	::lv_display_set_buffers(disp_, buff1, buff2, bytesBuff, LV_DISPLAY_RENDER_MODE_PARTIAL);
	SetDefault();	// just for the convenience in the following process
	return true;
}

void Adapter::SetDefault()
{
	::lv_display_set_default(disp_);
}

void Adapter::Flush(lv_display_t* disp, const lv_area_t* area, unsigned char* buf)
{
	Image image(GetDrawableOut().GetFormat(), ::lv_area_get_width(area), ::lv_area_get_height(area), buf);
	drawImageFastHandler_.disp = disp;	
	GetDrawableOut().DrawImageFast(rectOut_.x + area->x1, rectOut_.y + area->y1, image, !doubleBuffFlag_, &drawImageFastHandler_);
}

lv_indev_t* Adapter::SetInput_Pointer(Input& input)
{
	pInput_Pointer_ = &input;
	return RegisterInput(LV_INDEV_TYPE_POINTER, IndevReadPointerCB);
}

lv_indev_t* Adapter::SetInput_Keypad(Input& input)
{
	pInput_Keypad_ = &input;
	return RegisterInput(LV_INDEV_TYPE_KEYPAD, IndevReadKeypadCB);
}

lv_indev_t* Adapter::SetInput_Button(Input& input)
{
	pInput_Button_ = &input;
	return RegisterInput(LV_INDEV_TYPE_BUTTON, IndevReadButtonCB);
}

lv_indev_t* Adapter::SetInput_Encoder(Input& input)
{
	pInput_Encoder_ = &input;
	return RegisterInput(LV_INDEV_TYPE_ENCODER, IndevReadEncoderCB);
}

lv_indev_t* Adapter::AttachInput(TouchScreen& touchScreen)
{
	inputTouchScreen_.SetTouchScreen(touchScreen);
	return SetInput_Pointer(inputTouchScreen_);
}

lv_indev_t* Adapter::AttachInput(UART& uart, bool setGroupFlag)
{
	uart.irq_add_shared_handler((uart.raw.get_index() == 0)? HandlerUART0 : HandlerUART1).irq_set_enabled(true);
	uart.raw.set_irq_enables(true, false);
	lv_indev_t* indev = SetInput_Keypad(inputKeyUART_);
	if (setGroupFlag) {
		lv_group_t* group = ::lv_group_create();
		::lv_group_set_default(group);
		::lv_indev_set_group(indev, group);
	}
	return indev;
}

lv_indev_t* Adapter::RegisterInput(lv_indev_type_t indev_type, lv_indev_read_cb_t cb)
{
	lv_indev_t* indev = ::lv_indev_create();
	::lv_indev_set_type(indev, indev_type);
	::lv_indev_set_read_cb(indev, cb);
	::lv_indev_set_user_data(indev, this);
	::lv_indev_set_display(indev, disp_);
	SetDefault();	// just for the convenience in the following process
	return indev;
}

void Adapter::FlushCB(lv_display_t* disp, const lv_area_t* area, unsigned char* buf)
{
	//::printf("FlushCB(x1:%d, y1:%d, x2:%d, y2:%d)\n", area->x1, area->y1, area->x2, area->y2);
	Adapter* pSelf = reinterpret_cast<Adapter*>(::lv_display_get_user_data(disp));
	pSelf->Flush(disp, area, buf);
}

void Adapter::IndevReadPointerCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	Adapter* pSelf = reinterpret_cast<Adapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Pointer_->Handle(indev, data);
}

void Adapter::IndevReadKeypadCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	Adapter* pSelf = reinterpret_cast<Adapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Keypad_->Handle(indev, data);
}

void Adapter::IndevReadButtonCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	Adapter* pSelf = reinterpret_cast<Adapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Button_->Handle(indev, data);
}

void Adapter::IndevReadEncoderCB(lv_indev_t* indev, lv_indev_data_t* data)
{
	Adapter* pSelf = reinterpret_cast<Adapter*>(::lv_indev_get_user_data(indev));
	pSelf->pInput_Encoder_->Handle(indev, data);
}

void Adapter::HandlerUART0(void)
{
	UART& uart = UART0;
	while (uart.raw.is_readable()) vt100Decoder.FeedChar(uart.raw.getc());
}

void Adapter::HandlerUART1(void)
{
	UART& uart = UART1;
	while (uart.raw.is_readable()) vt100Decoder.FeedChar(uart.raw.getc());
}

//------------------------------------------------------------------------------
// Adapter::InputTouchScreen
//------------------------------------------------------------------------------
void Adapter::InputTouchScreen::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	Point pt;
	const Rect& rectOut = adapter_.GetRectOut();
	bool touchFlag = pTouchScreen_->ReadPoint(&pt);
	data->state = touchFlag? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	data->point.x = pt.x - rectOut.x;
	data->point.y = pt.y - rectOut.y;
}

//------------------------------------------------------------------------------
// Adapter::InputKeyUART
//------------------------------------------------------------------------------
void Adapter::InputKeyUART::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	int keyData;
	bool vkFlag;
	if (vt100Decoder_.GetKeyData(&keyData, &vkFlag)) {
		if (vkFlag) {
			data->key =
				(keyData == VK_TAB)?	LV_KEY_NEXT :
				(keyData == VK_PRIOR)?	LV_KEY_PREV :
				(keyData == VK_NEXT)?	LV_KEY_NEXT :
				(keyData == VK_RETURN)?	LV_KEY_ENTER :
				(keyData == VK_UP)?		LV_KEY_UP :
				(keyData == VK_DOWN)?	LV_KEY_DOWN :
				(keyData == VK_LEFT)?	LV_KEY_LEFT :
				(keyData == VK_RIGHT)?	LV_KEY_RIGHT :
				(keyData == VK_ESCAPE)?	LV_KEY_ESC :
				(keyData == VK_DELETE)?	LV_KEY_DEL :
				(keyData == VK_BACK)?	LV_KEY_BACKSPACE :
				(keyData == VK_HOME)?	LV_KEY_HOME :
				(keyData == VK_END)?	LV_KEY_END : 0;
		} else {
			data->key = keyData;
		}
		data->state = LV_INDEV_STATE_PRESSED;
	} else {
		data->state = LV_INDEV_STATE_RELEASED;
	}
}

}
