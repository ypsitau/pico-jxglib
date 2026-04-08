add_library(jxglib_LineBuff INTERFACE)

target_include_directories(jxglib_LineBuff INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_LineBuff INTERFACE
	jxglib_Common)

target_sources(jxglib_LineBuff INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/LineBuff.cpp)
