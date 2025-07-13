add_library(jxglib_PIO INTERFACE)

target_include_directories(jxglib_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_PIO INTERFACE
	hardware_pio
	jxglib_Common)

target_sources(jxglib_PIO INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/PIO.cpp)
