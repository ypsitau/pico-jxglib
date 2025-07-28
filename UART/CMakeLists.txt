add_library(jxglib_UART INTERFACE)

target_include_directories(jxglib_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_UART INTERFACE
	jxglib_Common)

target_sources(jxglib_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/UART.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_UART INTERFACE)

target_include_directories(jxglib_ShellCmd_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_UART INTERFACE
	jxglib_UART
	jxglib_GPIOInfo
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_UART.cpp)
