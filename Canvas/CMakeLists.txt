add_library(jxglib_Canvas INTERFACE)

target_include_directories(jxglib_Canvas INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Canvas INTERFACE
	jxglib_Common
	jxglib_Drawable
	jxglib_Font
	jxglib_Image)

target_sources(jxglib_Canvas INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Canvas.cpp)
