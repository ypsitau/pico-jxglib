add_library(jxglib_CmdLine INTERFACE)

target_include_directories(jxglib_CmdLine INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_CmdLine INTERFACE
	jxglib_Terminal)

target_sources(jxglib_CmdLine INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/CmdLine.cpp)

add_library(jxglib_CmdLine_BuiltIn INTERFACE)

target_include_directories(jxglib_CmdLine_BuiltIn INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_CmdLine_BuiltIn INTERFACE
	jxglib_CmdLine)

target_sources(jxglib_CmdLine_BuiltIn INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/CmdLine_BuiltIn.cpp)
