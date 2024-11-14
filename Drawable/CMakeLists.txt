add_library(jxglib_Drawable INTERFACE)

target_include_directories(jxglib_Drawable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Drawable INTERFACE
	jxglib_Common
	jxglib_Font
	jxglib_Image)

target_sources(jxglib_Drawable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Drawable.cpp)
