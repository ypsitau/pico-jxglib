add_library(jxglib_Shell INTERFACE)

target_include_directories(jxglib_Shell INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Shell INTERFACE
	jxglib_Terminal)

target_sources(jxglib_Shell INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Shell.cpp)

add_library(jxglib_ShellCmd_Basic INTERFACE)

target_include_directories(jxglib_ShellCmd_Basic INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Basic INTERFACE
	jxglib_Shell)

target_sources(jxglib_ShellCmd_Basic INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Basic.cpp)
