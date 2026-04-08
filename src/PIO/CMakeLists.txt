add_library(jxglib_PIO INTERFACE)

target_include_directories(jxglib_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_PIO INTERFACE
	hardware_pio
	jxglib_Common)

target_sources(jxglib_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/PIO.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_PIO INTERFACE)

target_include_directories(jxglib_ShellCmd_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_PIO INTERFACE
	hardware_pio
	jxglib_ShellCmd_Basic
	jxglib_PIO)

target_sources(jxglib_ShellCmd_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_PIO.cpp)
