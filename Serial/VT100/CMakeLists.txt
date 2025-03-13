add_library(jxglib_VT100 INTERFACE)

target_include_directories(jxglib_VT100 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_VT100 INTERFACE
	jxglib_Printable
	jxglib_Keyboard)

target_sources(jxglib_VT100 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/VT100.cpp)
