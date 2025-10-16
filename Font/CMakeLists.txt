add_library(jxglib_Font INTERFACE)

target_include_directories(jxglib_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Font INTERFACE
	jxglib_Common)

target_sources(jxglib_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Font.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Font INTERFACE)

target_include_directories(jxglib_ShellCmd_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Font INTERFACE
	jxglib_Font
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Font INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Font.cpp)
