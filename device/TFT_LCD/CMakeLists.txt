add_library(jxglib_TFT_LCD INTERFACE)

target_include_directories(jxglib_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TFT_LCD INTERFACE
	hardware_spi
	jxglib_Display)

target_sources(jxglib_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TFT_LCD.cpp)

# jxglib_ST7735 and jxglib_ST7789 are just aliases.
add_library(jxglib_ST7735 INTERFACE)
add_library(jxglib_ST7789 INTERFACE)
target_link_libraries(jxglib_ST7735 INTERFACE jxglib_TFT_LCD)
target_link_libraries(jxglib_ST7789 INTERFACE jxglib_TFT_LCD)
