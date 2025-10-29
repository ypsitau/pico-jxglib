add_library(jxglib_Display_WS2812 INTERFACE)

target_include_directories(jxglib_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Display_WS2812 INTERFACE
	jxglib_Common)

target_sources(jxglib_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WS2812.cpp)
