add_library(jxglib_Dislay_TFT_LCD INTERFACE)

target_include_directories(jxglib_Dislay_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Dislay_TFT_LCD INTERFACE
	hardware_spi
	jxglib_DMA
	jxglib_Display)

target_sources(jxglib_Dislay_TFT_LCD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TFT_LCD.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_Display_ST7735 INTERFACE)

target_link_libraries(jxglib_Display_ST7735 INTERFACE
	jxglib_Dislay_TFT_LCD)

#------------------------------------------------------------------------------
add_library(jxglib_Display_ST7789 INTERFACE)

target_link_libraries(jxglib_Display_ST7789 INTERFACE
	jxglib_Dislay_TFT_LCD)

#------------------------------------------------------------------------------
add_library(jxglib_Display_ILI9341 INTERFACE)

target_link_libraries(jxglib_Display_ILI9341 INTERFACE
	jxglib_Dislay_TFT_LCD
	jxglib_TouchScreen_TSC2046)

#------------------------------------------------------------------------------
add_library(jxglib_Display_ILI9488 INTERFACE)

target_link_libraries(jxglib_Display_ILI9488 INTERFACE
	jxglib_Dislay_TFT_LCD
	jxglib_TouchScreen_TSC2046)
