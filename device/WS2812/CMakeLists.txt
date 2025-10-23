add_library(jxglib_WS2812 INTERFACE)

target_include_directories(jxglib_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_WS2812 INTERFACE
	jxglib_PIO)

target_sources(jxglib_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WS2812.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_WS2812 INTERFACE)

target_include_directories(jxglib_ShellCmd_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_WS2812 INTERFACE
	jxglib_WS2812
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_WS2812 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_WS2812.cpp)
