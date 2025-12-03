add_library(jxglib_Camera INTERFACE)

target_include_directories(jxglib_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Camera INTERFACE
	jxglib_Common)

target_sources(jxglib_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Camera.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Camera INTERFACE)

target_include_directories(jxglib_ShellCmd_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Camera INTERFACE
	jxglib_Camera
	jxglib_ImageFile
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Camera INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Camera.cpp)

#------------------------------------------------------------------------------
add_subdirectory(OV7670)
