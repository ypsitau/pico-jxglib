add_library(pico_jxglib_ST7789 INTERFACE)

target_include_directories(pico_jxglib_ST7789 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_jxglib_ST7789 INTERFACE
	hardware_spi
	pico_jxglib_Common
	pico_jxglib_Font)

target_sources(pico_jxglib_ST7789 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ST7789.cpp)
