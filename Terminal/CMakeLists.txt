add_library(jxglib_Terminal INTERFACE)

target_include_directories(jxglib_Terminal INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Terminal INTERFACE
	jxglib_Canvas
	jxglib_Printable)

target_sources(jxglib_Terminal INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Terminal.cpp)
