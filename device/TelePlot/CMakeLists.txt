add_library(jxglib_TelePlot INTERFACE)

target_include_directories(jxglib_TelePlot INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_TelePlot INTERFACE
	jxglib_UART)

target_sources(jxglib_TelePlot INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/TelePlot.cpp)
