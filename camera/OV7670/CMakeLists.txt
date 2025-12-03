add_library(jxglib_Device_OV7670 INTERFACE)

target_include_directories(jxglib_Device_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Device_OV7670 INTERFACE
	jxglib_Common)

target_sources(jxglib_Device_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/OV7670.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Device_OV7670 INTERFACE)

target_include_directories(jxglib_ShellCmd_Device_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Device_OV7670 INTERFACE
	jxglib_Device_OV7670
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Device_OV7670 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Device_OV7670.cpp)
