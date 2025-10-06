//==============================================================================
// LVGL::Adapter.cpp
//==============================================================================
#include "jxglib/LVGL.h"

namespace jxglib::LVGL {

//------------------------------------------------------------------------------
// LVGL::Adapter
//------------------------------------------------------------------------------
Adapter::InputDumb Adapter::inputDumb_;

const uint8_t Adapter::argbData_MouseCursor[] = {
	0b10000000,0b00000000,
	0b11000000,0b00000000,
	0b11100000,0b00000000,
	0b11110000,0b00000000,
	0b11111000,0b00000000,
	0b11111100,0b00000000,
	0b11111110,0b00000000,
	0b11111111,0b00000000,
	0b11111111,0b10000000,
	0b11111110,0b11000000,
	0b10011110,0b00000000,
	0b00011110,0b00000000,
	0b00011110,0b00000000,
	0b00001111,0b00000000,
	0b00001111,0b00000000,
	0b00001111,0b00000000
};

const lv_image_dsc_t Adapter::imageDsc_MouseCursor = {
	header: {
		cf: LV_COLOR_FORMAT_A1,
		w: 16,
		h: 16,
	},
	data_size: sizeof(argbData_MouseCursor),
	data: argbData_MouseCursor,
};

Adapter::Adapter() : doubleBuffFlag_{false}, nPixelsBuff_{-1}, nPartialBuff_{10}, pDrawableOut_{nullptr}, disp_{nullptr},
	pInput_Pointer_{&inputDumb_}, pInput_Keypad_{&inputDumb_}, pInput_Button_{&inputDumb_}, pInput_Encoder_{&inputDumb_},
	inputTouchScreen_(*this)
{}

Adapter& Adapter::AttachDrawable(Drawable& drawable, const Rect& rect)
{
	pDrawableOut_ = &drawable;
	rectOut_ = rect;
	Rect rectBound(0, 0, drawable.GetWidth(), drawable.GetHeight());
	if (rectOut_.IsEmpty()) {
		rectOut_ = rectBound;
	} else if (!rectOut_.Adjust(rectBound)) {
		::panic("failed to adjust bounding rectangle");
		return *this;
	}
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
		::panic("failed to allocate the first buffer (%dbytes) in LVGL::Adapter::AttachDrawable()", bytesBuff);
		return *this;
	}
	void* buff2 = nullptr;
	if (doubleBuffFlag_) {
		buff2 = ::lv_malloc(bytesBuff);
		if (!buff2) {
			::panic("failed to allocate the second buffer (%dbytes) in LVGL::Adapter::AttachDrawable()", bytesBuff);
			return *this;
		}
	}
	::lv_display_set_buffers(disp_, buff1, buff2, bytesBuff, LV_DISPLAY_RENDER_MODE_PARTIAL);
	SetDefault();	// just for the convenience in the following process
	return *this;
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

Adapter& Adapter::AttachTouchScreen(TouchScreen::Base& touchScreen, lv_indev_t** p_indev)
{
	inputTouchScreen_.SetTouchScreen(touchScreen);
	lv_indev_t* indev = SetInput_Pointer(inputTouchScreen_);
	if (p_indev) *p_indev = indev;
	return *this;
}

Adapter& Adapter::AttachMouse(Mouse& mouse, lv_indev_t** p_indev)
{
	inputMouse_.SetMouse(mouse);
	lv_indev_t* indev = SetInput_Pointer(inputMouse_);
	// https://docs.lvgl.io/master/details/main-modules/indev.html
	lv_obj_t* objCursor = lv_image_create(::lv_screen_active());	// Create image Widget for cursor.
	::lv_image_set_src(objCursor, &imageDsc_MouseCursor);			// Set image source.
	::lv_indev_set_cursor(indev, objCursor);						// Connect image to Input Device.
	Mouse::Status status = mouse.CaptureStatus();
	::lv_obj_set_pos(objCursor, status.GetPoint().x, status.GetPoint().y);
	if (p_indev) *p_indev = indev;
	mouse.SetStage(GetRectOut());
	return *this;
}

Adapter& Adapter::AttachKeyboard(Keyboard& keyboard, lv_indev_t** p_indev, bool setGroupFlag)
{
	inputKeyboard_.SetKeyboard(keyboard);
	lv_indev_t* indev = SetInput_Keypad(inputKeyboard_);
	if (setGroupFlag) {
		lv_group_t* group = ::lv_group_create();
		::lv_group_set_default(group);
		::lv_indev_set_group(indev, group);
	}
	if (p_indev) *p_indev = indev;
	return *this;
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
// Adapter::InputMouse
//------------------------------------------------------------------------------
void Adapter::InputMouse::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	Mouse::Status status = pMouse_->CaptureStatus();
	data->state = status.GetButtonLeft()? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	data->point.x = status.GetPoint().x;
	data->point.y = status.GetPoint().y;
}

//------------------------------------------------------------------------------
// Adapter::InputKeyboard
//------------------------------------------------------------------------------
void Adapter::InputKeyboard::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	KeyData keyData;
	if (pKeyboard_->SenseKeyData(&keyData) == 0) {
		data->state = LV_INDEV_STATE_RELEASED;
	} else if (keyData.IsKeyCode()) {
		uint8_t keyCode = keyData.GetKeyCode();
		data->key =
			(keyCode == VK_TAB)?	(keyData.IsShiftDown()? LV_KEY_PREV : LV_KEY_NEXT) :
			(keyCode == VK_PRIOR)?	LV_KEY_PREV :
			(keyCode == VK_NEXT)?	LV_KEY_NEXT :
			(keyCode == VK_RETURN)?	LV_KEY_ENTER :
			(keyCode == VK_UP)?		LV_KEY_UP :
			(keyCode == VK_DOWN)?	LV_KEY_DOWN :
			(keyCode == VK_LEFT)?	LV_KEY_LEFT :
			(keyCode == VK_RIGHT)?	LV_KEY_RIGHT :
			(keyCode == VK_ESCAPE)?	LV_KEY_ESC :
			(keyCode == VK_DELETE)?	LV_KEY_DEL :
			(keyCode == VK_BACK)?	LV_KEY_BACKSPACE :
			(keyCode == VK_HOME)?	LV_KEY_HOME :
			(keyCode == VK_END)?	LV_KEY_END : 0;
		data->state = data->key? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	} else {
		data->key = keyData.GetChar();
		if (data->key == '\t') data->key = keyData.IsShiftDown()? LV_KEY_PREV : LV_KEY_NEXT;
		data->state = LV_INDEV_STATE_PRESSED;
	}
}

}
