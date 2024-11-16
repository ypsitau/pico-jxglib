cmake_minimum_required(VERSION 3.12)

project(jxglib C CXX ASM)

add_subdirectory(Button)
add_subdirectory(Canvas)
add_subdirectory(Common)
add_subdirectory(Display)
add_subdirectory(Drawable)
add_subdirectory(Font)
add_subdirectory(Image)

add_subdirectory(LCD1602)
add_subdirectory(SSD1306)
add_subdirectory(StepMotor)
add_subdirectory(TextBox)
add_subdirectory(TFT_LCD)

add_library(jxglib INTERFACE)

target_link_libraries(jxglib INTERFACE
	jxglib_Button
	jxglib_Canvas
	jxglib_Common
	jxglib_Display
	jxglib_Drawable
	jxglib_Font
	jxglib_Image
	jxglib_TextBox)

add_library(jxglib_device INTERFACE)
	
target_link_libraries(jxglib_device INTERFACE
	jxglib_LCD1602
	jxglib_SSD1306
	jxglib_StepMotor
	jxglib_TFT_LCD)
