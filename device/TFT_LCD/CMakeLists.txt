add_library(jxglib_TFT_LCD INTERFACE)

target_include_directories(jxglib_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TFT_LCD INTERFACE
	hardware_spi
	jxglib_DMA
	jxglib_Display)

target_sources(jxglib_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TFT_LCD.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ST7735 INTERFACE)

target_link_libraries(jxglib_ST7735 INTERFACE
	jxglib_TFT_LCD)

#------------------------------------------------------------------------------
add_library(jxglib_ST7789 INTERFACE)

target_link_libraries(jxglib_ST7789 INTERFACE
	jxglib_TFT_LCD)

#------------------------------------------------------------------------------
add_library(jxglib_ILI9341 INTERFACE)

target_link_libraries(jxglib_ILI9341 INTERFACE
	jxglib_TFT_LCD
	jxglib_TSC2046)
