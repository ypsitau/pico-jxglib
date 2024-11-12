add_library(jxglib_SSD1306 INTERFACE)

target_include_directories(jxglib_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_SSD1306 INTERFACE
	hardware_i2c
	jxglib_Common
	jxglib_Drawable
	jxglib_Font
	jxglib_Image)

target_sources(jxglib_SSD1306 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/SSD1306.cpp)
