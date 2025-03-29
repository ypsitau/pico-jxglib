add_library(jxglib_CmdLine INTERFACE)

target_include_directories(jxglib_CmdLine INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_CmdLine INTERFACE
	jxglib_Common)

target_sources(jxglib_CmdLine INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/CmdLine.cpp)
