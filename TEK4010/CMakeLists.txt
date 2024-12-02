add_library(jxglib_TEK4010 INTERFACE)

target_include_directories(jxglib_TEK4010 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TEK4010 INTERFACE
	jxglib_UART)

target_sources(jxglib_TEK4010 INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TEK4010.cpp)
