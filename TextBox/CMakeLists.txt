add_library(jxglib_TextBox INTERFACE)

target_include_directories(jxglib_TextBox INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TextBox INTERFACE
	jxglib_Drawable)

target_sources(jxglib_TextBox INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TextBox.cpp)
