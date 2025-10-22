add_library(jxglib_WS2812 INTERFACE)

target_include_directories(jxglib_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_WS2812 INTERFACE
	jxglib_PIO)

target_sources(jxglib_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WS2812.cpp)
