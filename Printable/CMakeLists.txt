add_library(jxglib_Printable INTERFACE)

target_include_directories(jxglib_Printable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Printable INTERFACE
	jxglib_UART)

target_sources(jxglib_Printable INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Printable.cpp)
