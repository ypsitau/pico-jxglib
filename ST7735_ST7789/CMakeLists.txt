add_library(jxglib_ST7735_ST7789 INTERFACE)

target_include_directories(jxglib_ST7735_ST7789 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ST7735_ST7789 INTERFACE
	hardware_spi
	jxglib_Common
	jxglib_Drawable
	jxglib_Font
	jxglib_Image)

target_sources(jxglib_ST7735_ST7789 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ST7735_ST7789.cpp)

# jxglib_ST7735 and jxglib_ST7789 are just aliases.
add_library(jxglib_ST7735 INTERFACE)
add_library(jxglib_ST7789 INTERFACE)
target_link_libraries(jxglib_ST7735 INTERFACE jxglib_ST7735_ST7789)
target_link_libraries(jxglib_ST7789 INTERFACE jxglib_ST7735_ST7789)
