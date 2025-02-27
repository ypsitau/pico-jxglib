#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ILI9488.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

struct Entry {
	const char* name;
	void (*func)();
};

const Entry entryTbl[] = {
	//-----------------------------------------------
	// examples/anim/lv_example_anim.h
	{ "anim_1",			::lv_example_anim_1				},
	{ "anim_2",			::lv_example_anim_2				},
	{ "anim_3",			::lv_example_anim_3				},
	{ "anim_timeline_1",::lv_example_anim_timeline_1	},
	{ "button_1",		::lv_example_button_1			},
	//-----------------------------------------------
	// examples/event/lv_example_event.h
	{ "event_click",	::lv_example_event_click		},
	{ "event_streak",	::lv_example_event_streak		},
	{ "event_button",	::lv_example_event_button		},
	{ "event_bubble",	::lv_example_event_bubble		},
	{ "event_draw",		::lv_example_event_draw			},
	//-----------------------------------------------
	// examples/get_started/lv_example_get_started.h
	{ "get_started_1",	::lv_example_get_started_1		},
	{ "get_started_2",	::lv_example_get_started_2		},
	{ "get_started_3",	::lv_example_get_started_3		},
	{ "get_started_4",	::lv_example_get_started_4		},
	//-----------------------------------------------
	// examples/layouts/lv_example_layout.h
	{ "flex_1",			::lv_example_flex_1				},
	{ "flex_2",			::lv_example_flex_2				},
	{ "flex_3",			::lv_example_flex_3				},
	{ "flex_4",			::lv_example_flex_4				},
	{ "flex_5",			::lv_example_flex_5				},
	{ "flex_6",			::lv_example_flex_6				},
	{ "grid_1",			::lv_example_grid_1				},
	{ "grid_2",			::lv_example_grid_2				},
	{ "grid_3",			::lv_example_grid_3				},
	{ "grid_4",			::lv_example_grid_4				},
	{ "grid_5",			::lv_example_grid_5				},
	{ "grid_6",			::lv_example_grid_6				},
	//-----------------------------------------------
	// examples/libs/lv_example_libs.h
//	{ "barcode_1",		::lv_example_barcode_1			},
//	{ "bmp_1",			::lv_example_bmp_1				},
	{ "ffmpeg_1",		::lv_example_ffmpeg_1			},
	{ "ffmpeg_2",		::lv_example_ffmpeg_2			},
	{ "freetype_1",		::lv_example_freetype_1			},
	{ "freetype_2",		::lv_example_freetype_2			},
//	{ "gif_1",			::lv_example_gif_1				},
	{ "libjpeg_turbo_1",::lv_example_libjpeg_turbo_1	},
	{ "libpng_1",		::lv_example_libpng_1			},
//	{ "lodepng_1",		::lv_example_lodepng_1			},
//	{ "qrcode_1",		::lv_example_qrcode_1			},
	{ "rlottie_1",		::lv_example_rlottie_1			},
	{ "rlottie_2",		::lv_example_rlottie_2			},
	{ "svg_1",			::lv_example_svg_1				},
//	{ "tiny_ttf_1",		::lv_example_tiny_ttf_1			},
//	{ "tiny_ttf_2",		::lv_example_tiny_ttf_2			},
//	{ "tiny_ttf_3",		::lv_example_tiny_ttf_3			},
//	{ "tjpgd_1",		::lv_example_tjpgd_1			},
	//-----------------------------------------------
	// examples/others/lv_example_others.h
//	{ "file_explorer_1",::lv_example_file_explorer_1	},
//	{ "file_explorer_2",::lv_example_file_explorer_2	},
//	{ "file_explorer_3",::lv_example_file_explorer_3	},
	{ "font_manager_1",	::lv_example_font_manager_1		},
//	{ "fragment_1",		::lv_example_fragment_1			},
//	{ "fragment_2",		::lv_example_fragment_2			},
//	{ "gestures",		::lv_example_gestures			},
//	{ "gridnav_1",		::lv_example_gridnav_1			},
//	{ "gridnav_2",		::lv_example_gridnav_2			},
//	{ "gridnav_3",		::lv_example_gridnav_3			},
//	{ "gridnav_4",		::lv_example_gridnav_4			},
//	{ "gridnav_5",		::lv_example_gridnav_5			},
//	{ "ime_pinyin_1",	::lv_example_ime_pinyin_1		},
//	{ "ime_pinyin_2",	::lv_example_ime_pinyin_2		},
	{ "imgfont_1",		::lv_example_imgfont_1			},
//	{ "monkey_1",		::lv_example_monkey_1			},
//	{ "monkey_2",		::lv_example_monkey_2			},
//	{ "monkey_3",		::lv_example_monkey_3			},
	{ "observer_1",		::lv_example_observer_1			},
	{ "observer_2",		::lv_example_observer_2			},
//	{ "observer_3",		::lv_example_observer_3			},
//	{ "observer_4",		::lv_example_observer_4			},
	{ "observer_5",		::lv_example_observer_5			},
	{ "observer_6",		::lv_example_observer_6			},
//	{ "snapshot_1",		::lv_example_snapshot_1				},
//	{ "xml_1",			::lv_example_xml_1				},
	//-----------------------------------------------
	// examples/porting/osal/lv_example_osal.h
	//-----------------------------------------------
	// examples/scroll/lv_example_scroll.h
	{ "scroll_1",		::lv_example_scroll_1			},
	{ "scroll_2",		::lv_example_scroll_2			},
	{ "scroll_3",		::lv_example_scroll_3			},
	{ "scroll_4",		::lv_example_scroll_4			},
//	{ "scroll_5",		::lv_example_scroll_5			},
	{ "scroll_6",		::lv_example_scroll_6			},
	{ "scroll_7",		::lv_example_scroll_7			},
	//-----------------------------------------------
	// examples/styles/lv_example_style.h
	{ "style_1",		::lv_example_style_1			},
	{ "style_2",		::lv_example_style_2			},
	{ "style_3",		::lv_example_style_3			},
	{ "style_4",		::lv_example_style_4			},
	{ "style_5",		::lv_example_style_5			},
	{ "style_6",		::lv_example_style_6			},
	{ "style_7",		::lv_example_style_7			},
	{ "style_8",		::lv_example_style_8			},
	{ "style_9",		::lv_example_style_9			},
	{ "style_10",		::lv_example_style_10			},
	{ "style_11",		::lv_example_style_11			},
	{ "style_12",		::lv_example_style_12			},
	{ "style_13",		::lv_example_style_13			},
	{ "style_14",		::lv_example_style_14			},
	{ "style_15",		::lv_example_style_15			},
	{ "style_16",		::lv_example_style_16			},
	{ "style_17",		::lv_example_style_17			},
	{ "style_18",		::lv_example_style_18			},
	//-----------------------------------------------
	// examples/widgets/lv_example_widgets.h
	{ "animimg_1",		::lv_example_animimg_1			},
	{ "arc_1",			::lv_example_arc_1				},
	{ "arc_2",			::lv_example_arc_2				},
	{ "bar_1",			::lv_example_bar_1				},
	{ "bar_2",			::lv_example_bar_2				},
	{ "bar_3",			::lv_example_bar_3				},
	{ "bar_4",			::lv_example_bar_4				},
	{ "bar_5",			::lv_example_bar_5				},
	{ "bar_6",			::lv_example_bar_6				},
	{ "bar_7",			::lv_example_bar_7				},
	{ "button_1",		::lv_example_button_1			},
	{ "button_2",		::lv_example_button_2			},
	{ "button_3",		::lv_example_button_3			},
	{ "buttonmatrix_1",	::lv_example_buttonmatrix_1		},
	{ "buttonmatrix_2",	::lv_example_buttonmatrix_2		},
	{ "buttonmatrix_3",	::lv_example_buttonmatrix_3		},
	{ "calendar_1",		::lv_example_calendar_1			},
	{ "calendar_2",		::lv_example_calendar_2			},
//	{ "canvas_1",		::lv_example_canvas_1			},
//	{ "canvas_2",		::lv_example_canvas_2			},
//	{ "canvas_3",		::lv_example_canvas_3			},
//	{ "canvas_4",		::lv_example_canvas_4			},
//	{ "canvas_5",		::lv_example_canvas_5			},
//	{ "canvas_6",		::lv_example_canvas_6			},
//	{ "canvas_7",		::lv_example_canvas_7			},
//	{ "canvas_8",		::lv_example_canvas_8			},
	{ "chart_1",		::lv_example_chart_1			},
	{ "chart_2",		::lv_example_chart_2			},
	{ "chart_3",		::lv_example_chart_3			},
	{ "chart_4",		::lv_example_chart_4			},
	{ "chart_5",		::lv_example_chart_5			},
	{ "chart_6",		::lv_example_chart_6			},
	{ "chart_7",		::lv_example_chart_7			},
	{ "chart_8",		::lv_example_chart_8			},
	{ "checkbox_1",		::lv_example_checkbox_1			},
	{ "checkbox_2",		::lv_example_checkbox_2			},
	{ "dropdown_1",		::lv_example_dropdown_1			},
	{ "dropdown_2",		::lv_example_dropdown_2			},
	{ "dropdown_3",		::lv_example_dropdown_3			},
	{ "image_1",		::lv_example_image_1			},
	{ "image_2",		::lv_example_image_2			},
	{ "image_3",		::lv_example_image_3			},
	{ "image_4",		::lv_example_image_4			},
	{ "imagebutton_1",	::lv_example_imagebutton_1		},
	{ "keyboard_1",		::lv_example_keyboard_1			},
	{ "keyboard_2",		::lv_example_keyboard_2			},
	{ "label_1",		::lv_example_label_1			},
	{ "label_2",		::lv_example_label_2			},
//	{ "label_3",		::lv_example_label_3			},
//	{ "label_4",		::lv_example_label_4			},
	{ "label_5",		::lv_example_label_5			},
	{ "led_1",			::lv_example_led_1				},
	{ "line_1",			::lv_example_line_1				},
	{ "list_1",			::lv_example_list_1				},
	{ "list_2",			::lv_example_list_2				},
	{ "lottie_1",		::lv_example_lottie_1			},
	{ "lottie_2",		::lv_example_lottie_2			},
	{ "menu_1",			::lv_example_menu_1				},
	{ "menu_2",			::lv_example_menu_2				},
	{ "menu_3",			::lv_example_menu_3				},
	{ "menu_4",			::lv_example_menu_4				},
	{ "menu_5",			::lv_example_menu_5				},
	{ "msgbox_1",		::lv_example_msgbox_1			},
	{ "msgbox_2",		::lv_example_msgbox_2			},
	{ "obj_1",			::lv_example_obj_1				},
	{ "obj_2",			::lv_example_obj_2				},
	{ "obj_3",			::lv_example_obj_3				},
	{ "roller_1",		::lv_example_roller_1			},
//	{ "roller_2",		::lv_example_roller_2			},
	{ "roller_3",		::lv_example_roller_3			},
	{ "scale_1",		::lv_example_scale_1			},
	{ "scale_2",		::lv_example_scale_2			},
	{ "scale_3",		::lv_example_scale_3			},
	{ "scale_4",		::lv_example_scale_4			},
	{ "scale_5",		::lv_example_scale_5			},
	{ "scale_6",		::lv_example_scale_6			},
	{ "scale_7",		::lv_example_scale_7			},
	{ "scale_8",		::lv_example_scale_8			},
	{ "scale_9",		::lv_example_scale_9			},
	{ "slider_1",		::lv_example_slider_1			},
	{ "slider_2",		::lv_example_slider_2			},
	{ "slider_3",		::lv_example_slider_3			},
	{ "slider_4",		::lv_example_slider_4			},
	{ "span_1",			::lv_example_span_1				},
	{ "spinbox_1",		::lv_example_spinbox_1			},
	{ "spinner_1",		::lv_example_spinner_1			},
	{ "switch_1",		::lv_example_switch_1			},
	{ "switch_2",		::lv_example_switch_2			},
	{ "table_1",		::lv_example_table_1			},
	{ "table_2",		::lv_example_table_2			},
	{ "tabview_1",		::lv_example_tabview_1			},
	{ "tabview_2",		::lv_example_tabview_2			},
	{ "textarea_1",		::lv_example_textarea_1			},
	{ "textarea_2",		::lv_example_textarea_2			},
	{ "textarea_1",		::lv_example_textarea_1			},
	{ "textarea_1",		::lv_example_textarea_1			},
	{ "textarea_3",		::lv_example_textarea_3			},
	{ "tileview_1",		::lv_example_tileview_1			},
	{ "win_1",			::lv_example_win_1				},
};

