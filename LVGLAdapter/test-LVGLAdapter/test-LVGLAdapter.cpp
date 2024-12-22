#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ST7789.h"
#include "jxglib/LVGLAdapter.h"
#include <examples/lv_examples.h>
//#include <demos/lv_demos.h>

using namespace jxglib;

class InputPointer : public LVGLAdapter::Input {
private:
	
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputPointer::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Pointer::Handle\n");
	data->point.x = 0;
	data->point.y = 0;
	data->state = LV_INDEV_STATE_PRESSED;
}

class InputKeypad : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputKeypad::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Keypad::Handle\n");
	data->point.x = 0;
	data->point.y = 0;
	data->state = LV_INDEV_STATE_PRESSED;
	data->key = '1';
}

class InputButton : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputButton::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Button::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;
	data->btn_id = 0;
}

class InputEncoder : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputEncoder::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Encoder::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;	
	data->enc_diff = 0;
}

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display1(spi1, 240, 320, GPIO18, GPIO19, GPIO20, GPIO21);
	ILI9341 display2(spi0, 240, 320, GPIO12, GPIO11, GPIO13, GPIO::None);
	display1.Initialize(Display::Dir::Rotate90);
	display2.Initialize(Display::Dir::Rotate90);
	LVGLAdapter lvglAdapter1;
	LVGLAdapter lvglAdapter2;
	lvglAdapter1.AttachOutput(display1);
	lvglAdapter2.AttachOutput(display2);
	//lvglAdapter1.AttachOutput(display1, {0, 100, 320, 100});
	InputPointer inputPointer;
	InputKeypad inputKeypad;
	InputButton inputButton;
	InputEncoder inputEncoder;
	//lvglAdapter.SetInput_Pointer(inputPointer);
	lvglAdapter2.SetInput_Keypad(inputKeypad);
	//lvglAdapter.SetInput_Button(inputButton);
	//lvglAdapter.SetInput_Encoder(inputEncoder);
	//-----------------------------------------------
	// examples/anim/lv_example_anim.h
	//::lv_example_anim_1();
	//::lv_example_anim_2();
	//::lv_example_anim_3();
	//::lv_example_anim_timeline_1();
	//::lv_example_button_1();
	//-----------------------------------------------
	// examples/event/lv_example_event.h
	//::lv_example_event_click();
	//::lv_example_event_streak();
	//::lv_example_event_button();
	//::lv_example_event_bubble();
	//::lv_example_event_draw();
	//-----------------------------------------------
	// examples/get_started/lv_example_get_started.h
	//::lv_example_get_started_1();
	//::lv_example_get_started_2();
	//::lv_example_get_started_3();
	//::lv_example_get_started_4();
	//-----------------------------------------------
	// examples/layouts/lv_example_layout.h
	//::lv_example_flex_1();
	//::lv_example_flex_2();
	//::lv_example_flex_3();
	//::lv_example_flex_4();
	//::lv_example_flex_5();
	//::lv_example_flex_6();
	//::lv_example_grid_1();
	//::lv_example_grid_2();
	//::lv_example_grid_3();
	//::lv_example_grid_4();
	//::lv_example_grid_5();
	//::lv_example_grid_6();
	//-----------------------------------------------
	// examples/libs/lv_example_libs.h
	//::lv_example_barcode_1();
	//::lv_example_bmp_1();
	//::lv_example_ffmpeg_1();
	//::lv_example_ffmpeg_2();
	//::lv_example_freetype_1();
	//::lv_example_freetype_2();
	//::lv_example_gif_1();
	//::lv_example_libjpeg_turbo_1();
	//::lv_example_libpng_1();
	//::lv_example_lodepng_1();
	//::lv_example_qrcode_1();
	//::lv_example_rlottie_1();
	//::lv_example_rlottie_2();
	//::lv_example_svg_1();
	//::lv_example_tiny_ttf_1();
	//::lv_example_tiny_ttf_2();
	//::lv_example_tiny_ttf_3();
	//::lv_example_tjpgd_1();
	//-----------------------------------------------
	// examples/others/lv_example_others.h
	//::lv_example_file_explorer_1();
	//::lv_example_file_explorer_2();
	//::lv_example_file_explorer_3();
	//::lv_example_font_manager_1();
	//::lv_example_fragment_1();
	//::lv_example_fragment_2();
	//::lv_example_gestures();
	//::lv_example_gridnav_1();
	//::lv_example_gridnav_2();
	//::lv_example_gridnav_3();
	//::lv_example_gridnav_4();
	//::lv_example_gridnav_5();
	//::lv_example_ime_pinyin_1();
	//::lv_example_ime_pinyin_2();
	//::lv_example_imgfont_1();
	//::lv_example_monkey_1();
	//::lv_example_monkey_2();
	//::lv_example_monkey_3();
	//::lv_example_observer_1();
	//::lv_example_observer_2();
	//::lv_example_observer_3();
	//::lv_example_observer_4();
	//::lv_example_observer_5();
	//::lv_example_observer_6();
	//::lv_example_snapshot_1();
	//::lv_example_xml_1();
	//-----------------------------------------------
	// examples/porting/osal/lv_example_osal.h
	//-----------------------------------------------
	// examples/scroll/lv_example_scroll.h
	//::lv_example_scroll_1();
	//::lv_example_scroll_2();
	//::lv_example_scroll_3();
	//::lv_example_scroll_4();
	//::lv_example_scroll_5();
	//::lv_example_scroll_6();
	//::lv_example_scroll_7();
	//-----------------------------------------------
	// examples/styles/lv_example_style.h
	//::lv_example_style_1();
	//::lv_example_style_2();
	//::lv_example_style_3();
	//::lv_example_style_4();
	//::lv_example_style_5();
	//::lv_example_style_6();
	//::lv_example_style_7();
	//::lv_example_style_8();
	//::lv_example_style_9();
	//::lv_example_style_10();
	//::lv_example_style_11();
	//::lv_example_style_12();
	//::lv_example_style_13();
	//::lv_example_style_14();
	//::lv_example_style_15();
	//::lv_example_style_16();
	//::lv_example_style_17();
	//::lv_example_style_18();
	//-----------------------------------------------
	// examples/widgets/lv_example_widgets.h
	//::lv_example_animimg_1();
	//::lv_example_arc_1();
	//::lv_example_arc_2();
	//::lv_example_bar_1();
	//::lv_example_bar_2();
	//::lv_example_bar_3();
	//::lv_example_bar_4();
	//::lv_example_bar_5();
	//::lv_example_bar_6();
	//::lv_example_bar_7();
	//::lv_example_button_1();
	//::lv_example_button_2();
	//::lv_example_button_3();
	//::lv_example_buttonmatrix_1();
	//::lv_example_buttonmatrix_2();
	//::lv_example_buttonmatrix_3();
	//::lv_example_calendar_1();
	//::lv_example_calendar_2();
	//::lv_example_canvas_1();
	//::lv_example_canvas_2();
	//::lv_example_canvas_3();
	//::lv_example_canvas_4();
	//::lv_example_canvas_5();
	//::lv_example_canvas_6();
	//::lv_example_canvas_7();
	//::lv_example_canvas_8();
	//::lv_example_chart_1();
	//::lv_example_chart_2();
	//::lv_example_chart_3();
	//::lv_example_chart_4();
	//::lv_example_chart_5();
	//::lv_example_chart_6();
	//::lv_example_chart_7();
	//::lv_example_chart_8();
	//::lv_example_checkbox_1();
	//::lv_example_checkbox_2();
	//::lv_example_dropdown_1();
	//::lv_example_dropdown_2();
	//::lv_example_dropdown_3();
	//::lv_example_image_1();
	//::lv_example_image_2();
	//::lv_example_image_3();
	//::lv_example_image_4();
	//::lv_example_imagebutton_1();
	//::lv_example_keyboard_1();
	//::lv_example_keyboard_2();
	//::lv_example_label_1();
	//::lv_example_label_2();
	//::lv_example_label_3();
	//::lv_example_label_4();
	//::lv_example_label_5();
	//::lv_example_led_1();
	//::lv_example_line_1();
	//::lv_example_list_1();
	//::lv_example_list_2();
	//::lv_example_lottie_1();
	//::lv_example_lottie_2();
	//::lv_example_menu_1();
	//::lv_example_menu_2();
	//::lv_example_menu_3();
	//::lv_example_menu_4();
	//::lv_example_menu_5();
	//::lv_example_msgbox_1();
	//::lv_example_msgbox_2();
	//::lv_example_obj_1();
	//::lv_example_obj_2();
	//::lv_example_obj_3();
	//::lv_example_roller_1();
	//::lv_example_roller_2();
	//::lv_example_roller_3();
	//::lv_example_scale_1();
	//::lv_example_scale_2();
	//::lv_example_scale_3();
	//::lv_example_scale_4();
	//::lv_example_scale_5();
	//::lv_example_scale_6();
	//::lv_example_scale_7();
	//::lv_example_scale_8();
	//::lv_example_scale_9();
	//::lv_example_slider_1();
	//::lv_example_slider_2();
	//::lv_example_slider_3();
	//::lv_example_slider_4();
	//::lv_example_span_1();
	//::lv_example_spinbox_1();
	//::lv_example_spinner_1();
	//::lv_example_switch_1();
	//::lv_example_switch_2();
	//::lv_example_table_1();
	//::lv_example_table_2();
	//::lv_example_tabview_1();
	//::lv_example_tabview_2();
	//::lv_example_textarea_1();
	//::lv_example_textarea_2();
	//::lv_example_textarea_1();
	//::lv_example_textarea_1();
	//::lv_example_textarea_3();
	//::lv_example_tileview_1();
	//::lv_example_win_1();
	do {
		lv_obj_t* button = lv_button_create(lv_screen_active());
		//lv_obj_add_event_cb(button, event_handler, LV_EVENT_ALL, NULL);
		lv_obj_align(button, LV_ALIGN_CENTER, 0, -40);
		lv_obj_set_width(button, 200);
		lv_obj_remove_flag(button, LV_OBJ_FLAG_PRESS_LOCK);
		do {
			lv_obj_t* label = lv_label_create(button);
			lv_label_set_text(label, "Button");
			lv_obj_center(label);
		} while (0);
	} while (0);
	//do {
	//	lvglAdapter2.SetDefault();
	//	lv_obj_t* btn1 = lv_button_create(lv_screen_active());
	//	//lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	//	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
	//	lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
	//	lv_obj_t* label = lv_label_create(btn1);
	//	lv_label_set_text(label, "Button");
	//	lv_obj_center(label);
	//} while (0);
	//::printf("----1\n");
	//lv_refr_now(lvglAdapter1.Get_lv_display());
	//::printf("----2\n");
	//lv_refr_now(lvglAdapter2.Get_lv_display());
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}
