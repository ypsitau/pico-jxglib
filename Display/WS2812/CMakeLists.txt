add_library(jxglib_Display_WS2812 INTERFACE)

target_include_directories(jxglib_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Display_WS2812 INTERFACE
	jxglib_Device_WS2812
	jxglib_Canvas
	jxglib_Display)

target_sources(jxglib_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WS2812.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Display_WS2812 INTERFACE)

target_include_directories(jxglib_ShellCmd_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Display_WS2812 INTERFACE
	jxglib_Shell
	jxglib_Display_WS2812)

target_sources(jxglib_ShellCmd_Display_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Display_WS2812.cpp)
