#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ILI9488.h"
#include "jxglib/LVGLAdapter.h"

using namespace jxglib;

void Setup();

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	::lv_init();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	//ILI9341 display(spi1, 240, 320, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	//ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
	ILI9488 display(spi1, 320, 480, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	ILI9488::TouchScreen touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
	display.Initialize(Display::Dir::Rotate270);
	touchScreen.Initialize(display);
	LVGLAdapter lvglAdapter(false);
	lvglAdapter.AttachOutput(display);
	lvglAdapter.AttachInput(touchScreen);
	lvglAdapter.AttachInput(UART::Default);
	Setup();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}

void Setup()
{
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
	::lv_example_scroll_1();
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
}
