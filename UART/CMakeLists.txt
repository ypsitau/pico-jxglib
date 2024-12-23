add_library(jxglib_UART INTERFACE)

target_include_directories(jxglib_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_UART INTERFACE
	jxglib_Common
	jxglib_Stream)

target_sources(jxglib_UART INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/UART.cpp)
