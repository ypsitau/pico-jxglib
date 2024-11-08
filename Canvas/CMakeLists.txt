add_library(pico_jxglib_Canvas INTERFACE)

target_include_directories(pico_jxglib_Canvas INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(pico_jxglib_Canvas INTERFACE
	pico_jxglib_Font
	pico_jxglib_Image)

target_sources(pico_jxglib_Canvas INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Canvas.cpp)
