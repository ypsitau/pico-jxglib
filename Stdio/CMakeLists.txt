add_library(jxglib_Stdio INTERFACE)

target_include_directories(jxglib_Stdio INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Stdio INTERFACE
	jxglib_VT100)

target_sources(jxglib_Stdio INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Stdio.cpp)