void Setup(Display& display);

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
#if 1
	ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
#else
	ILI9488 display(spi1, 320, 480, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9488::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
#endif
	display.Initialize(Display::Dir::Rotate90);
	touchScreen.Initialize(display);
	//touchScreen.Calibrate(display);
	LVGL::Initialize(5);
	LVGL::Adapter lvglAdapter;
	lvglAdapter.EnableDoubleBuff(true).AttachOutput(display);
	lvglAdapter.AttachInput(touchScreen);
	lvglAdapter.AttachInput(UART::Default);
	Setup(display);
	for (;;) Tickable::Tick();
}

void Setup(Display& display)
{
	int nCols = 4;
	int nEntries = count_of(entryTbl);
	int nEntriesPerCol = nEntries / (nCols - 1);
	::printf("--------\n");
	for (int iRow = 0; iRow < nEntriesPerCol; iRow++) {
		for (int iCol = 0; iCol < nCols; iCol++) {
			int idx = iCol * nEntriesPerCol + iRow;
			if (idx >= nEntries) break;
			const Entry& entry = entryTbl[idx];
			::printf("%3d:%-30s", idx + 1, entry.name);
		}
		::printf("\n");
	}
	char buff[64];
	::printf("Enter Number: ");
	for (;;) {
		int nChars = 0;
		for (;;) {
			char ch = ::getchar();
			if ('0' <= ch && ch <= '9') {
				buff[nChars++] = ch;
				::putchar(ch);
			} else if (ch == '\r') {
				::printf("\n");
				buff[nChars] = '\0';
				break;
			}
		}
		int idx = ::strtol(buff, nullptr, 10);
		if (1 <= idx && idx < nEntries) {
			(*entryTbl[idx - 1].func)();
			break;
		}
	}
}
