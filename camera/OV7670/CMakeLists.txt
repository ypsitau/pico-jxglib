add_library(jxglib_Camera_OV7670 INTERFACE)

target_include_directories(jxglib_Camera_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Camera_OV7670 INTERFACE
	jxglib_Camera)

target_sources(jxglib_Camera_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/OV7670.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Camera_OV7670 INTERFACE)

target_include_directories(jxglib_ShellCmd_Camera_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Camera_OV7670 INTERFACE
	jxglib_Camera_OV7670
	jxglib_ShellCmd_Camera)

target_sources(jxglib_ShellCmd_Camera_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Camera_OV7670.cpp)
